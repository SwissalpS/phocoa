<?php
/* * * *
 * * SssSbootstrap.inc
 * *
 * * taken from phocoa/framework/bootstrap.php
 * * adapted so the autoloading also uses SwissalpS Framework
 * * and prefered class mapping
 * *
 * * @version 20100501_011353 (CC) Luke JZ aka SwissalpS
 * * * */
if (!defined('FRAMEWORK_DIR') || !defined('SwissalpS_FRAMEWORK_DIR')
		|| !defined('PROPEL_FRAMEWORK_DIR') || !defined('SMARTY_DIR')
		|| !defined('LOG_DIR') || !defined('RUNTIME_DIR')
		|| !defined('APP_ROOT') || !defined('WWW_ROOT')
		|| !defined('WEBAPP_DELEGATE') || !defined('SssS_APP_DELEGATE')
		|| !defined('IS_PRODUCTION'))
				throw new Exception('Error, conf not loaded?');
/**
 * PHOCOA has its own autoload infrastructure that is handled by WFWebApplication. So, this is the only require_once() we need to use in all PHOCOA framework code outside of bootstrapping this file.
 */
//require(FRAMEWORK_DIR . '/framework/util/WFIncluding.php');
//$ok = spl_autoload_register(array('WFIncluding', 'autoload'));
// leave a small as possible footprint in the global var space
$aTmp = array('bOK' => false, 'sPathSssSsynPhocoa' => SwissalpS_FRAMEWORK_DIR . DIR_SEP . '_synergize' . DIR_SEP . 'PHOCOA' . DIR_SEP);
require($aTmp['sPathSssSsynPhocoa'] . 'util' . DIR_SEP . 'WFIncluding.inc');
$aTmp['bOK'] = spl_autoload_register(array('WFIncluding', 'autoload'));
if (true !== $aTmp['bOK'])
		throw new Exception("Error registering WFIncluding::autoload()");

// This version number should be updated with each release - this version number, among other things, is used to construct version-unique URLs for static resources
// thus anytime anything in wwwroot/www/framework changes, this should be bumped. This version string should match [0-9\.]*
define('PHOCOA_VERSION', '0.3.6.000.1'); // 0.3.6.SwissalpS.1

require($aTmp['sPathSssSsynPhocoa'] . 'WFLog.inc');    // need this for the PEAR_LOG_* constants below, which can't autoload.
if (IS_PRODUCTION) {
    error_reporting(E_ALL);
    ini_set('display_errors', false);
    if (!defined('WF_LOG_LEVEL')) define('WF_LOG_LEVEL', PEAR_LOG_ERR);

} else {
    error_reporting(E_ALL); // | E_STRICT);
    ini_set('display_errors', true);
    if (!defined('WF_LOG_LEVEL')) define('WF_LOG_LEVEL', PEAR_LOG_DEBUG);

} // if production mode or not

// load the WFWebApplication so that it is initialized() before __autoload() is called for the first time.
// if we don't do this, classes attempted to autoload from WFWebApplication::initialize() will cause a fatal error.
//require(FRAMEWORK_DIR . '/framework/WFWebApplication.php');    // WFWebApplicationMain() can't autoload...
// start either as webapp or as cli app
require($aTmp['sPathSssSsynPhocoa'] . 'WFWebApplication.inc'); unset($aTmp);
WFWebApplication::sharedApplication(); // WFWebApplication::sharedWebApplication();

/* * * *\ SssSbootstrap.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
