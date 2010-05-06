<?php
/* * * *
 * * SssS_FileGetPutContents.inc
 * * 
 * * if file_put_contents isn't defined we need our own
 * * same thing with file_get_contents
 * *
 * * note: tho they mimic PHP's counterparts, they are not necessarily
 * *	to be considered equals
 * * 
 * * (c) 20091024_191155 + file_get/put_contents_safely
 * * (c) 2008 Luke J Zimmermann aka SwissalpS
 */
if (!function_exists('file_put_contents')) {
/*Description
int file_put_contents ( string $filename, mixed $data [, int $flags [, resource $context]] )

This function is identical to calling fopen(), fwrite() and fclose() successively to write data to a file. 

If filename does not exist, the file is created. Otherwise, the existing file is overwritten, unless the FILE_APPEND flags is set. 

Return Values
The function returns the number of bytes that were written to the file, or FALSE on failure.

That's what it is since PHP 5
my version bellow should run on older php versions
functionallity is being added as time and requests meet
flags are at this stage: any value over 0 means to append
*/
	// boolean false or integer file_put_contents(string sPathFile, string sContents, integer iFlags, resource rContext)
	function file_put_contents($sPathFile = "", $sContents = "", $iFlags = 0, $rContext = -1) {
		// valid filname?
		if (("" == $sPathFile) || (!is_writable($sPathFile))) return false;
		// open handle
		$rHandle = (0 == $iFlags) ? fopen($sPathFile, "wb") : fopen($sPathFile, "ab");
		// check handle
		if (!$rHandle) return false;
		// write to file, hold result to pass on
		$mResult = fwrite($rHandle, $sContents);
		fclose($rHandle);
		return $mResult;
	} // file_put_contents
} // file_put_contents didn't exist yet

if (!function_exists('file_get_contents')) {
/* Description
string file_get_contents ( string $filename [, int $flags [, resource $context [, int $offset [, int $maxlen]]]] )

This function is similar to file(), except that file_get_contents() returns the file in a string, starting at the specified offset up to maxlen bytes.
On failure, file_get_contents() will return FALSE. 
That's what it is since PHP 4.0.3
my version bellow should run on older php versions
functionallity is being added as time and requests meet
flags are ignored at present mainly because older systems don't support these functions anyway
*/
	// boolean false or string file_get_contents(string sPathFile, boolean bUseIncludePath, resource rContext, integer iOffset, integer iMaxLen)
	function file_get_contents($sPathFile = "", $bUseIncludePath = false, $rContext = NULL, $iOffset = 0, $iMaxLen = -1) {
		// valid filname?
		if (("" == $sPathFile) || (!is_file($sPathFile))) return false;
		// open file handle
		$rHandle = fopen($sPathFile, 'rb');
		if (!$rHandle) return false;
		// Trash offset
		$sTrash = $sContents = '';
		while ((!feof($rHandle)) && ($iOffset > strlen($sTrash))) {
			$sTrash .= fread($rHandle, ($iOffset - strlen($sTrash)));
		}
		$iMaxLen = (0 < $iMaxLen) ? $iMaxLen : filesize($sPathFile);
		
		while ((!feof($rHandle)) && ($iMaxLen > strlen($sContents))) {
			$sContents .= fread($rHandle, ($iMaxLen - strlen($sContents)));
		}
		fclose($rHandle);
		return $sContents;
	} // file_get_contents
} // file_get_contents didn't exist yet


