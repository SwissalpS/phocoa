<?php
/* * * *
 * * SssS/_synergize/PHOCOA/SssSphocoaGenerator.inc
 * *
 * * to be run as cli
 * * guides thru the setup of a copy from template structure
 * * modifying as few options as needed
 * *
 * * developed and tested on a mac w OS X 10.5.x
 * * written mainly for own use... don't have much \ in my paths ;-)
 * * for now no smarty, propel support -> targeting cli app
 * *
 * * @todo TODO: read defaults from templates config
 * * @version 20100501_081510 (CC) Luke JZ aka SwissalpS
 * * * */
if (!defined('AUTO_INCLUDING')) {
	require_once('SssS_Plist.inc');
	require_once('SssS_CLI_Tools.inc');
	require_once('SssS_ShellScriptRunnerForDummies.inc');
} // if not autoincluding but on php's include path

if (!defined('DIR_SEP')) define('DIR_SEP', DIRECTORY_SEPARATOR);

class SssSphocoaGenerator {

	const sAppleScriptletBitNew = '/SssSphocoaGen/newSssSphocoaProjectFromPlist.app/Contents/Resources/newSssSphocoaProject';
	const sAppleScriptletBitInPlace = '/SssSphocoaGen/newSssSphocoaProjectFromPlist.app/Contents/Resources/remakeTemplatedFilesWithInfoFromPlist';

	// __construct()
	public function SssSphocoaGenerator() {} // SssSphocoaGenerator

	// dealloc
	static public function bail() {	echo chr(10) . 'SssSphocoaGenerator exiting...' . chr(10); exit(0); } // bail

	// main cli 'inlet'
	static public function runCLI() {
		$a = $_SERVER['argv'];
		$sInvocation = array_shift($a); // get $0 path
		//echo chr(10) . $sInvocation . '<-sInvocation-';
		$sPathInvocationBase = null;
		if (false !== strstr($sInvocation, self::sAppleScriptletBitNew)) {
			$sPathInvocationBase = substr($sInvocation, 0, strlen($sInvocation) - strlen(self::sAppleScriptletBitNew));
		} // if got an optional source path because run from winthin realpath()
		// AppleScript.app in a SssSphocoaGen directory.
		// That directory could be used as sPathDefaultBaseTemplate
		// if sPathDefaultBaseTemplate is missing in the plist

		$bUseAllDefaults = $sPathDest = false;
		$aPaths = array();
		while (0 < count($a)) {
			$s = array_shift($a);

			if ('help' == $s) {
				//self::showHelpCLI();
				self::bail();
			} // if help passed as argument

			if (('-target' == $s) && (0 < count($a)))
				$sPathDest = array_shift($a);
				// setting new sPathDest destination dir

			else if ((('-appName' == $s)
				|| ('-appname' == $s)) && (0 < count($a)))
				$sAppName = array_shift($a);
				// setting an app name by command line (not yet implemented) might never as editing the plists is so simple and gui-able

			else if (('-noInteraction' == $s)
				|| ('-nointeraction' == $s)) $bUseAllDefaults = true;
				// just use defaults for all files from now on

			else if (('-withInteraction' == $s)
				|| ('-withinteraction' == $s)) $bUseAllDefaults = false;
				// ask for each item for all files from now on

			else $aPaths[] = array($s, $bUseAllDefaults, $sPathDest);
				// got a path to a plist (if correct syntax etc) theoretically could be a string xml:plist

		} // loop all args building aPaths array

		$aFails = array(); $iYes = 0;
		foreach ($aPaths as $aPath) {
			list($sPath, $bUseAllDefaults, $sPathDest) = $aPath;
			// paths in $a may come quoted or escaped
			$sPath = ('"' == $sPath{0} || "'" == $sPath{0})
					? trim($sPath, "\" \t'") 		// trim whitespace and ' "
					: str_replace('\\', '', $sPath);// or remove \ escapes

			if (self::makeNewSssSphocoaProjectWithPlist($sPath, $bUseAllDefaults, $sPathDest, $sPathInvocationBase))
				$iYes++; // count success

			else
				$aFails[] = array('sPath' => $sPath,
						'bUseAllDefaults' => $bUseAllDefaults,
							  'sPathDest' => $sPathDest);
				// record fails for debugging at end

		} // loop all paths generating a project at each given set of values in plist or flagged form

		// good bye
		echo sprintf('%1$ssuccessful in %2$s out of %3$s files%1$s',
						chr(10), $iYes, count($aPaths));

		// debug fails if any
		if (!empty($aFails)) echo 'failed paths: ' . print_r($aFails, true);
		self::bail();
	} // runCLI

