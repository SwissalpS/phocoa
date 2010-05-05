<?php
/* * * *
 * * SssS_ShellScriptRunner.inc
 * *
 * * basically runs shell scripts as system calls. Either directly or by
 * * placing the script in a file first and changing it's mode to executable
 * *
 * * @version 20100502_175315 + added backgroundable and 2>&1 for file running.
 * * @version 20100501_200518 + added some quick getters iRes(), sOut() etc.
 * * @version 20091104_195900 + try catch and mute option in doScript(..,..)
 * * @version 20091024_202143 + file_get/put_contents_safely
 * * @version 20090823_151325 (CC) Luke JZ aka SwissalpS
 * * * */


require_once('SssS_FileGetPutContents.inc');



class SssS_ShellScriptRunner {

	// var chosen for all 3 to enable maximum flexibility
	// the current script or the one just executed
	var $sScript;
	// hash set and returned after execution with results
	var $aResults;
	// shebang paths per intepreter @see initShellScriptRunnerInterpreterPaths
	var $aShellScriptRunnerInterpreterPaths;

	function SssS_ShellScriptRunner() {

		//$this->fReset();

	} // SssS_ShellScriptRunner

	public function fReset() {

		$this->sScript = '';
		$this->aResults = array(
				'scriptExitCode'	=> null,
				'fullOutput'		=> null,
				'lastLine'			=> null);

	} // fReset



	public function doScript($sScript = '', $bMuted = false) {

		$this->initShellScriptRunnerInterpreterPaths();

		$this->fReset();

		$this->sScript = $sScript;

		try {

			ob_start();

			$sLastLine = ($bMuted)
					? @system($this->sScript, $iRes)
					: system($this->sScript, $iRes);

			$sOut = ob_get_clean();

		} catch (Exception $e) {

			if (!isset($sLastLine)) $sLastLine = '1';

			if (!isset($sOut)) $sOut =
					' --- something bad happened ---' . "\n" .
					'   with shell script --->' . $sScript . '<-- ';

		} // try catch errors

		// fill up our results array
		$this->aResults = array(
				'scriptExitCode'	=> $iRes,
				'fullOutput'		=> $sOut,
				'lastLine'			=> $sLastLine);

		return $this->aResults;
	} // doScript

// 20100501_200518 SwissalpS added some getters
	public function iRes() { return $this->scriptExitCode(); } // iRes
	public function exitCode() { return $this->scriptExitCode(); } // exitCode
	public function scriptExitCode() {
		if (!$this->aResults) return null;
		if (!isset($this->aResults['scriptExitCode'])) return null;
		return $this->aResults['scriptExitCode'];
	} // scriptExitCode
	public function sOut() { return $this->fullOutput(); } // sOut
	public function fullOutput() {
		if (!$this->aResults) return null;
		if (!isset($this->aResults['fullOutput'])) return null;
		return $this->aResults['fullOutput'];
	} // fullOutput
	public function sLline() { return $this->sLastLine(); } // sLline
	public function sLastLine() {
		if (!$this->aResults) return null;
		if (!isset($this->aResults['lastLine'])) return null;
		return $this->aResults['lastLine'];
	} // sLastLine
// 20100501_200518 SwissalpS added some getters

	// 20100502_175315 SwissalpS added $bConsolidateStdErrToStdOut and $bRunAsBackgroundProcess
	public function doScriptAsFile($s, $sPath = false, $iPerm = 700,
			$bConsolidateStdErrToStdOut = false, // 2>&1
			$bRunAsBackgroundProcess = false) { // &

		// ensure that the file ends with a carriage return
		if (chr(10) != substr($s, -1)) $sScript .= chr(10);

		// write it out carefully
		if (false == @file_put_contents_safely($sPath, $s, 'wb')) {

			// failed => set both string values to an error message
			$this->aResults['lastLine']
					= $this->aResults['fullOutput']
					= 'could not write file';

			return $this->aResults;

		} // if couldn't write file

		// attempt to chmod
		$this->doScript('chmod ' . $iPerm . ' ' . escapeshellarg($sPath));

		// abort if failed to chmod
		if (0 !== $this->aResults['scriptExitCode']) { // return false;

			// failed => set both string values to an error message
			$this->aResults['lastLine']
					= $this->aResults['fullOutput']
					= 'could not chmod file';

			return $this->aResults;

		} // if couldn't chmod file

		// prepare to run the file
		$sScript = escapeshellargument($sPath);

		if ($bConsolidateStdErrToStdOut) $sScript .= ' 2>&1';

		if ($bRunAsBackgroundProcess) $sScript .= ' &';

		// return the results of running the file
		return $this->doScript($sScript);

	} // doScriptAsFile

