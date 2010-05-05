<?php
/**
  * This file contains delegate implementations for the basic parts of this cli Application.
  */


// custom WFWebApplication delegate
class MyApplicationDelegate
{

    /**
     *  Hook to call the initialize method fo the web application.
     *  Applications will typically initialize DB stuff here.
     */
    function initialize() {
        // manifest core modules that we want to use -- if you don't want people to access a module, remove it from this list!
        $webapp = WFWebApplication::sharedApplication();
        $webapp->addModulePath('login', FRAMEWORK_DIR . '/modules/login');
        $webapp->addModulePath('menu', FRAMEWORK_DIR . '/modules/menu');
        $webapp->addModulePath('css', FRAMEWORK_DIR . '/modules/css');
        $webapp->addModulePath('examples', FRAMEWORK_DIR . '/modules/examples');

        // load propel
        //Propel::init(PROPEL_CONF);

        $this->bRunning = false;

    } // initialize

    function defaultInvocationPath() {
        return 'examples/widgets/toc';
    } // defaultInvocationPath

    // switch between different skin catalogs; admin, public, partner reporting, etc
    function defaultSkinDelegate() {
        return 'simple';
    } // defaultSkinDelegate

	function handleUncaughtException($e) {
		if (!$this->bRunning) return false;

		echo chr(10) . '!!' . chr(10) . $e . chr(10) . '!!' . chr(10);
		return true;
	} // handleUncaughtException

	function handleError($errNum, $errString, $file, $line, $contextArray) {
		echo '!#!' . chr(10) . $errNum . ' ' . $errString . chr(10) . '!#!';
	} // handleError

	function handleCLIRequest() {

		$this->bRunning = true;
        static $errset = null;
		static $sWelcome = null;
		static $sPrompt = null;
		static $oPiSR = null;

		if (!$oPiSR) {
			$oPiSR = new SssS_PHPinlineScriptRunner($this);
			$sPrompt = 'PiSR> ';
			$sWelcome = 'SssS_PHPinlineScriptRunner: have nothing to say
		except what you told me:' . chr(0x0a);
			$errset = E_ERROR | E_CORE_ERROR | E_COMPILE_ERROR;
       		if (defined("E_RECOVERABLE_ERROR")) $errset |= E_RECOVERABLE_ERROR;
		} // first call

		$lastHandler = set_error_handler(array($this, 'handleError'), $errset);

        $bReturn = false;

		echo $sWelcome;

		$a = $_SERVER['argv'];
		//array_shift($a); // get (rid of) invocation path

		foreach ($a as $s) { echo $s . chr(10); }

		echo chr(10);

		while (true) {
			echo $sPrompt;
			$sInput =  fgets(STDIN);

			// catch ctl-d or other errors
			if (false === $sInput) { echo '--got false input--' . chr(10); $bReturn = true; break; }

        	$sInput = trim($sInput);

        	if ('help' == $sInput) { echo $this->helpCLI(); continue; }

        	if (in_array($sInput, array('iphp', 'stop'))) { echo '--handing over to iphp--' . chr(10); break; }

        	if (in_array($sInput, array('exit', 'end', 'quit'))) { echo '--quitting--' . chr(10); $bReturn = true; break; }

			$aRes = $oPiSR->doScript($sInput, false); // true); // @-muting eval

			if (isset($aRes['scriptOutput'])) echo '=>' . $aRes['scriptOutput'] . '<=' . chr(10);

			if (isset($aRes['scriptReturn'])) var_dump($aRes['scriptReturn']) . chr(10);

		} // loop

        set_error_handler($lastHandler, $errset);

		return $bReturn; // false; // true; // return false to have WFRequestController deal with the cli call (if using from _synergize/PHOCOA then it starts iphp session after running 'webapp') ~due to change, might stay
	} // handleCLIRequest

	function helpCLI() {
		return ' * * SssS_PHPinlineScriptRunner.inc
 * * was worth a try. Works for correctly when given correct code syntax.
 * * Can be used for interaktive shell. Also take a look at iphp which is
 * * different mainly that it doesn\'t crash the mothership quit as much :-)
 * * The crucial difference is that this class uses eval() -> inline
 * * while iphp uses exec() -> runs on seperate thread from file rebuilding
 * * per successfull call. This is a little disk intensive but has a clear
 * * stableizing advantage.
 * * I have tested the $_-feature and did not like it, that\'s why this class
 * * does not have a "last result" variable. It works more like a bash would
 * * in that you must echo or return a value to be printed (less screen filling
 * * when working with objects). With "return" the value is printed using
 * * var_dump().
 * *
 * * The advantage of eval() is that you can define new functions on the fly:
 * * enter "function help() { echo "hi there :-)";}" to see what I mean type the
 * * equivelant in iphp prepending some value to satisfy $_:
 * *  "3; function help() { echo "hi there :-)";}" so far so good. Now comes the
 * * difference, type "help()" in both script runners. iphp will roll up some
 * * error while SssS_PHPinlineScriptRunner will print "hi there :-)".
 * *
 * * to have a value in aResults[\'scriptReturn\'] your snippet must actually
 * * return something e.g. \'$a = 45*123; return $a;
 * *
 * * + you can instantiate new objects modify existing.
 * * + declare functions and classes but be aware not to redefine as that
 * *   crashes uncatchably.
 * *
 * * $this is SssS_PHPinlineScriptRunner object if you omit $this-> and just
 * * type $a, it will be converted to $this->oDelegate->a (or if no delegate
 * * just $this->a). $$a becomes $$this->oDelegate->a resp $$this->a
 * *
 * * $_a stays $_a which unless it\'s a global (e.g. $_SERVER) it will not be
 * * accessable outside the snippet scope. To keep this speedy do not declare
 * * functions and classes or other loops (unless you prepend function vars with _)
 * * wherever the snippet contains $this, it is not touched same goes for $this->oDelegate
 * *
 * * @version 20100502_203256 + treatScript() $a -> $this->a conversions
 * * @version 20091104_143909 (CC) Luke JZ aka SwissalpS
 ';
	} // helpCLI

    function autoload($className) {

        $requirePath = NULL;

    	switch ($className) {
            // Custom Classes - add in handlers for any custom classes used here.
            case 'Propel':
                $requirePath = 'propel/Propel.php';
                break;
        }

        if ($requirePath) {
            require($requirePath);
            return true;
        }

        return false;
    } // autoload


    /**
     *  Hook to provide opportunity for the web application to munge the session config before php's session_start() is called.
     */
    function sessionWillStart() {

    }

    /**
     *  Hook to provide opportunity for the web application to munge the session data after php's session_start() is called.
     */
    function sessionDidStart() {

    } // sessionDidStart

    /**
     *  Hook to call the authorizationInfoClass method fo the web application.
     *  @see WFWebApplicationDelegate::authorizationInfoClass()
     */
    function authorizationInfoClass() {

        return 'WFAuthorizationInfo';

    } // authorizationInfoClass

} // MyApplicationDelegate
?>
