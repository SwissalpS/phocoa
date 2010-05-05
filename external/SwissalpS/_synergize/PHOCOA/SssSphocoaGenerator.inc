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

	// __construct()
	public function SssSphocoaGenerator() {} // SssSphocoaGenerator

	// dealloc
	static public function bail() {	echo chr(10) . 'SssSphocoaGenerator exiting...' . chr(10); exit(0); } // bail

	// cli 'inlet'
	static public function runCLI() {
		$a = $_SERVER['argv'];
		$sInvocation = array_shift($a); // get $0 path
		$sPathInvocationBase = null;
		static $sAppleScriptletBit = null; if (!$sAppleScriptletBit)
		$sAppleScriptletBit = '/SssSphocoaGen/newSssSphocoaProjectFromPlist.app/Contents/Resources/newSssSphocoaProject';
		if (false !== strstr($sInvocation, $sAppleScriptletBit)) {
			$sPathInvocationBase = realpath(substr($sInvocation, 0, strlen($sInvocation) - strlen($sAppleScriptletBit)));
		} // if got an optional source path because run from winthin
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

			else if (('-noInteraction' == $s)
				|| ('-nointeraction' == $s)) $bUseAllDefaults = true;
				// just use defaults for all files from now on

			else if (('-withInteraction' == $s)
				|| ('-withinteraction' == $s)) $bUseAllDefaults = false;
				// ask for each item for all files from now on

			else $aPaths[] = array($s, $bUseAllDefaults, $sPathDest);
				// got a path to a plist (if correct syntax etc) theoretically could be a string xml:plist

		} // loop all args building aPaths array

		$aFails = array();$iYes = 0;
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

	// edit this function to accomodate default values
	static public function defaultPlist() {
		static $a = null;
		if (!$a) {
			$sPathSssSFrameWork = '/usr/share/php/SwissalpS';
			$a = array(
					'sPathDefaultBaseTemplate'
					=> '/usr/share/php/SwissalpS/_synergize/PHOCOA/SssSphocoaGenTemplates/SssSphocoaApp',
					'sPathPhocoaFrameWork'
					=> '/usr/share/php/phocoa', // FRAMEWORK_DIR', '##PHOCOA_BASE_DIR##
					'sPathSssSFrameWork'
					=> $sPathSssSFrameWork, // ##SwissalpS_FRAMEWORK_DIR##
					'sPathSmartyFrameWork'
					=> //$sPathSssSFrameWork . '/_synergize/PHOCOA/Smarty', // ##SMARTY_DIR##
					'/usr/share/php/Smarty',
					'sPathPropelFrameWork'
					=> //$sPathSssSFrameWork . '/_synergize/PHOCOA/propel', // ##PROPEL_FRAMEWORK_DIR##
					'/usr/share/php/propel',
					'sPathPearLog'
					=> //$sPathSssSFrameWork . '/_synergize/PHOCOA', // ##PEAR_LOG_FRAMEWORK_DIR##
					'/usr/share/php',
					'sDBtype' => 'mysql', // 'pgsql'; // 'mysql'; // 'mssql'; // 'sqllite'; // 'ldap'; //
					'sDBhost' => 'debian2',
					'sDBuser' => 'phocoa',
					'sDBpass' => 'phocoa',
					'sDBname' => 'phocoa',
					'sIP' => '127.0.0.1',
					'sHost' => 'localhost',
					'iPort' => 80,
					'sAppName' => 'MyPHPapp');
		} // if not yet set

		return new SssS_Plist($a);
	} // defaultPlist

	// main interactive wizard
	static public function makeNewSssSphocoaProject($bUseAllDefaults = false) {

		self::makeNewSssSphocoaProjectWithPlist(self::defaultPlist(), $bUseAllDefaults);

	} // makeNewSssSphocoaProject

	// make from settings in a plist file interactively changing or complementing arguments
	static public function makeNewSssSphocoaProjectWithPlist($mPlist, $bUseAllDefaults = false, $sPathDest = false) {

		static $nl = null; if (!$nl) $nl = chr(10);

		$sArgType = gettype($mPlist);
		// mPlist is plist string or path
		if (('string' == $sArgType) || ('array' == $sArgType))
			$oP = new SssS_Plist($mPlist);

		else if (empty($mPlist))
			$oP = self::defaultPlist();

		else $oP = $mPlist; // hopefully is an SssS_Plist or subclass

		// some default paths

		// what to copy...
		$sPathDefaultBaseTemplate = $oP->get('sPathDefaultBaseTemplate');

		// where to point it to
		$sPathPhocoaFrameWork	= $oP->get('sPathPhocoaFrameWork');
		$sPathSssSFrameWork		= $oP->get('sPathSssSFrameWork');
		$sPathSmartyFrameWork	= $oP->get('sPathSmartyFrameWork');
		$sPathPropelFrameWork	= $oP->get('sPathPropelFrameWork');
        $sPathPearLog			= $oP->get('sPathPearLog');

		// some other default values
		// for propel -conf.xml
		$sDBtype = $oP->get('sDBtype');
		$sDBhost = $oP->get('sDBhost'); //phocoaDBhost';
		$sDBuser = $oP->get('sDBuser'); //DBuser';
		$sDBpass = $oP->get('sDBpass'); //DBpass';
		$sDBname = $oP->get('sDBname'); //DBname';

		// for apache httpd.conf
		$sIP	= $oP->get('sIP');
		$sHost	= $oP->get('sHost');
		$iPort	= $oP->get('iPort');

		// default appName
		$sAppName = $oP->get('sAppName');

		// where are we?
		$sPathCurentWorkingDirectory = getcwd();

		// welcome....
		echo $nl . '     SssSphocoaGenerator::makeNewSssSphocoaProject()' . $nl;

		$oRunner = new SssS_ShellScriptRunnerForDummies();

	if ($bUseAllDefaults) {
		echo 'not interacting for values...' . $nl;
		$sPathOrig = $sPathDefaultBaseTemplate;
		$sPathDest = ($sPathDest)
				?  SssS_CLI_Tools::removeTrailingSlash($sPathDest, DIR_SEP) . DIR_SEP . $sAppName
				: $oP->getOrSet('sPathDest', $sPathCurentWorkingDirectory . DIR_SEP . $sAppName);
		$sPathLog = $sPathDest . DIR_SEP . 'log';
		$sPathDestAppRoot = $sPathDest . DIR_SEP . $sAppName;
	} else {
	// get template path
		// normally gets parts from sPathPhocoaFrameWork. we have our templates
		$sPathOrig = SssS_CLI_Tools::removeTrailingSlash(
								SssS_CLI_Tools::getInputCLI(
										'Path of template Base Structure:',
										$sPathDefaultBaseTemplate), DIR_SEP);
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
		$sAppName = SssS_CLI_Tools::getInputCLI(
									$nl . 'Name of new App:', $sAppName);
		$oP->set('sAppName', $sAppName);

	// get target path
		// ##PHOCOA_APP_CONTAINER_DIR##
		$sPathDest = SssS_CLI_Tools::removeTrailingSlash(
								SssS_CLI_Tools::getInputCLI(
										$nl . 'Path of new App:',
										$sPathCurentWorkingDirectory . DIR_SEP
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
			$sIP = SssS_CLI_Tools::getInputCLI($nl . 'IP:', $sIP);
			$iPort = intval(
					SssS_CLI_Tools::getInputCLI($nl . 'port:', $iPort));
			$sHost = SssS_CLI_Tools::getInputCLI($nl . 'hostname:', $sHost);
			echo $nl;
		} // if not cli

	// ask if uses propel
		$sPathLog = $sPathDest . DIR_SEP . 'log';
		$sUsePropel = SssS_CLI_Tools::getChoiceCLI('Use propel for db?',
				array('no' => 'NO', 'yes' => 'YES'), 'yes');
		if ('yes' == strtolower($sUsePropel)) {
			// get db props
			$sPathLog = SssS_CLI_Tools::getInputCLI(
											$nl . 'log path:', $sPathLog);
			$oP->set('sPathLog', $sPathLog);
			echo $nl;
			$sDBtype = SssS_CLI_Tools::getChoiceCLI('Choose database type:',
					array('pgsql' => 'Postgres', 'mysql' => 'MySQl',
						  'mssql' => 'mssql', 'sqllite' => 'sqllite',
						   'ldap' => 'ldap'), $sDBtype);
			$oP->set('sDBtype', $sDBtype);
			$sDBname = SssS_CLI_Tools::getInputCLI(
												$nl . 'DB name:', $sDBname);
			$oP->set('sDBname', $sDBname);
			$sDBuser = SssS_CLI_Tools::getInputCLI(
											$nl . 'DB username:', $sDBuser);
			$oP->set('sDBuser', $sDBuser);
			$sDBpass = SssS_CLI_Tools::getInputCLI(
											$nl . 'DB password:', $sDBpass);
			$oP->set('sDBpass', $sDBpass);
			$sDBhost = SssS_CLI_Tools::getInputCLI(
											$nl . 'DB hostname:', $sDBhost);
			$oP->set('sDBhost', $sDBhost);
		} // if propel

		echo $nl;

	} // if ask for anything

	// copy the bare-bone structure
		// make destination path
		$bRes = $oRunner->makePath($sPathDest);
		$bPHPcompatiblePathDest = true;
		if ((!is_dir($sPathDest)) && $bRes) {
			echo 'destination path is not PHP compatible, problems to be'
					. ' expected' . $nl;
			$bPHPcompatiblePathDest = false;
		} // if php compatible path destination

		// make log dir
		$oRunner->makePath($sPathDest . DIR_SEP . 'log');
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

		// make sure there isn't any old symlink
		$oRunner->doScript('rm "' . $sPathDestAppRoot . DIR_SEP . 'bin' . DIR_SEP . 'run"');
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
		$oRunner->doScript('rm "' . $sPathWebAppConfSyml . '";');
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
		$oRunner->doScript('rm "' . $sPathGen . 'newSssSphocoaProject";');
		// make symlink to newSssSphocoaProject in SssSphocoaGen
		$oRunner->symlinkToAt($sPathASnew, $sPathGen . 'newSssSphocoaProject');

		$aSearches = array('##PHOCOA_BASE_DIR##', '##PHOCOA_PROJECT_NAME##',
				'##PHOCOA_APP_CONTAINER_DIR##', '##PHOCOA_APP_DIR##',
				'##SwissalpS_FRAMEWORK_DIR##', '##PEAR_LOG_FRAMEWORK_DIR##',
				'##PROPEL_FRAMEWORK_DIR##', '##DATE_STAMP##',
				'##SERVER_IP##', '##SERVER_PORT##', '##SERVER_NAME##',
				'##LOG_DIR##', '##DB_NAME##', '##PROPEL_DATABASE##',
				'##DB_USER##', '##DB_HOST##', '##DB_PASS##',
				'##SMARTY_DIR##', '##DIR_SEP##', '##PHP_BIN##',
				'/*##RUN_BLOCKER##', '##RUN_BLOCKER##*/'); // last 2 need no replacement

		$aReplacements = array($sPathPhocoaFrameWork, $sAppName,
				$sPathDest, $sPathDestAppRoot,
				$sPathSssSFrameWork, $sPathPearLog,
				$sPathPropelFrameWork, gmdate('Ymd_his'),
				$sIP, $iPort, $sHost,
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
						$sPathASnew) as $sPath) {
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

} // SssSphocoaGenerator
/* * * *\ SssSphocoaGenerator.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
