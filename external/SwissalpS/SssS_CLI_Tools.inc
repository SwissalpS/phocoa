<?php
/* * * *
 * * SssS_CLI_Tools.inc
 * *
 * * simple tools for basic cli apps
 * *
 * * more sophisticated options can be had with SssS_SocketController and
 * * SssS_ServerCommander classes using ANSI coloring etc.
 * *
 * * @used by _synergize/PHOCOA/SssSphocoaGenerator
 * * @version 20100501_083218 (CC) Luke JZ aka SwissalpS
 * * * */
class SssS_CLI_Tools {

	static public function getInputCLI($sPrompt, $sDefault) {
		if (77 < (strlen($sPrompt) + strlen($sDefault))) {
			fwrite(STDOUT, $sPrompt . chr(10) . '[' . $sDefault . '] ');
		} else {
			fwrite(STDOUT, $sPrompt . '[' . $sDefault . '] ');
		} // if long or short prompt

		$sReply = trim(fgets(STDIN));

		//fwrite(STDOUT, 'you said: ' . $sReply . chr(10));
		if (empty($sReply)) $sReply = $sDefault;

		fwrite(STDOUT,  'Using: ' . $sReply . chr(10));

		return $sReply;
	} // SssS_getInputCLI

/* this snippet shows the behaviour of getChoiceCLI()
	depending on the array format
// simple array
$a1 = array('one', 'two', 'three', 'four');

// caution! these can cause unexpected choices:
// mixed array with string, numerical and no indexs
$a2= array('one' => 'ONE longer', 'two', 5 => 'five', 'four' => 'four');

// string indexed array -> works great :-) returns the value not key as choice
$a3= array('one' => 'ONE longer', 'two' => 'too long', 'three' => 'tree', 'four' => 'four');
echo SssS_CLI_Tools::getChoiceCLI('prompt1>', $a1, 'four') . chr(10);
echo SssS_CLI_Tools::getChoiceCLI('prompt2>', $a2, 'four') . chr(10);
echo SssS_CLI_Tools::getChoiceCLI('prompt3>', $a3, 'four') . chr(10);
*/
	static public function getChoiceCLI($sPrompt, $aChoices, $sDefault = null) {
		if ('array' != gettype($aChoices)) return $sDefault;

		$iCount = count($aChoices);
		if (0 == $iCount) return $sDefault;

		$aChoicesValues = array_values($aChoices);
		$aChoicesKeys = array_keys($aChoices);
		if (null == $sDefault) $sDefault = $aChoicesValues[0];

		$sOut = '';
		for ($x = 0; $x < $iCount; $x++) {
			$sOut .= $x . ') ' . $aChoicesValues[$x] . ' (' . $aChoicesKeys[$x]
					. chr(10);
		} //
		fwrite(STDOUT, $sOut);

		$sPrompt .= '[' . $sDefault . '] ';

		do {
			fwrite(STDOUT, $sPrompt);
			$sReply = trim(fgets(STDIN));

			// special case 0
			if ('0' == $sReply) { $sReply = $aChoicesValues[0]; break; }

			// just return hit -> default
			if (empty($sReply)) { $sReply = $sDefault; break; }
//echo '--is not empty---';
			// a choice spellt out
			if (in_array($sReply, $aChoicesValues)) break;
//echo '--is not spellt out---';
			// a key of a possibly associated array of choices...
			if (isset($aChoices[$sReply])) {
						$sReply = $aChoices[$sReply]; break; }
//echo '--is not key---';
			// the index number of a choice -> defaults to 0 if no number
			$iReply = intval($sReply);
			if (isset($aChoicesValues[$iReply])) {
					$sReply = $aChoicesValues[$iReply]; break; }
//echo '--is not in choices---';
		} while (true);

		fwrite(STDOUT, 'Using: ' . $sReply . chr(10));

		return $sReply;

	} // getChoiceCLI

	static public function removeTrailingSlash($sPath, $sSlash = '/') {
		if ($sSlash == substr($sPath, -1)) $sPath = substr($sPath, 0, -1);
		return $sPath;
	} // removeTrailingSlash

	static public function addTrailingSlash($sPath, $sSlash = '/') {
		$sPath .= ($sSlash != substr($sPath, -1)) ? $sSlash : ''; // add trailing /
		return $sPath;
	} // addTrailingSlash

	function __toString() { return print_r(self, true); } // __toString

} // SssS_CLI_Tools

/* * * *\ SssS_CLI_Tools.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