if (!function_exists('file_put_contents_safely')) {
	/**
	 * file_put_contents_safely($sPath, $mData, $sMode = 'ab', $iLock = LOCK_EX, $iRetry = -1)
	 * write data to file safely locking and respecting locked files
	 *
	 * @param string $sPath path to file
	 * @param mixed $mData the data to write
	 * @param string $sMode the fopen mode string (defaults to: 'ab')
	 * @param string $iLock the flock value. _EX or _EX | _NB make sense others don't for writing (defaults to: 'LOCK_EX')
	 * {@param int $iRetry how often to try. defaults to infinite (-1)} not yet defined and tested
	 *
	 * @return int/boolean int length on success or false: failure to write
	 *
	 * @note iLock must be same on all participants or it won't work! Read the php manual carefully especially when working on Microsoft
	 * @uses something - uses_desciption_here
	 *
	 * @version 20091021_101653 (CC)2009 Luke JZ aka SwissalpS
	 */
	function file_put_contents_safely($sPath, $mData,
			$sMode = 'ab', $iLock = LOCK_EX, $iTimeOutMilSec = 1000) {
		
		// check the mode, we might not support such as x
		if (0 < stripos(' ' . $sMode, 'x')) return false;
		// or r?!
		if (0 < stripos(' ' . $sMode, 'r')) return false;
		// we can ignore + as we are only writing
		//$bPlus = (0 < strpos(' ' . $sMode, '+'));
		$sBin = (0 < stripos(' ' . $sMode, 'b')) ? 'b' : ''; // actually on windows it wouldn't work as of yesterday
		$bPointer2zero = (0 < stripos(' ' . $sMode, 'w'));
		
		// Let's just hope the others paramaters are correct
		
		// flock only makes sense with a or ab
		$sMode = 'a' . $sBin;
		// let's get a handle without disturbing ie mode is a or ab
		if ($rF = fopen($sPath, $sMode)) {
			$iStartTime = microtime(); // keep track of time
			$iMaxTime = 1000;
			// alternativly we could use a count out
			$iMaxRetries = 100; // when using count out approach
			$iRetries = 0;
			
			do {
				$bCanNotWrite = !flock($rF, $iLock);
				if ($bCanNotWrite) usleep(round(rand(0, 100) * 1000));
			} while ($bCanNotWrite and ((microtime() - $iStartTime) < $iMaxTime)); // for time out based
			// for count out approach you would
			// } while ($bCanNotWrite and ($iRetries <= $iMaxRetries)));
			
			if ($bCanNotWrite) {
				//throw user_notification
				trigger_error('unable to recieve file lock for write acces on file: '
						. $sPath . ' Timed or counted out. Data may be lost', E_USER_NOTICE);
				fclose($rF);
				return false;
			} // if failed to get acces lock
			
			if ($bPointer2zero) {
				// flush file
				$bTrunc = ftruncate($rF, 0); // <-- erase the contents ala 'w'
			} // if zero pointer mode (overwrite) this is the mode that needs this kind of function most
			
			// write the data
			$mRes = fwrite($rF, $mData);
			
			// release lock and close handle
			flock($rF, LOCK_UN);
			fclose($rF);
			return $mRes;
		} // if got file handle
		return false; // failed
	} // file_put_contents_safely
} // file_put_contents_safely didn't exist yet


if (!function_exists('file_get_contents_safely')) {
	/**
	 * file_get_contents_safely($sPath, $sMode = 'rb', $iLock = LOCK_SH, $iTimeOutMilSec = 1000)
	 * read contents of file respecting flock locks and waiting
	 * until lock is opened or timed out
	 *
	 * @param string $sPath path of the file to read
	 * @param string $sMode 'r' or 'rb' (defaults to: 'rb')
	 * @param string $iLock LOCK_SH or LOCK_EX (defaults to: LOCK_SH)
	 * @param int $iTimeOutMilSec max time to retry (defaults to: 1000)
	 *
	 * @return bool | string false on fail, the contents as string on success
	 *
	 * @version 20091024_191327 (CC)2007 Luke JZ aka SwissalpS
	 */
	function file_get_contents_safely($sPath, $sMode = 'rb',
			$iLock = LOCK_SH, $iTimeOutMilSec = 1000) {
		
		if (!file_exists($sPath)) return false;
		if (!is_readable($sPath)) return false;
		
		// check the mode, we might not support such as x
		if (false !== stripos($sMode, 'x')) return false;
		// or w?!
		if (false !== stripos($sMode, 'w')) return false;
		// or a?!
		if (false !== stripos($sMode, 'a')) return false;
		// we can ignore + as we are only reading
		//$bPlus = (0 < strpos(' ' . $sMode, '+'));
		$sBin = (false !== stripos($sMode, 'b')) ? 'b' : ''; // actually on windows it wouldn't work as of yesterday
		
		// flock reading only makes sense with LOCK_SH or LOCK_EX
		if (!in_array($iLock, array(LOCK_SH, LOCK_EX))) return false;
		
		// Let's just hope the others paramaters are correct
		
		// flock reading only makes sense with 'r' or 'rb' or you would write your own
		$sMode = 'r' . $sBin;
		// let's get a handle without disturbing ie mode is r or rb
		if ($rF = fopen($sPath, $sMode)) {
			$iStartTime = microtime(); // keep track of time
			$iMaxTime = 1000;
			// alternativly we could use a count out
			$iMaxRetries = 100; // when using count out approach
			$iRetries = 0;
			
			do {
				$bCanNotRead = !flock($rF, $iLock);
				if ($bCanNotRead) usleep(round(rand(0, 100) * 1000));
			} while ($bCanNotRead and ((microtime() - $iStartTime) < $iMaxTime)); // for time out based
			// for count out approach you would
			// } while ($bCanNotRead and ($iRetries <= $iMaxRetries)));
			
			if ($bCanNotRead) {
				//throw user notification
				trigger_error('unable to recieve file lock for read acces on file: '
						. $sPath . ' Timed or counted out.', E_USER_NOTICE);
				fclose($rF);
				return false;
			} // if failed to get acces lock
						
			// read the data
			$mData = '';
			while (!feof($rF)) {
				$mData .= fread($rF, 2048);
			} // while not EOF
			
			// release lock and close handle
			flock($rF, LOCK_UN);
			fclose($rF);
			return $mData;
		} // if got file handle
		return false; // failed
	} // file_get_contents_safely
} // file_get_contents_safely didn't exist yet
/* * * * \ SssS_FileGetPutContents.inc (c) 2008 Luke J Zimmermann aka SwissalpS / * * * */
?>