	// cli 'inlet'
	static public function runCLIinPlaceReplacement() {
		$a = $_SERVER['argv'];
		$sInvocation = array_shift($a); // get $0 path
		$sPathInvocationBase = null;
		if (false !== strstr($sInvocation, self::sAppleScriptletBitInPlace)) {
			$sPathInvocationBase = substr($sInvocation, 0, strlen($sInvocation) - strlen(self::sAppleScriptletBitInPlace));
		} // if got an optional source path because run from winthin realpath()
		// AppleScript.app in a SssSphocoaGen directory.
		// That directory could be used as sPathDefaultBaseTemplate
		// if sPathDefaultBaseTemplate is missing in the plist

		$aFails = array(); $iYes = 0; $iCount = count($a);
		while (0 < count($a)) {
			$s = array_shift($a);
			$mPlist = ('"' == $s{0} || "'" == $s{0})
					? trim($s, "\" \t'") 		// trim whitespace and ' "
					: str_replace('\\', '', $s);// or remove \ escapes --> this endangers sending clear xml plist todo: check xml header
			if (self::inPlaceRefreshTemplatesInPlist($mPlist, null, $sPathInvocationBase))
				$iYes++; // count success

			else
				$aFails[] = array('mPlist' => $mPlist,
					 'sPathInvocationBase' => $sPathInvocationBase);

		} // while have arguments

		// good bye
		echo sprintf('%1$ssuccessful in %2$s out of %3$s files%1$s',
						chr(10), $iYes, $iCount);

		// debug fails if any
		if (!empty($aFails)) echo 'failed paths: ' . print_r($aFails, true);
		self::bail();

	} // runCLIinPlaceReplacement

	// edit this function to accomodate default values
	static public function defaultPlist() {
		static $a = null;
		if (!$a) {

			$a = array();
			@include('SssSphocoaGeneratorDefault.conf');

		} // if not yet set

		return new SssS_Plist($a);
	} // defaultPlist

	// main interactive wizard
	static public function makeNewSssSphocoaProject($bUseAllDefaults = false) {

		self::makeNewSssSphocoaProjectWithPlist(self::defaultPlist(), $bUseAllDefaults);

	} // makeNewSssSphocoaProject

