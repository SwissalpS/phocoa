<?php
/* * * *
 * * SssS_PHPinlineScriptRunner.inc
 * * was worth a try. Works for correctly when given correct code syntax.
 * * Can be used for interactive shell. Also take a look at iphp which is
 * * different mainly that it doesn't crash the mothership quite as much :-)
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
 * * to have a value in aResults['scriptReturn'] your snippet must actually
 * * return something e.g. '$a = 45*123; return $a;
 * *
 * * + you can instantiate new objects modify existing.
 * * + declare functions and classes but be aware not to redefine as that
 * *   crashes uncatchably.
 * *
 * * $this is SssS_PHPinlineScriptRunner object if you omit $this-> and just
 * * type $a, it will be converted to $this->oDelegate->a (or if no delegate
 * * just $this->a). $$a becomes $$this->oDelegate->a resp $$this->a
 * *
 * * $_a stays $_a which unless it's a global (e.g. $_SERVER) it will not be
 * * accessable outside the snippet scope. To keep this speedy do not declare
 * * functions and classes or other loops (unless you prepend function vars with _)
 * * wherever the snippet contains $this, it is not touched same goes for $this->oDelegate
 * *
 * * @version 20100502_203256 + treatScript() $a -> $this->a conversions
 * * @version 20091104_143909 (CC) Luke JZ aka SwissalpS
 * * * */

//require_once('SssS_FileGetPutContents.inc');



class SssS_PHPinlineScriptRunner {

	var $oDelegate = null; // my delegate if any to hold objects and values created without prefixing '$this'

	function SssS_PHPinlineScriptRunner($oDelegate = null) {

		$this->oDelegate = $oDelegate;

	} // SssS_PHPinlineScriptRunner

	public function fReset() {

		$this->sScript = '';

		$this->aResults = array(
				'scriptReturn'		=> null,
				'scriptOutput'		=> null);

	} // fReset



	public function doScript($sScript = '', $bMuted = true) {

		$this->fReset();

		$this->sScript = $this->treatScript($sScript);

		try {

			ob_start();

			$sReturn = ($bMuted) ? @eval($this->sScript) : eval($this->sScript);

			$sOut = ob_get_clean();

		} catch (Exception $e) {

			// somehow this text never shows up, but it does keep the mothership going (nothing can keep exit() from working
			//echo ' --- something bad happened ---' . "\n"
			//		. '   with PHPsnippet --->' . $sScript . '<-- ';

			if (!isset($sOut)) $sOut =
					' --- something bad happened ---' . "\n" .
					'   with PHPsnippet --->' . $sScript . '<-- ';

		} // try catch errors

		$this->aResults = array(
				//'scriptResult'		=> $__SwissalpS_PHPinlineScriptRunner_Eval_Result,
				'scriptReturn'		=> $sReturn,
				'scriptOutput'		=> $sOut);

		return $this->aResults;
	} // doScript

	public function treatScript($sScript) {
		$sAttachTo = '$this->' . ((is_object($this->oDelegate))
														? 'oDelegate->' : '');

		// TODO: detect function/class/loop variabes in case user is declaring new functions and classes (maybe first strip comments)

		$s = str_replace('$_', '####globalOrProtected####', $sScript);
		$s = str_replace('$this->oDelegate', '####oDelegate####', $s);
		$s = str_replace('$this', '####this####', $s);
		$s = str_replace('$$', '####string####', $s);

		$s = str_replace('$', $sAttachTo, $s);

		$s = str_replace('####string####', '$' . $sAttachTo, $s);
		$s = str_replace('####this####', '$this', $s);
		$s = str_replace('####oDelegate####', '$this->oDelegate', $s);
		$s = str_replace('####globalOrProtected####', '$_', $s);

		return $s . ';'; // can't hurt to add an extra ';'
	} // treatScript

	static function helpCLI() {
		static $s = null; if (!$s) $s =
' * * SssS_PHPinlineScriptRunner.inc
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
		return $s;
	} // helpCLI

    function __toString() { return print_r($this, true); } // __toString

} // SssS_PHPinlineScriptRunner

/* * * *\ SssS_PHPinlineScriptRunner.inc (CC)2009 Luke JZ aka SwissalpS /* * * */
?>
