<?php
/* * * *
 * * SssS_ShellScriptRunnerForDummies.inc
 * *
 * * extends SssS_ShellScriptRunner with usefull shortcuts that don't need to
 * * be in the base class
 * *
 * * @uses SssS_ShellScriptRunner
 * * @version 20100501_203928 (CC) Luke JZ aka SwissalpS
 * * * */

if (!defined('AUTO_INCLUDING')) {
	require_once('SssS_ShellScriptRunner.inc');
} // if not autoincluding but on php's include path

class SssS_ShellScriptRunnerForDummies extends SssS_ShellScriptRunner {
	/**
	 * SssS_ShellScriptRunnerForDummies()
	 * __construct()
	 *
	 * @param none
	 *
	 * @uses SssS_ShellScriptRunner - base class
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_204844 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function SssS_ShellScriptRunnerForDummies() {
		$this->SssS_ShellScriptRunner();
	} // SssS_ShellScriptRunnerForDummies constructor/initializer



	/**
	 * aliasAtOf($sPathDest, $sPathOrig, $bMuted = true)
	 * shell: ln -s "$sPathOrig" "$sPathDest"
	 * make an 'alias' to a file or folder at sPathDest.
	 *  sPathOrig is the already existing file/folder. Beware, will overwrite
	 *
	 * @param string $sPathOrig from path: file or folder to point to
	 * @param string $sPathDest to path: file (? or folder) to save the link at
	 * @param string $bMuted mute php error with @system (defaults to: true)
	 *
	 * @return bool success
	 *
	 * @uses SssS_ShellScriptRunnerForDummies->symlinkToAt() to run link script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_234330 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function aliasAtOf($sPathDest, $sPathOrig, $bMuted = true) {
		return $this->symlinkToAt($sPathOrig, $sPathDest, $bMuted);
	} // aliasAtOf

	/**
	 * copyPreserving($sPathOrig, $sPathDest, $bMuted = true)
	 * shell: cp -p -R "$sPathOrig" "$sPathDestAppRoot"
	 * does no checking if orig or dest exist. sLline() or sOut() should tell
	 * works better than PHPs copy()
	 *
	 * @param string $sPathOrig from path: file or folder
	 * @param string $sPathDest to path: file or folder
	 * @param bool $bMuted mute php error with @system (defaults to: true)
	 *
	 * @return bool success or not
	 *
	 * @uses SssS_ShellScriptRunner->doScript - to run copy script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_222305 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function copyPreserving($sPathOrig, $sPathDest, $bMuted = true) {
		if (empty($sPathOrig) || empty($sPathDest)) return false;
		$this->doScript('cp -p -R "' . $sPathOrig . '" "' . $sPathDest . '";',
																	$bMuted);
		return (0 === $this->iRes());
	} // copyPreserving

	/**
	 * isFolder($sPath)
	 * shell: [ -d "$sPath" ]
	 * PHPs is_dir() can't deal with spaces. this seems to. (on mac)
	 *
	 * @param string $sPath the path to check
	 *
	 * @return bool  true if is a directory false if not
	 *
	 * @uses SssS_ShellScriptRunner->doScript - to run test script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_205411 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function isFolder($sPath, $bMuted = true) {
		if (empty($sPath)) return false;
		$this->doScript('[ -d "' . $sPath . '" ];', $bMuted);
		return (0 === $this->iRes());
	} // isFolder

	/**
	 * isSomethingAtPath($sPath, $bMuted = true)
	 * shell: [ -e "$sPath" ]
	 * is there a file or folder or whatever at this path?
	 *
	 * @param string $sPath path to check
	 * @param string $bMuted mute php error with @system (defaults to: true)
	 *
	 * @return bool true means: yes there is something there
	 *
	 * @uses SssS_ShellScriptRunner->doScript - to run test script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_224933 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function isSomethingAtPath($sPath, $bMuted = true) {
		if (empty($sPath)) return false;
		$this->doScript('[ -e "' . $sPath . '" ];', $bMuted);
		return (0 === $this->iRes());
	} // isSomethingAtPath

	/**
	 * makePath($sPath, $bMuted = true)
	 * shell: mkdir -p "$sPath"
	 * creates all directories in path, if they don't yet exist
	 *
	 * @param string $sPath the path to create
	 * @param string $bMuted mute php error with @system (defaults to: true)
	 *
	 * @return bool true on success...
	 *
	 * @uses SssS_ShellScriptRunner->doScript - to run mkdir script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_220357 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function makePath($sPath, $bMuted = true) {
		if (empty($sPath)) return false;
		$this->doScript('mkdir -p "' . $sPath . '";', $bMuted);
		return (0 === $this->iRes());
	} // makePath

	/**
	 * moveOrRename($sPathOrig, $sPathDest, $bMuted = true)
	 * shell: mv "$sPathOrig" "$sPathDest"
	 * move or rename a file or folder
	 *
	 * @param string $sPathOrig from path: file or folder
	 * @param string $sPathDest to path: file or folder
	 * @param string $bMuted mute php error with @system (defaults to: true)
	 *
	 * @return bool true on success...
	 *
	 * @uses SssS_ShellScriptRunner->doScript - to run move script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_231537 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function moveOrRename($sPathOrig, $sPathDest, $bMuted = true) {
		if (empty($sPathOrig) || empty($sPathDest)) return false;
		$this->doScript('mv "' . $sPathOrig . '" "' . $sPathDest . '";',
																	$bMuted);
		return (0 === $this->iRes());
	} // moveOrRename

	/**
	 * symlinkToAt($sPathOrig, $sPathDest, $bMuted = true)
	 * shell: ln -s "$sPathOrig" "$sPathDest"
	 * make an 'alias' to a file or folder at sPathDest.
	 *  sPathOrig is the already existing file/folder. Beware, will overwrite
	 *
	 * @param string $sPathOrig from path: file or folder to point to
	 * @param string $sPathDest to path: file (? or folder) to save the link at
	 * @param string $bMuted mute php error with @system (defaults to: true)
	 *
	 * @return bool success
	 *
	 * @uses SssS_ShellScriptRunner->doScript - to run link script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_233730 (CC)2007 Luke JZ aka SwissalpS
	 */
	function symlinkToAt($sPathOrig, $sPathDest, $bMuted = true) {
		if (empty($sPathOrig) || empty($sPathDest)) return false;
		$this->doScript('ln -s "' . $sPathOrig . '" "' . $sPathDest . '";',
																	$bMuted);
		return (0 === $this->iRes());
	} // symlinkToAt

	/**
	 * whoami($bMuted = true)
	 * shell: whoami
	 * currently active user running the script or empty string on fail
	 *
	 * @param string $bMuted mute php error with @system (defaults to: true)
	 *
	 * @return string currently active user running the script or empty string on fail
	 *
	 * @uses SssS_ShellScriptRunner->doScript - to run whoami script
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_221029 (CC)2007 Luke JZ aka SwissalpS
	 */
	public function whoami($bMuted = true) {
		$this->doScript('whoami;', $bMuted);
		return (0 === $this->iRes()) ? strval($this->sLline()) : '';
	} // whoami

} // SssS_ShellScriptRunnerForDummies
/* * *\ SssS_ShellScriptRunnerForDummies END /* * */
/* * * *\ SssS_ShellScriptRunnerForDummies.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