	// make from settings in a plist file interactively changing or complementing arguments
	static public function makeNewSssSphocoaProjectWithPlist(
			$mPlist = null, // SssS_Plist object a path or a plist xml string or an array
			$bUseAllDefaults = false,
			$sPathDest = false,
			$sPathInvocationBase = null) { // alternative root calculated from invocation path

		static $rts = null; static $ats = null; static $gi = null;
		static $nl = null; if (!$nl) {
			$nl = chr(10);
			$rts = 'removeTrailingSlash';
			$ats = 'addTrailingSlash';
			$gi = 'getInputCLI';
		} // first run

		if ($sPathInvocationBase)
			$sPathInvocationBase = SssS_CLI_Tools::$rts($sPathInvocationBase, DIR_SEP);

		$sArgType = gettype($mPlist);
		if (('object' == $sArgType) && is_subclass_of($mPlist, 'SssS_Plist')) {
			$oP = $mPlist;
			if ($sPathInvocationBase)
				$oP->getOrSet('sPathDefaultBaseTemplate', $sPathInvocationBase);

		} else { // is not an SssS_Plist or subclass
			// mPlist is plist string or path
			if (('string' == $sArgType) || ('array' == $sArgType)) {
				$oP = new SssS_Plist($mPlist);
				if ($sPathInvocationBase)
					$oP->getOrSet('sPathDefaultBaseTemplate', $sPathInvocationBase);

			} // if string (path or xml) or array as mPlist
			else {
				$oP = self::defaultPlist();
				if ($sPathInvocationBase)
					$oP->set('sPathDefaultBaseTemplate', $sPathInvocationBase);

			} // if mPlist type handling
		} // if no useable mPlist

		// some default paths

		// what to copy... (will ask for confirmation)
		$sPathDefaultBaseTemplate = $oP->get('sPathDefaultBaseTemplate');

		// where to point frameworks to (~required fields)
		$sPathPhocoaFrameWork	= $oP->get('sPathPhocoaFrameWork');
		$sPathSmartyFrameWork	= $oP->get('sPathSmartyFrameWork');
		$sPathPropelFrameWork	= $oP->get('sPathPropelFrameWork');
		$sPathHordeFrameWork	= $oP->get('sPathHordeFrameWork');
		$sPathSssSFrameWork		= $oP->get('sPathSssSFrameWork');
        $sPathPearLog			= $oP->get('sPathPearLog');

		// some other default values
		// for propel -conf.xml (will ask for confirmation)
		$sDBtype = $oP->get('sDBtype', 'mysql');
		$sDBhost = $oP->get('sDBhost', 'localhost'); //phocoaDBhost';
		$sDBuser = $oP->get('sDBuser'); //DBuser';
		$sDBpass = $oP->get('sDBpass'); //DBpass';
		$sDBname = $oP->get('sDBname'); //DBname';

		// for apache httpd.conf (will ask for confirmation)
		$sHTTPip	= $oP->get('sHTTPip', '127.0.0.1');
		$sHTTPHost	= $oP->get('sHTTPHost', 'localhost');
		$iHTTPPort	= $oP->get('iHTTPPort', 80);

		// default appName (will ask for confirmation)
		$sAppName = $oP->get('sAppName', 'PhocoaApp');

		// where are we?
		$sPathCurentWorkingDirectory = getcwd();

		// welcome....
		echo $nl .  $nl . '     SssSphocoaGenerator::makeNewSssSphocoaProject()' . $nl;

		$oRunner = new SssS_ShellScriptRunnerForDummies();

		$sPathOrig = ($sPathDefaultBaseTemplate)
			? SssS_CLI_Tools::$rts($sPathDefaultBaseTemplate, DIR_SEP) : ($sPathInvocationBase)
				? $sPathInvocationBase : '';

		$sPathDest = ($sPathDest)
				?  SssS_CLI_Tools::$ats($sPathDest, DIR_SEP) . $sAppName
				: $oP->getOrSet('sPathDest', $sPathCurentWorkingDirectory . DIR_SEP . $sAppName);

	if ($bUseAllDefaults) {
		echo 'not interacting for values...' . $nl;
		$sPathLog = $sPathDest . DIR_SEP . 'log';
		$sPathDestAppRoot = $sPathDest . DIR_SEP . $sAppName;
	} else {
	// get template path
		// normally gets parts from sPathPhocoaFrameWork. we have our templates
		$sPathOrig = SssS_CLI_Tools::$rts(SssS_CLI_Tools::$gi(
				'Path of template Base Structure (sPathDefaultBaseTemplate):',
										$sPathOrig), DIR_SEP);
		$oP->set('sPathDefaultBaseTemplate', $sPathOrig);

		// PHP is not letting me use paths with spaces...best not to have unsoported
		// chars as this will only make things worse down the line
		//if (!is_dir(escapeshellarg($sPathOrig))) {
		//	echo 'original template path not found: ' . escapeshellarg($sPathOrig);
		//	return false;
		//} // if no source
		if (!$oRunner->isFolder($sPathOrig)) {
			echo 'original template path not found: ' . $sPathOrig . $nl;
			return false;
		} // if no source;

	// get app name
		// ##PHOCOA_PROJECT_NAME##
		$sAppName = SssS_CLI_Tools::$gi(
								$nl . 'Name of new App (sAppName):', $sAppName);
		$oP->set('sAppName', $sAppName);

	// get target path
		// ##PHOCOA_APP_CONTAINER_DIR##
		$sPathDest = SssS_CLI_Tools::$rts(SssS_CLI_Tools::$gi(
										$nl . 'Path of new App (sPathDest):',
										substr($sPathDest, 0, strlen($sPathDest) - strlen(strrchr($sPathDest, DIR_SEP))) . DIR_SEP
										. $sAppName), DIR_SEP);
		$oP->set('sPathDest', $sPathDest);

		echo $nl;

		if ($oRunner->isSomethingAtPath($sPathDest)) {
			echo 'destination already exists: ' . $sPathDest . $nl;
			return false;
		} // if destination already exists;

	// set target app-root
		// APP_ROOT', '##PHOCOA_APP_DIR##
		$sPathDestAppRoot = $sPathDest . DIR_SEP . $sAppName;

	// get app type
		// for application delegate
		$sAppType = SssS_CLI_Tools::getChoiceCLI('Type of app (cli|web|both)',
				array('cli' => 'Command Line Tool', 'web' => 'WFWebApplication',
				'both' => 'both types'), 'both types');
		//echo 'currently only supporting cli, so set to cli' . $nl . $nl;
		//$sAppType = 'cli';

	// httpd.conf takes also servername, ip and port
		if ('Command Line Tool' != $sAppType) {
			$sHTTPip = SssS_CLI_Tools::$gi($nl . 'IP (sHTTPip):', $sHTTPip);
			$oP->set('sHTTPip', $sHTTPip);
			$iHTTPPort = intval(
					SssS_CLI_Tools::$gi($nl . 'port (iHTTPPort): ', $iHTTPPort));
			$oP->set('iHTTPPort, $iHTTPPort');
			$sHTTPHost = SssS_CLI_Tools::$gi(
									$nl . 'hostname (sHTTPHost):', $sHTTPHost);
			$oP->set('sHTTPHost', $sHTTPHost);
			echo $nl;
		} // if not cli

	// ask if uses propel
		$sPathLog = $sPathDest . DIR_SEP . 'log';
		$sUsePropel = SssS_CLI_Tools::getChoiceCLI('Use propel for db?',
				array('no' => 'NO', 'yes' => 'YES'), 'yes');
		if ('yes' == strtolower($sUsePropel)) {
			// get db props
			$sPathLog = SssS_CLI_Tools::$gi(
											$nl . 'log path:', $sPathLog);
			$oP->set('sPathLog', $sPathLog);
			echo $nl;
			$sDBtype = SssS_CLI_Tools::getChoiceCLI('Choose database type:',
					array('pgsql' => 'Postgres', 'mysql' => 'MySQl',
						  'mssql' => 'mssql', 'sqllite' => 'sqllite',
						   'ldap' => 'ldap'), $sDBtype);
			$oP->set('sDBtype', $sDBtype);
			$sDBname = SssS_CLI_Tools::$gi($nl . 'DB name:', $sDBname);
			$oP->set('sDBname', $sDBname);
			$sDBuser = SssS_CLI_Tools::$gi($nl . 'DB username:', $sDBuser);
			$oP->set('sDBuser', $sDBuser);
			$sDBpass = SssS_CLI_Tools::$gi($nl . 'DB password:', $sDBpass);
			$oP->set('sDBpass', $sDBpass);
			$sDBhost = SssS_CLI_Tools::$gi($nl . 'DB hostname:', $sDBhost);
			$oP->set('sDBhost', $sDBhost);
		} // if propel

		echo $nl;

	} // if ask for anything

	// copy the bare-bone structure
		// make destination path
		$bRes = $oRunner->makePath($sPathDest);
		$bPHPcompatiblePathDest = true;
		if ((!is_dir($sPathDest)) && $bRes) {
			echo $nl . $nl . 'destination path is not PHP compatible, problems'
					. ' to be expected' . $nl;
			$bPHPcompatiblePathDest = false;
		} // if php compatible path destination

		// make log dir
		$oRunner->makePath($sPathLog);
		// make runtime dir
		//$oRunner->makePath($sPathDest . DIR_SEP . 'runtime');
		// copy runtime dir
		$bRes = $oRunner->copyPreserving($sPathOrig . DIR_SEP . 'runtime',
								 		 $sPathDest . DIR_SEP . 'runtime');
		if (!$bRes) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			return false;
		} // if copy error