	// 20100502_175315 added by SwissalpS to have a central way for all interpreters making code more maintainable
	public function doScriptAsFileForInterpreter($s, $sPath = false,
			$sInterpreter = 'sh',
			$iPerm = 700,
			$bConsolidateStdErrToStdOut = false, // 2>&1
			$bRunAsBackgroundProcess = false) {

			if (!isset(
					$this->aShellScriptRunnerInterpreterPaths[$sInterpreter])) {
				$this->aResults = array(
					'scriptExitCode'	=> null,
					'fullOutput'		=> 'unknown interpreter chosen for SssS_ShellScriptRunner->doScriptAsFileForInterpreter'
					. "\n" . 'passed interpreter string: ' . $sInterpreter
					. "\n" . 'passed script: ' . $s
					. "\n" . 'target path: ' . $sPath,
					'lastLine'			=> 'unknown interpreter chosen for SssS_ShellScriptRunner->doScriptAsFileForInterpreter');
				return $this->aResults;
			} // if interpreter not in list

			$sScript = '#!'
				. $this->aShellScriptRunnerInterpreterPaths[$sInterpreter]
				. chr(10) . $s;

			return $this->doScriptAsFile($sScript, $sPath, $iPerm,
					$bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess);

	} // doScriptAsFileForInterpreter

	public function doScriptAsBashFile($s, $sPath = false, $iPerm = 700,
			$bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess) {

		return $this->doScriptAsFileForInterpreter($sScript, $sPath, 'bash',
				$iPerm, $bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess);

	} // doScriptAsBashFile



	public function doScriptAsPHPFile($s, $sPath = false, $iPerm = 700,
			$bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess) {

		return $this->doScriptAsFileForInterpreter($sScript, $sPath, 'php',
				$iPerm, $bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess);

	} // doScriptAsPHPFile



	public function doScriptAsPythonFile($s, $sPath = false, $iPerm = 700,
			$bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess) {

		return $this->doScriptAsFileForInterpreter($sScript, $sPath, 'python',
				$iPerm, $bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess);

	} // doScriptAsPythonFile



	public function doScriptAsRubyFile($s, $sPath = false, $iPerm = 700,
			$bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess) {

		return $this->doScriptAsFileForInterpreter($sScript, $sPath, 'ruby',
				$iPerm, $bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess);

	} // doScriptAsRubyFile



	public function doScriptAsShFile($s, $sPath = false, $iPerm = 700,
			$bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess) {

		return $this->doScriptAsFileForInterpreter($sScript, $sPath, 'sh',
				$iPerm, $bConsolidateStdErrToStdOut, $bRunAsBackgroundProcess);

	} // doScriptAsShFile



	function initShellScriptRunnerInterpreterPaths() {

		$this->aShellScriptRunnerInterpreterPaths = array(
				'bash'	=> '/bin/bash',
				'php'	=> $this->getPHPinterpreterPath(),//'/usr/bin/php',
				'python'=> '/usr/bin/python',
				'ruby'	=> '/usr/bin/ruby',
				'sh'	=> '/bin/sh');

	} // initBashLines

	// thanks to alan pinstein or whoever contributed (iphp::getDefaultPhpBin())
	static function getPHPinterpreterPath() {
        return PHP_BINDIR . DIRECTORY_SEPARATOR
        		. (('WIN' === strtoupper(substr(PHP_OS, 0, 3)))
        				? 'php.exe' : 'php');
    } // getPHPinterpreterPath

    function __toString() { return print_r($this, true); } // __toString

} // SssS_ShellScriptRunner


/* * * *\ SssS_ShellScriptRunner.inc (CC)2009 Luke JZ aka SwissalpS /* * * */
?>
