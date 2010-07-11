<?php
/* * * *
 * * SssS_IncludeThesePathsToPHPIncludePath.inc
 * *
 * * no autoload support but is a great help when initiating it
 * *
 * * @version 20100501_004024 (CC) Luke JZ aka SwissalpS
 * * * */
if (!function_exists('SssS_IncludeThesePathsToPHPIncludePath')) {
	/**
	 * SssS_IncludeThesePathsToPHPIncludePath($mPaths, $bPrependWithoutChecking = false)
	 * makes sure the given paths are added to PHPs include_path
	 * uses ini_set('include_path', ...) and ini_get('include_path')
	 *
	 * @param mixed $mPaths array of string paths or 1 path string
	 * @param mixed $bPrependWithoutChecking bool when true just prepends the paths in the order they came to the current include path
	 *
	 * @return mixed null on failure uint of how many added
	 *
	 * @uses PHP - keep it that way
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_005024 (CC)2007 Luke JZ aka SwissalpS
	 */
	function SssS_IncludeThesePathsToPHPIncludePath($mPaths, $bPrependWithoutChecking = false, $bRemoveIfExisting = false) {
		// nonsense -> go home
		if (empty($mPaths)) return null;

		// only one path as string supplied...
		if ('string' == gettype($mPaths)) $mPaths = array($mPaths);

		// final type check
		if ('array' != gettype($mPaths)) return null;

		// get current include path
		$sIncludePath = ini_get('include_path');

		// add ':' if not yet
		$sPS = PATH_SEPARATOR;
		if ($sPS !== substr($sIncludePath, -1)) $sIncludePath .= $sPS;

		// prepare new path string
		$sNewIncludePath = ($bPrependWithoutChecking) ? '' : ($bRemoveIfExisting) ? '' : $sIncludePath;

		// add any missing paths
		$iCount = 0;
		foreach($mPaths as $sPath) {
			// append ':' if not yet
			if ($sPS !== substr($sPath, -1)) $sPath .= $sPS;

			if ($bPrependWithoutChecking) {
				$sNewIncludePath .= $sPath;
				$iCount++;

			} else {
				// check existance
				if (false === strstr($sIncludePath, $sPath)) {
					$sNewIncludePath .= $sPath;
					$iCount++;

				} // if missing found
				else if ($bRemoveIfExisting)
					$sIncludePath = str_replace($sPath, '', $sIncludePath);
					// remove already existing if so

			} // if $bPrependWithoutChecking
		} // loop all paths

		$sNewIncludePath .= ($bPrependWithoutChecking || $bRemoveIfExisting) ? $sIncludePath : '';

		// set the new path
		if (false === ini_set('include_path', $sNewIncludePath)) return null;

		return $iCount;
	} // SssS_IncludeThesePathsToPHPIncludePath
} // if not yet defined function SssS_IncludeThesePathsToPHPIncludePath

// prepend no matter what
if (!function_exists('SssS_PrependThesePathsToPHPIncludePath')) {
	function SssS_PrependThesePathsToPHPIncludePath($mPaths) {
		return SssS_IncludeThesePathsToPHPIncludePath($mPaths, true);
	} // SssS_PrependThesePathsToPHPIncludePath
} // if not yet defined function SssS_PrependThesePathsToPHPIncludePath

// prepend no matter what and also remove them then remove any in mPaths2go
// returns array ( iCountAdded , iCountRemoved )
if (!function_exists('SssS_PrependThesePathsToPHPIncludePathAndRemove')) {
	function SssS_PrependThesePathsToPHPIncludePathAndRemove($mPaths, $mPaths2go = '') {
		$iCountAdded = SssS_IncludeThesePathsToPHPIncludePath($mPaths, false, true);

		// had enough -> go home
		if (empty($mPaths2go)) return $iCountAdded;

		// only one path as string supplied...
		if ('string' == gettype($mPaths2go)) $mPaths2go = array($mPaths2go);

		// final type check
		if ('array' != gettype($mPaths2go)) return iCountAdded;

		// remove any paths in mPaths2go
		$aGo = array();
		$sPS = PATH_SEPARATOR;
		foreach($mPaths2go as $sPath) {
			// remove any prepending ':' // append ':' if not yet
			$aGo[] = trim($sPath, $sPS) . $sPS;
		} // loop all paths

		$sNewIncludePath .= ($bPrependWithoutChecking || $bRemoveIfExisting) ? $sIncludePath : '';

		// set the new path removeing any in aGo
		return (false === ini_set('include_path', str_replace($aGo, '', ini_get('include_path')))) ? null : $iCountAdded;

	} // SssS_PrependThesePathsToPHPIncludePathAndRemove
} // if not yet defined function SssS_PrependThesePathsToPHPIncludePathAndRemove


/*/ example adds both paths to include path if not yet there
$sSwissalpSFrameworkPath = "/Volumes/UNIX/public/SssS";
$mRes = SssS_IncludeThesePathsToPHPIncludePath(array($sSwissalpSFrameworkPath,
													'/Volumes/UNIX'));

/* * * *\ SssS_IncludeThesePathsToPHPIncludePath.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