		// copy app dir -> Project dir and app dir MUST bear same name
		$sPathOrigAppRoot = $sPathOrig . DIR_SEP
									. substr(strrchr($sPathOrig, DIR_SEP), 1);
		$bRes = $oRunner->copyPreserving($sPathOrigAppRoot,	$sPathDestAppRoot);
		if (!$bRes) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			return false;
		} // if copy error
		// also here PHP fails
		//if (false === copy($sPathOrig, $sPathDest)) {
		//	echo 'error making copy';
		//	return false;
		//} // if copy fail

		// copy bin dir
		//if (!$oRunner->copyPreserving($sPathOrig . '/AppRoot/bin', $sPathDestAppRoot . '/bin')) { echo 'error with copy' . $nl; var_dump($oRunner->aResults); return false; } // if copy error

		// copy replicator dir holding info and applescript after the plists
		$sPathGen = $sPathDest . DIR_SEP . 'SssSphocoaGen';
		if (!$oRunner->copyPreserving(
						$sPathOrig . DIR_SEP . 'SssSphocoaGen', $sPathGen)) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			//return false;
		} // if copy error
		$sPathGen .= DIR_SEP;

		// copy runner.phps.SssStemplate -> sAppName
		$sPathRunner = $sPathDestAppRoot . DIR_SEP . 'bin' . DIR_SEP . $sAppName;
		$sPathRunnerOrig = $sPathDestAppRoot . DIR_SEP . 'bin' . DIR_SEP
													. 'runner.phps.SssStemplate';
		if (!$oRunner->copyPreserving($sPathRunnerOrig, $sPathRunner)) {//moveOrRename
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			return false;
		} // if rename error

		// make sure there isn't any old symlink or empty directory...
		$oRunner->doScript('rm -dfv "' . $sPathDestAppRoot . DIR_SEP . 'bin' . DIR_SEP . 'run"');
		// make a symlink for easy running
		$bRes = $oRunner->symlinkToAt($sPathRunner, $sPathDestAppRoot . DIR_SEP
													. 'bin' . DIR_SEP . 'run');
		if (!$bRes) {
			echo 'error with alias creation' . $nl; var_dump($oRunner->aResults);
			return false;
		} // if symlink error

	// config files

		$sPathConf = $sPathDestAppRoot . DIR_SEP . 'conf' . DIR_SEP;

		// copy propel-conf.xml.SssStemplate -> sAppName-conf.xml
		$sPathPropelConf = $sPathConf . $sAppName . '-conf.xml';
		$sPathPropelConfOrig = $sPathConf . 'propel-conf.xml.SssStemplate';
		if (!$oRunner->copyPreserving($sPathPropelConfOrig, $sPathPropelConf)) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			return false;
		} // if rename error

		// .htaccess.SssStemplate
		$sPathHTAconf = $sPathConf . '.htaccess';
		$sPathHTAconfOrig = $sPathConf . '.htaccess.SssStemplate';
		if (!$oRunner->copyPreserving($sPathHTAconfOrig, $sPathHTAconf)) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			//return false;
		} // if rename error

		// httpd.conf.SssStemplate
		$sPathHTTPdConf = $sPathConf . 'httpd.conf';
		$sPathHTTPdConfOrig = $sPathConf . 'httpd.conf.SssStemplate';
		if (!$oRunner->copyPreserving($sPathHTTPdConfOrig, $sPathHTTPdConf)) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			//return false;
		} // if rename error

		// copy SssSphocoaApp.conf.SssStemplate
		$sPathWebAppConf = $sPathConf . 'SssSphocoaApp.conf';
		$sPathWebAppConfSyml = $sPathConf . 'webapp.conf';
		$sPathWebAppConfOrig = $sPathConf . 'SssSphocoaApp.conf.SssStemplate';
		if (!$oRunner->copyPreserving($sPathWebAppConfOrig, $sPathWebAppConf)) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			//return false;
		} // if rename error

		// first remove old symlink
		$oRunner->doScript('rm -dfv "' . $sPathWebAppConfSyml . '";');
		// make symlink to SssSphocoaApp.conf for compatability with webapp.conf
		$oRunner->symlinkToAt($sPathWebAppConf, $sPathWebAppConfSyml);

		// copy AppleScripts php shell runner (on apple dir_sep is /)
		$sPathASres = $sPathGen . 'newSssSphocoaProjectFromPlist.app/Contents/Resources/';
		$sPathASnew = $sPathASres . 'newSssSphocoaProject';
		$sPathASnewOrig = $sPathASres . 'newSssSphocoaProject.SssStemplate';
		if (!$oRunner->copyPreserving($sPathASnewOrig, $sPathASnew)) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			//return false;
		} // if rename error

		// first remove old symlink
		$oRunner->doScript('rm -dfv "' . $sPathGen . 'newSssSphocoaProject";');
		// make symlink to newSssSphocoaProject in SssSphocoaGen
		$oRunner->symlinkToAt($sPathASnew, $sPathGen . 'newSssSphocoaProject');

		// copy inPlace AppleScripts php shell runner (on apple dir_sep is /)
		$sPathASres = $sPathGen . 'remakeTemplatedFilesWithInfoFromPlist.app/Contents/Resources/';
		$sPathASipNew = $sPathASres . 'remakeTemplatedFilesWithInfoFromPlist';
		$sPathASipNewOrig = $sPathASres . 'remakeTemplatedFilesWithInfoFromPlist.SssStemplate';
		if (!$oRunner->copyPreserving($sPathASipNewOrig, $sPathASipNew)) {
			echo 'error with copy' . $nl; var_dump($oRunner->aResults);
			//return false;
		} // if rename error

		// first remove old symlink
		$oRunner->doScript('rm -dfv "' . $sPathGen . 'remakeTemplatedFilesWithInfoFromPlist";');
		// make symlink to newSssSphocoaProject in SssSphocoaGen
		$oRunner->symlinkToAt($sPathASipNew, $sPathGen . 'remakeTemplatedFilesWithInfoFromPlist');

		$aSearches = array('##PHOCOA_BASE_DIR##', '##PHOCOA_PROJECT_NAME##',
				'##PHOCOA_APP_CONTAINER_DIR##', '##PHOCOA_APP_DIR##',
				'##SwissalpS_FRAMEWORK_DIR##', '##PEAR_LOG_FRAMEWORK_DIR##',
				'##PROPEL_FRAMEWORK_DIR##', '##HORDE_FRAMEWORK_DIR##',
				'##DATE_STAMP##',
				'##SERVER_IP##', '##SERVER_PORT##', '##SERVER_NAME##',
				'##LOG_DIR##', '##DB_NAME##', '##PROPEL_DATABASE##',
				'##DB_USER##', '##DB_HOST##', '##DB_PASS##',
				'##SMARTY_DIR##', '##DIR_SEP##', '##PHP_BIN##',
				'/*##RUN_BLOCKER##', '##RUN_BLOCKER##*/'); // last 2 need no replacement

		$aReplacements = array($sPathPhocoaFrameWork, $sAppName,
				$sPathDest, $sPathDestAppRoot,
				$sPathSssSFrameWork, $sPathPearLog,
				$sPathPropelFrameWork, $sPathHordeFrameWork,
				gmdate('Ymd_his'),
				$sHTTPip, $iHTTPPort, $sHTTPHost,
				$sPathLog, $sDBname, $sDBtype,
				$sDBuser, $sDBhost, $sDBpass,
				$sPathSmartyFrameWork, DIR_SEP,
				SssS_ShellScriptRunner::getPHPinterpreterPath()); // delete last 2

		// setup config file
		// shell script runner
		// .htaccess needs only ##PHOCOA_APP_DIR##
		// httpd.conf setup if needed or not
		// propel-conf.xml ##LOG_DIR## ##DB_NAME## ##PROPEL_DATABASE## ##DB_USER## ##DB_HOST## ##DB_PASS##
		// AppleScript droplet
		foreach (array($sPathWebAppConf, //webapp.conf',
						$sPathRunner, $sPathPropelConf,
						$sPathHTAconf, $sPathHTTPdConf,
						$sPathASnew, $sPathASipNew) as $sPath) {
			file_put_contents_safely($sPath,
					str_replace($aSearches, $aReplacements,
					file_get_contents_safely($sPath)), 'wb');
		} // replace all tokens in these files

		echo '--done, thanks & have fun--';


		// let's save a plist for remake and one for useAsTemplate
		$oP->set('sPathLog', null);
		$oP->saveTo($sPathGen . 'useTemplateThatMadeMe.plist');

		$oP->set('sPathDefaultBaseTemplate', $sPathDest);
		$oP->set('sPathDest', null);
		$oP->set('sAppName', $oP->get('sAppName') . 'Copy');

		$oP->saveTo($sPathGen . 'useMeAsTemplate.plist');

		return true;
	} // makeNewSssSphocoaProjectWithPlist

	// TODO:
	static function inPlaceRefreshTemplatesInPlist(
			$mPlist = null,
			$aFiles = null,
			$sPathInvocationBase = null) {
		if (!$mPlist) return null;

		static $rts = null; static $ats = null; static $gi = null;
		static $aSearches = null;
		static $nl = null; if (!$nl) {
			$nl = chr(10);
			$rts = 'removeTrailingSlash';
			$ats = 'addTrailingSlash';
			$gi = 'getInputCLI';
			$aSearches = array('##PHOCOA_BASE_DIR##', '##PHOCOA_PROJECT_NAME##',
				'##PHOCOA_APP_CONTAINER_DIR##', '##PHOCOA_APP_DIR##',
				'##SwissalpS_FRAMEWORK_DIR##', '##PEAR_LOG_FRAMEWORK_DIR##',
				'##PROPEL_FRAMEWORK_DIR##', '##HORDE_FRAMEWORK_DIR##',
				'##DATE_STAMP##',
				'##SERVER_IP##', '##SERVER_PORT##', '##SERVER_NAME##',
				'##LOG_DIR##', '##DB_NAME##', '##PROPEL_DATABASE##',
				'##DB_USER##', '##DB_HOST##', '##DB_PASS##',
				'##SMARTY_DIR##', '##DIR_SEP##', '##PHP_BIN##',
				'/*##RUN_BLOCKER##', '##RUN_BLOCKER##*/'); // last 2 need no replacement
		} // first run

		$sArgType = gettype($mPlist);
		if ('object' == $sArgType && is_subclass_of($mPlist, 'SssS_Plist'))
			$oP = $mPlist;

		else { // is not an SssS_Plist or subclass
			// mPlist is plist string or path
			if (('string' == $sArgType) || ('array' == $sArgType))
				$oP = new SssS_Plist($mPlist);
			// if string (path or xml) or array as mPlist

			else {
				$oP = self::defaultPlist();
				if ($sPathInvocationBase)
					$oP->set('sPathDefaultBaseTemplate', null);
			} // if use defaults

		} // if mPlist is is a SssS_Plist subclass or not

		$oP->getOrSet('sPathDefaultBaseTemplate', $sPathInvocationBase);

		$oPd = self::defaultPlist();
		$a = array_merge($oPd->a_plist, $oP->a_plist);
		$oP->set('', $a);

		// collect the information
		$sPathOrig = SssS_CLI_Tools::$rts($a['sPathDefaultBaseTemplate'], DIR_SEP);
		$sAppName = basename($sPathOrig);
		$sPathDestAppRoot = $sPathOrig . DIR_SEP . $sAppName;
		$sPathGen = $sPathOrig . DIR_SEP . 'SssSphocoaGen';

		if (empty($aFiles))
				$aFiles = self::findTemplatesInProject($sPathOrig);

		if (empty($aFiles))	return 0;

		$aReplacements = array(
			SssS_CLI_Tools::$rts($a['sPathPhocoaFrameWork']),
			$sAppName, $sPathOrig, $sPathDestAppRoot,
			SssS_CLI_Tools::$rts($a['sPathSssSFrameWork']),
			SssS_CLI_Tools::$rts($a['sPathPearLog']),
			SssS_CLI_Tools::$rts($a['sPathPropelFrameWork']),
			SssS_CLI_Tools::$rts($a['sPathHordeFrameWork']),
			gmdate('Ymd_his'),
			$oP->get('sHTTPip', '127.00.0.1'), $oP->get('iHTTPPort', 80),
			$oP->get('sHTTPHost', 'localhost'),
			$oP->get('sPathLog', $sPathOrig . DIR_SEP . 'log'),
			$oP->get('sDBname', 'phocoa'), $oP->get('sDBtype', 'mysql'),
			$oP->get('sDBuser', 'phocoa'), $oP->get('sDBhost', 'phocoa'),
			$oP->get('sDBpass', 'phocoa'),
			SssS_CLI_Tools::$rts($a['sPathSmartyFrameWork']),
			(isset($a['DIR_SEP']) ? $a['DIR_SEP'] : DIR_SEP), // todo: read this earlier
			SssS_ShellScriptRunner::getPHPinterpreterPath()); // delete last 2

		foreach ($aFiles as $sPath) {
			$sPath = trim($sPath);
			if (empty($sPath)) continue;
			$sNameTemplate = basename($sPath);
			$sName = basename($sPath, '.SssStemplate');
			if ('runner.phps' == $sName) $sName = $sAppName;
			if ('propel-conf.xml' == $sName) $sName = $sAppName . '-conf.xml';
			$sPath = dirname($sPath) . DIR_SEP;
			file_put_contents_safely($sPath . $sName,
					str_replace($aSearches, $aReplacements,
					file_get_contents_safely($sPath . $sNameTemplate)), 'wb');
		} // foreach path

		return true;
	} // refreshSetOfConfigToPlistValues or something of the kind to make an alternative .conf for different deployment or testing
	// scan current approot for any files ending with .SssStemplate
	// for each make a copy removing the suffix and overwrite existing

	static function findTemplatesInProject($sPathOrig = null) {
		static $oRunner = null; if (!$oRunner)
			$oRunner = self::sharedSssS_ShellScriptRunnerForDummies();

		if (!$oRunner->isFolder($sPathOrig)) return array();

		$oRunner->doScript('find "' . $sPathOrig . '" -name "*.SssStemplate"');

		return explode("\n", $oRunner->sOut());

	} // findTemplatesInProject

	static function sharedSssS_ShellScriptRunnerForDummies() {
		static $oRunner = null; if (!$oRunner)
			$oRunner = new SssS_ShellScriptRunnerForDummies();
		return $oRunner;
	} // sharedSssS_ShellScriptRunnerForDummies

} // SssSphocoaGenerator
/* * * *\ SssSphocoaGenerator.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
