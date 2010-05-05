<?php
// MODIFIED FOR mb_ less & mb_ ed machines
/** SssS_Plist.inc
 *
 * @version 20100503_123217 + added __toString() for easy info. + Now handles NULL -> removes key if want to set to null. + added __toPlistArray(), __toDataBase64() and serializeAndEncodeObject() in conjunction with adding more support for adding non Plist values. Still triggers E_USER_NOTICE and informs what has been done. Read in parseArrayToTag() to see exact order of options: $o->__toPlistArray(), $o->__toDataBase64(), $o->__toString() and finally serializeAndEncodeObject($o)
 * @version 20091031_103025 + debugged the getter setter functions
 * @version 20091025_014644 + get...ForKeyPath + set...ForKeyPath functions added
 * @version 20091024_183354 + file_get/put_safely + save + saveTo + save as and a whole bunch of set/get values for key path functions
 * @version 20090803_213406
**/
if (!defined('AUTO_INCLUDING')) {
	require_once('SssS_xhtml.inc');
} // if not autoincluding but on php's include path

/**
 * adds plist functionality to SssS_XHTML
 */
// extends the base class
class SssS_Plist extends SssS_XHTML {
 	/**
 	 * @var array array representation of plist e.g. your preference settings in an array tree
	 * this is what you use to acces settings directly and quickly
	 * the getValueByKeyPath methods make some overhead
	 * @note the final state of this array will be saved to file
 	 */
	var $a_plist;			// array representation of plist
	/**
	 * @var string path to physical store representation of plist
	 */
	var $s_path = '';
	/**
	 * @var int base id &= i_html (of SssS_XHTML)
	 * @see SssS_XHTML::$i_html aliased to SssS_Plist::$i_plist
	 */
	var $i_plist;			// base id &= i_html
	/**
	 * @var string string representation of plist
	 */
	var $s_plist;			// string containing xml that will be parsed
	/**
	 * @var string possible values: ["plist" | "html" | "xhtml"] parsed from $s_plist if possible -> root tag name
	 */
	var $s_docTypeID;		// plist, html, xhtml parsed from s_plist if possible -> root tag name
	/**
	 * @var string possible values: ["SYSTEM" | "PUBLIC"] from/for doctype declaration
	 */
	var $s_pubSysID;		// from & for doctype declaration: SYSTEM or PUBLIC
	/**
	 * @var string PHP date formatting string
	 */
	var $s_dateFormat; 		// string dateFormat
	/**
	 * @var int timeZone
	 */
	var $i_timeZone; 		// integer timeZone
	/**
	 * @var bool false (default) if you want <key><nexttag> or true if you want <nexttag> on next line when outputing plist
	 */
	var $b_outPlistKeyOnOwnLine; // false (default) if you want <key><nexttag> or true if you want <nexttag> on next line when outputing plist.
	/**
	 * @var bool true (default) if you want dates and data values to be identifiable in the output array
	 */
	var $b_identifyDataAndDatesInOutputArray; // true (default)

	/**
	 * constructs the SssSPlist instance from the given string or array
	 *
	 * NOTE: if $m_plist is a string containing doctype and xml declarations these may override the parameters 1 to 3
	 * @param string|array $m_plist either a plist as string or as array. Defaults to an empty string
	 * @param string $s_charset optional, defaults to "utf-8"
	 * @param string $s_lang optional, defaults to "en"
	 * @param string $s_XMLversion optional, defaults to "1.0"
	 *
	 * @uses SssS_XHTML::SssS_XHTML() init super with given parameters and $i_DTDflag of 4 -> PLIST
	 * @uses SssS_XHTML::addTag() to add the root tag at id -1
	 * @uses SssS_Plist::initFromString() if $m_plist is string and not empty
	 * @uses SssS_Plist::initFromArray() if $m_plist is not a string but a non empty array
	 * @uses SssS_XHTML::$a_tags empties it
	 * @uses SssS_Plist::$i_timeZone sets to 0
	 * @uses SssS_Plist::$s_dateFormat sets to null
	 * @uses SssS_Plist::$b_outPlistKeyOnOwnLine set to false
	 * @uses SssS_Plist::$b_identifyDataAndDatesInOutputArray set to true
	 * @uses SssS_XHTML::$i_head set to null as not required
	 * @uses SssS_XHTML::$i_title set to null as not required
	 * @uses SssS_XHTML::$i_body set to null as not required
	 * @uses SssS_XHTML::$i_mChar set to null as not required
	 * @uses SssS_XHTML::$i_html aliased with SssS_Plist::$i_plist
	 * @uses SssS_Plist::$i_plist set to the root id for this plist ( returned id from addTag('plist', -1, array("version" => "1.0")) )
	 */
	// constructor
	public function SssS_Plist($m_plist = '', $s_charset = '',
			$b_outPlistKeyOnOwnLine = false,
			$b_identifyDataAndDatesInOutputArray = true,
			$bPathCheckCreate = false) {
		$s_charset = (!$s_charset || '' == $s_charset) ? 'utf-8' : $s_charset;
		$this->SssS_XHTML($s_charset, 'en', '1.0', 4);
		$this->a_tags = array();
		$this->i_timeZone = 0;
		$this->s_dateFormat = null;

		// optional switches
		// output option
		$this->b_outPlistKeyOnOwnLine = (true === $b_outPlistKeyOnOwnLine);
		// adds tags to data and dates as php arrays hold them as strings
		// when true we can save them to the correct format
		$this->b_identifyDataAndDatesInOutputArray =
				(false !== $b_identifyDataAndDatesInOutputArray);

		// void unneaded ids
		$this->i_head	= null;
		$this->i_title	= null;
		$this->i_body 	= null;
		$this->i_mChar 	= null;

		// stuff we don't need (still under dev in xhtml.inc)
		$this->a_newLineAfterOpenTags = array();
		$this->a_newLineAfterCloseTags = array();

		/* * add basic plist tag * */
		$this->i_plist &= $this->i_html;
		$this->i_plist = $this->addTag('plist', -1, array('version' => '1.0'));

		// read from given string or array
		// + or path 20091024_194309
		if (('array' == gettype($m_plist)) && (0 < count($m_plist))) {
			$this->initFromArray($m_plist);
		} elseif (('string' == gettype($m_plist)) && ('' != $m_plist)) {
			// check if is path with file_exists. If so return it's contents (if readable)
			// if not existing try and make a file using m_plist as path (if bPathCheckCreate = true)
			$mRes = $this->pathChecks($m_plist, $bPathCheckCreate);
			if (false !== $mRes) $m_plist = $mRes;
			$this->initFromString($m_plist);
		} // if array or string
	} // constructor

	/**
	 * reads the doctype declaration and populates instance variables: SssS_XHTML::$s_docType, SssS_Plist::$s_docTypeID, SssS_Plist::$s_pubSysID and SssS_XHTML::$s_lang
	 * only populates if a value is found
	 * @return void
	 * @uses SssS_Plist::$s_plist to extract doctype (and remove it)
	 * @uses SssS_XHTML::$s_docType set to the full doctype declaration (if found)
	 * @uses SssS_Plist::$s_docTypeID sets it to parsed value (if found)
	 * @uses SssS_Plist::$s_pubSysID sets it to found value (if found)
	 * @uses SssS_XHTML::$s_lang sets it to found language in doctype (if found)
	 * @todo this is ok when you have only one doc imported or if all are of the same language and encoding. For our Plist class this is ok as we allways have utf-8 encoding and en language
	 */
	// void fetchDocTypeFromS_PLIST()
	function fetchDocTypeFromS_PLIST() {

		static $bMB_strtolower, $s_search;

		if (!isset($bMB_strtolower))
				$bMB_strtolower = function_exists('mb_strtolower');

		if (!isset($s_search)) $s_search = '_'
			. '<!DOCTYPE '	// open tag
			. '(\w+)'		// type
			. ' (\w+) "'	// PUBLIC and SYSTEM identifier
			. '[^"]+?'
			. '//(\w+)'		// language
			. '"[^>]+?>'
			. '_si';
		preg_match_all($s_search, $this->s_plist, $a_matches, PREG_SET_ORDER);
		$s_docTypeFull = (isset($a_matches[0][0])) ? $a_matches[0][0] : '';
		$s_docTypeID = (isset($a_matches[0][1])) ? $a_matches[0][1] : '';
		$s_pubSysID = (isset($a_matches[0][2])) ? $a_matches[0][2] : '';
		$s_language = (isset($a_matches[0][3])) ? $a_matches[0][3] : '';

		// old: $this->s_plist = str_replace($s_docTypeFull, "", $this->s_plist);

		// TO DO: this is ok when you have only one doc imported or if all are of the same language and encoding. For our Plist class this is ok as we allways have utf-8 encoding and en language
		if ('' != $s_docTypeFull) $this->s_docType = $s_docTypeFull;
		if ('' != $s_docTypeID) $this->s_docTypeID = $s_docTypeID;
		if ('' != $s_pubSysID) $this->s_pubSysID = $s_pubSysID;
		if ('' != $s_language) {
			$this->s_lang = ($bMB_strtolower)
				? mb_strtolower($s_language, $this->s_charset)
				: strtolower($s_language);
		}
	} // fetchDocTypeFromS_PLIST

	/**
	 * reads the xml declaration and populates instance variables: SssS_XHTML::$s_XMLversion and SssS_XHTML::$s_charset
	 * only populates if a value is found
	 * @return void
	 * @uses SssS_Plist::$s_plist removes the XMLDeclaration if one was found
	 * @uses SssS_XHTML::$s_XMLversion set with information extracted from SssS_Plist::$s_plist (if found)
	 * @uses SssS_XHTML::$s_charset set with information extracted from SssS_Plist::$s_plist (if found)
	 * @todo this is ok when you have only one doc imported or if all are of the same language and encoding. For our Plist class this is ok as we allways have utf-8 encoding and en language
	 */
	// void fetchXMLDeclarationFromS_PLIST()
	function fetchXMLDeclarationFromS_PLIST() {
		$s_search = '_'
			. '<\?xml'						// open tag
			. '(\s+?version="([\d\.]+?)")?'	// version
			. '(\s+?encoding="([^"]*?)")'	// encoding
			. '\s*?\?>'						// closure
			. '_si';
		preg_match_all($s_search, $this->s_plist, $a_matches, PREG_SET_ORDER);
		$s_XMLdeclaration = (isset($a_matches[0][0])) ? $a_matches[0][0] : '';
		$s_XMLversion = (isset($a_matches[0][2])) ? $a_matches[0][2] : '';
		$s_XMLencoding = (isset($a_matches[0][4])) ? $a_matches[0][4] : '';

		// TO DO: this is ok when you have only one doc imported or if all are of the same language and encoding. For our Plist class this is ok as we allways have utf-8 encoding and en language
		$this->s_plist = str_replace($s_XMLdeclaration, '', $this->s_plist);
		if ('' != $s_XMLversion) $this->s_XMLversion = $s_XMLversion;
		if ('' != $s_XMLencoding) $this->s_charset = $s_XMLencoding;
	} // fetchXMLDeclarationFromS_PLIST

	/**
	 * taken from:
	 * PHP Plist Parser Class
	 *
	 * which is wonderfull but uses domdocument() which doesn't work (properly) on older PHP versions -> the reason I wrote this class
	 * Thank you very much Matsuda for your splendid code. I use it on systems with newer PHP
	 *
	 * @return string returns the formated string or if $s_format isn't supplied, the given value: $s_date
	 * @param string $s_date the string representation of a date (human readable)
	 * @param string $s_format the format string to be used with the PHP date() function
	 * @param int $i_offsetHour offset the hours
	 * @param int $i_offsetMinute offset the minutes
	 *
	 * @author Matsuda Shota <admin@sgssweb.com>
	 * @version Version 0.4.1
	 * @copyright Copyright (C) 2006 Matsuda Shota
	 * http://sgssweb.com/
	 */
	// string formatDate(string date, string format)
	// string formatDate(string date, string format, int offsetHour)
	// string formatDate(string date, string format, int offsetHour, int i_offsetMinute)
	public function formatDate($s_date, $s_format = null, $i_offsetHour = 0, $i_offsetMinute = 0) {
		if (!$s_format) {
			return $s_date;
		}

		// split into day notation and time notation by "T"
		$a_d = array();
		preg_match_all('/([\d.,:\-W]+)(?:T([\d.,:\-+WZ]*))?/', $s_date, $a_d);
		$s_dayNotation = $a_d[1][0];
		$s_timeNotation = $a_d[2][0];

		// extract year, month and day
		$a_days = array();
		preg_match_all('/^(\d{2})(?:\-?(\d{2}))?(?:\-?(\d{2}))?(?:\-?(\d{2}))?$/', $s_dayNotation, $a_days);
		if (count($a_days[0]) > 0) {
			$i_year = $a_days[1][0] * 100 + $a_days[2][0];
			$i_month = $a_days[3][0] + 0;
			$i_day = $a_days[4][0] + 0;
		}
		else {
			$i_year = $i_month = $i_day = 0;
		}

		// extract hour, minute andd second
		$a_times = array();
		preg_match_all('/^(\d{2})(?:[,.](\d+)(?=[+\-Z]|$))?(?:\:?(\d{2})(?:[,.](\d+)(?=[+\-Z]|$))?)?(?:\:?(\d{2})(?:[,.](\d+)(?=[+\-Z]|$))?)?(.*)/', $s_timeNotation, $a_times);
		if (count($a_times[0]) > 0) {

			$a_offsets = array();
			preg_match_all('/([+-])(\d{2})(?:\:?(\d{2}))?/', $a_times[7][0], $a_offsets);

			if (count($a_offsets[0]) > 0) {
				$i_offsetHour += $a_offsets[2][0] * ('-' == $a_offsets[1][0] ? -1 : 1);
				$i_offsetMinute += $a_offsets[3][0] * ('-' == $a_offsets[1][0] ? -1 : 1);
			}

			$i_hour = $a_times[1][0] + $i_offsetHour;
			$i_minute = ('0.' . $a_times[2][0]) * 60 + $a_times[3][0] + $i_offsetMinute;
			$i_second = ('0.' . $a_times[4][0]) * 60 + $a_times[5][0] + 0;
		}
		else {
			$i_hour = $i_minute = $i_second = 0;
		}

		return date($s_format, mktime($i_hour, $i_minute, $i_second, $i_month, $i_day, $i_year));
	} // formatDate

	function freeMemory() {
		$this->initFromArray($this->a_plist);
		return true;
	} // freeMemory

	/* * *
	 * * mixed function getPlistValue(
	 * *	  i_tag)	integer tag id whose value we seek
	 * *
	 * * companion of outputPlistArray()
	 * *
	 * * special thanks to Matsuda Shota, this is also a 'clone' of one of your functions
	 * * version 0.3 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * get a value from the plist representation
	 *
	 * companion of SssS_Plist::outputPlistArray()
	 *
	 * @param int $i_tag the id of the tag whose value we seek
	 * @return array|string|bool|int|float|null depending on the type of the value
	 * arrays and dictionarys are returned as arrays
	 * dates are returned as formated string using instance var s_dateFormat to format
	 * strings and data is returned as string
	 * boolean true or false
	 * numerical values undergo a preg_match for "/^[\+\-]*?[0-9]+$/" to determine wether to cast as integer or float
	 * if the tag is defined as "real", then we cast as float
	 * null is returned if no other match was found or $i_tag is an invalid id
	 *
	 * @uses SssS_XHTML::isValidTagID() to filter invalid ids
	 * @uses SssS_Plist::outputPlistArray() to return arrays and dictionaries
	 * @uses SssS_Plist::formatDate() to return dates as set with SssS_Plist::$s_dateFormat
	 * @uses SssS_XHTML::$a_tags to read data
	 * @uses SssS_Plist::$s_dateFormat to ensure that dates are returned as needed
	 * @uses SssS_Plist::$i_timeZone to ensure that dates are returned as needed
	 * @uses SssS_Plist::$b_identifyDataAndDatesInOutputArray to know if respective entries should be marked
	 *
	 * @version 0.3 (CC)2008 Luke JZ aka SwissalpS
	 * @author Matsuda Shota
	 * @see SssS_Plist::formatDate() for Matsuda's info
	 * @author Luke JZ aka SwissalpS cloned this function and made little changes
	 */
	// mixed getPlistValue(int i_tag)
	function getPlistValue($i_tag) {
		// filter invalid ids
		if (!$this->isValidTagID($i_tag)) return null;
		switch ($this->a_tags[$i_tag]['tag']) {
			// collections
			case 'array':
			case 'dict':
				return $this->outputPlistArray($i_tag);
			// primitive types
			case 'date':
				$s_tmp = $this->formatDate($this->a_tags[$this->a_tags[$i_tag]['children'][0]]['contents'], $this->s_dateFormat, $this->i_timeZone);
				if ($this->b_identifyDataAndDatesInOutputArray)
					return '[[SssS_Plist_date]]' . $s_tmp;
				else return $s_tmp;
			case 'string':
				if (!isset($this->a_tags[$i_tag]['children'][0])) return '';
				// don't exactly know how this happens, but it does occur that there is no child - real strange as the plist is valid (made with plist editor) and there were no MT strings
				//if (0 == count($this->a_tags[$i_tag]['children'])) return '';
				return (string) $this->a_tags[$this->a_tags[$i_tag]['children'][0]]['contents'];
			case 'data':
				$s_tmp = (string) $this->a_tags[$this->a_tags[$i_tag]['children'][0]]['contents'];
				if ($this->b_identifyDataAndDatesInOutputArray)
					return '[[SssS_Plist_data]]' . $s_tmp;
				else return $s_tmp;
			// numerical primitives
			case 'true':
				return true;
			case 'false':
				return false;
			case 'integer':
				if (preg_match("/^[\+\-]*?[0-9]+$/", $this->a_tags[$this->a_tags[$i_tag]['children'][0]]['contents'])) {
					return (integer) $this->a_tags[$this->a_tags[$i_tag]['children'][0]]['contents'];
				}
			case 'real':
				return (float) $this->a_tags[$this->a_tags[$i_tag]['children'][0]]['contents'];
			default: return null;
		}
	} // getPlistValue

	/* * *
	 * * void function initFromArray(
	 * *	  a_plist)	array representation of plist * dates and data are treated as strings, I'll be working on it once I should need that
	 * *
	 * * resets this object and adds tags or rather items from the array
	 * * to our objects a_tags array
	 * * finally s_plist is filled with the xml representation
	 * *
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * initialize from an array (of arrays)
	 *
	 * resets this object and adds tags or rather items from the array
	 * SssS_Plist::$a_plist is replaced with given array if it is not empty
	 * finally SssS_Plist::$s_plist is filled with the xml representation
	 *
	 * @param array $a_plist the array to init from
	 *
	 * @uses resetDeclarations() to return to defaults (in case user modified or whatever)
	 * @uses SssS_XHTML::addTag() to add root tag
	 * @uses SssS_Plist::parseArrayToTag()
	 * @uses SssS_Plist::outputPlist() to set SssS_Plist::$s_plist
	 * @uses SssS_XHTML::$a_tags empties it
	 * @uses SssS_Plist::$a_plist replaces it with $a_plist
	 * @uses SssS_Plist::$i_plist sets it to root id (0)
	 * @uses SssS_Plist::$s_plist replaces it with parsed array (as save-ready xml.plist)
	 * @uses SssS_XHTML::$s_XMLdeclaration to populate SssS_Plist::$s_plist
	 * @uses SssS_XHTML::$s_docType to populate SssS_Plist::$s_plist
	 *
	 * @todo do we really need doctype and xml declarations in SssS_Plist::$s_plist? in the returned value for sure, but otherwise we should probably keep it in a compareable state as when we initFromString
	 * @todo add some error reporting ||| dates and data are treated as strings, I'll be working on it once I should need that
	 * @version 0.2 (CC)2008 Luke JZ aka SwissalpS
	 */
	// void initFromArray(array a_plist)
	function initFromArray($a_plist) {
		if (('array' == gettype($a_plist)) && (0 < count($a_plist))) {
			$this->a_tags = array();
			$this->a_plist = $a_plist;
			$this->resetDeclarations();

			/* * add basic plist tag * */
			$this->i_plist = $this->addTag('plist', -1); //$$this->addTag($this->s_docTypeID, -1);
			$this->parseArrayToTag($this->a_plist, $this->i_plist);
			// now we have the tree, let's parse it to create an XML representation
			return $this->s_plist = $this->s_XMLdeclaration . chr(10)
				. $this->s_docType . chr(10)
				. $this->outputPlist($this->i_plist);
		} // if a_plist
	} // initFromArray

	/* * *
	 * * void function initFromString(
	 * *	  s_plist)	string containing plist
	 * *
	 * * resets this object to newly given xml declaration and or doctype
	 * * then removes the declarations from s_plist and parses it filling
	 * * a_tags with the information it finds
	 * * finally a_plist is filled with a nested array
	 * *
	 * *
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * initialize from a string (XML plist)
	 * resets this object to newly given xml declaration and or doctype
	 * then removes the declarations from SssS_Plist::$s_plist and parses it filling
	 * SssS_Plist::$a_plist with the information it finds as a nested array
	 *
	 * @param string $s_plist the string to init from
	 * @uses SssS_Plist::fetchXMLDeclarationFromS_PLIST()
	 * @uses SssS_Plist::fetchDocTypeFromS_PLIST()
	 * @uses SssS_Plist::parseXMLStringToTag()
	 * @uses SssS_Plist::outputPlistArray() to set SssS_Plist::$a_plist with
	 * @uses SssS_XHTML::$a_tags empties it
	 * @uses SssS_Plist::$s_plist sets it to the given string
	 * @uses SssS_Plist::$i_plist is reset to 0
	 * @uses SssS_Plist::$a_plist is filled with the parsed array returned from SssS_Plist::outputPlistArray()
	 * @todo add some error reporting
	 * @version 0.2 (CC)2008 Luke JZ aka SwissalpS
	 */
	// void initFromString(string s_plist)
	function initFromString($s_plist) {
		if ('' != trim($s_plist)) {
			$this->a_tags = array();
			$this->s_plist = $s_plist;
			$this->fetchXMLDeclarationFromS_PLIST();
			$this->fetchDocTypeFromS_PLIST();
			// $this->getAndRemoveCommentsFromS_PLIST();

			/* * add basic xml tag * */
			$this->i_plist = 0; //$$this->addTag($this->s_docTypeID, -1);
			$this->parseXMLStringToTag($this->s_plist, $this->i_plist);
			// now we have the tree, let's parse it to create an array representation
			//$this->a_plist = $this->outputPlistArray($this->i_plist +1); // this works here
			// right after initializing i_plist's first child is 1 after the structure has
			// been modified we must check for this value like so: $this->a_tags[$this->i_plist]['children'][0]
			//so the above line should be be:
			/* $this->a_plist = $this->outputPlistArray($this->a_tags[$this->i_plist]['children'][0]); */
			// but as we have only just initialized, we know the shortcut:
			$this->a_plist = $this->outputPlistArray(1);
			// technically we shoul outputPlistArray(0) to be able to have multiple items
			// in the plist 'root'
		} // if s_plist
	} // initFromString

	public function isMT() { return (0 == count($this->a_plist)); } // isMT

	public function load($sPath = null) {
		$sPath = ($sPath && '' != trim($sPath)) ? $sPath : $this->s_path;
		$mRes = $this->pathChecks($sPath);
		if (false === $mRes) return false;

		$this->s_path = $sPath;
		$this->a_plist = array(); // in case res == ''
		$this->initFromString($mRes);
		return $this->a_plist;
	} // load

	/* * *
	 * * string function outputPlist(
	 * *	  i_tag = -1		the tag id integer
	 * *	, i_level = 0		inset level to start with
	 * *	, s_spacer = "\t")	inset char(s) defaults to 1 tab
	 * *
	 * * returns content of tag including children as xml
	 * *
	 * * version 0.2p (CC)2008 Luke JZ aka SwissalpS (cloned from outputTag() of 0.2 version and adapted for plists with clean output)
	 * * */
	/**
	 * returns content of tag including children as xml string
	 *
	 * without doctype declaration
	 *
	 * @return string content of tag including children as xml string
	 * @param int $i_tag tags id
	 * @param int $i_level indentation level
	 * @param string $s_spacer indentation char(s) defaults to 1 tab per indentation
	 *
	 * @uses SssS_XHTML::isValidTagID() to filter invalid ids
	 * @uses SssS_Plist::outputPlist() to get children
	 * @uses SssS_XHTML::$a_tags to read data
	 * @uses SssS_XHTML::$s_textNodeName to identify text nodes
	 * @uses SssS_Plist::$b_outPlistKeyOnOwnLine to know how to format output
	 *
	 * @version 0.2p (CC)2008 Luke JZ aka SwissalpS(adopted from SssS_XHTML::outputTag() 0.2 dropping $i_flagIdNameClass functionality)
	 */
	function outputPlist($i_tag = -1, $i_level = 0, $s_spacer = "\t") {
		static $b_leadIn = true;
		$s_return = '';

		// filter invalid ids
		if (!$this->isValidTagID($i_tag)) return $s_return;
		$a_thisTag	= $this->a_tags[$i_tag];

		switch($a_thisTag['tag']) {
			case $this->s_textNodeName : // filter text nodes
				$s_return = $a_thisTag['contents']; //html_entity_decode(, ENT_QUOTES, $this->s_charset);
			break;
			case 'key' :
				$s_return = str_repeat($s_spacer, $a_thisTag['level'] + $i_level)
					. '<key>' . $this->outputPlist($a_thisTag['children'][0], $i_level, $s_spacer) . '</key>';
				$b_leadIn = $this->b_outPlistKeyOnOwnLine; // false; if you want <key><nexttag> or true if you want <nexttag> on next line
				if ($b_leadIn) $s_return .= chr(10);
			break;
			// basic types
			case 'string' :
			case 'integer' :
			case 'real' :
			case 'date' :
			case 'data' :
				$s_inner = (isset($a_thisTag['children'][0])) ? $this->outputPlist($a_thisTag['children'][0], $i_level, $s_spacer) : '';
				$s_leadin = (($b_leadIn) ? str_repeat($s_spacer, $a_thisTag['level'] + $i_level) : '');
				$s_return = sprintf('%3$s<%1$s>%2$s</%1$s>' . chr(10), $a_thisTag['tag'], $s_inner, $s_leadin);
				$b_leadIn = true;
			break;
			// boolean
			case 'true' : $s_return = '<true/>' . chr(10);
			break;
			case 'false' : $s_return = '<false/>' . chr(10);
			break;
			// null not official Apple standard
			//case 'null' : $s_return = '<null/>' . chr(10);
			break;
			// collections
			case 'array' :
			case 'dict' :
			case 'plist' :
				$s_return = sprintf('%2$s<%1$s>' . chr(10), $a_thisTag['tag'], (($b_leadIn) ? str_repeat($s_spacer, $a_thisTag['level'] + $i_level) : ''));
				$b_leadIn = true;
				foreach ($a_thisTag['children'] as $i_child) {
					$s_return .= $this->outputPlist($i_child, $i_level, $s_spacer); }
				$s_return .= sprintf('%2$s</%1$s>' . chr(10), $a_thisTag['tag'], str_repeat($s_spacer, $a_thisTag['level'] + $i_level));
			break;
		}
		return $s_return;
	}
	/* * *\ outputPlist (CC)2008 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * array function outputPlistArray(
	 * *	  i_tag = -1)		the tag id integer from where to start the array
	 * *
	 * * returns content of tag including children
	 * *
	 * * thanks to Matsuda Shota for your plist class. I leaned on it for this function
	 * * in particular.
	 * *
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * returns content of tag including children as array tree like plist
	 *
	 * thanks to Matsuda Shota for your plist class. I leaned on it for this function in particular.
	 * @see SssS_Plist::formatDate() for Matsuda's info
	 * @param int $i_tag tag id defaults to the root tag
	 * @return array content of tag including children as array tree representation of SssS_Plist object
	 *
	 * @uses SssS_XHTML::isValidTagID() to filter invalid ids
	 * @uses SssS_Plist::getPlistValue() to get children values
	 * @uses SssS_XHTML::$a_tags to read node data
	 * @uses SssS_XHTML::$s_textNodeName to identify text nodes
	 *
	 * @version 0.2 (CC)2008 Luke JZ aka SwissalpS thanks to Matsuda Shota
	 */
	function outputPlistArray($i_tag = 0) {
		$i_n = 0;
		$a_return = array();
		// filter invalid ids
		if (!$this->isValidTagID($i_tag)) return $a_return;

		$a_thisTag = $this->a_tags[$i_tag];
		foreach($a_thisTag['children'] as $i_child) {
			if ($this->s_textNodeName != $this->a_tags[$i_child]['tag']) {
				if ('key' == $this->a_tags[$i_child]['tag']) { // found key
					$s_key = $this->a_tags[$s_key = $this->a_tags[$i_child]['children'][0]]['contents'];
				} else { // other value
					$a_return[(isset($s_key) ? $s_key : $i_n)] = $this->getPlistValue($i_child);
				} //
				$i_n++;
			} // not text node
		} // foreach child
		return $a_return;
	}
	/* * *\ outputPlistArray (CC)2008 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * array function parseArrayToTag(array a_plist, int i_addTo)
	 * *
	 * * parses (plist) array into our XML object
	 * *
	 * * NOTE: dates will be treated as strings or numbers depending on how they are in the given array
	 * *
	 * * version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * parses (plist) array into our XML object
	 *
	 * if there are only int keys in $a_plist then it is an array in the plist sense. Otherwise it is a dictionary.
	 *
	 * @param array $a_plist the (plist) array to add to $i_addTo
	 * @param int $i_addTo tag id (receiver) of the array or dictionary
	 *
	 * @uses SssS_XHTML::addTag() to add tags, duh
	 * @uses SssS_Plist::parseArrayToTag() to process children
	 * @uses SssS_XHTML::$s_charset to mb_substr those identified date and data 'tags'
	 *
	 * @todo i_addTo passes unchecked for validity, that's ok?:!!!
	 * @todo analyze string to determine between string, date and data
	 * @todo dates will be treated as strings or numbers depending on how they are in the given array
	 * @version 20100503_143707 added null handling and some object acceptance:
	 * @version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 */
	// void parseArrayToTag(array a_plist, int i_addTo)
	function parseArrayToTag($a_plist, $i_addTo) {
		static $bMB_substr;

		if (!isset($bMB_substr))
				$bMB_substr = function_exists('mb_substr');

		// first we need to find out if the given array a_plist is a dict or an array
		// we asume array as default
		$s_arrayType = 'array';
		$i_count = 0;
		foreach (array_keys($a_plist) as $i_index => $m_key) {
			if (('integer' != gettype($m_key)) || ($i_index != $i_count++)) {
				$s_arrayType = 'dict'; break;
		}	}

		// now we know what we need to add tag to our Plist object
		$i_arrayID = $this->addTag($s_arrayType, $i_addTo);

		// let's add the children
		foreach ($a_plist as $m_key => $m_item) {
			// do we need a key preceeding our item?
			if ('dict' == $s_arrayType) $this->addTag('key', $i_arrayID, '', $m_key);
			switch(gettype($m_item)) {
				case 'array' :
					$this->parseArrayToTag($m_item, $i_arrayID);
				break;
				case 'double' :
				case 'float' :
					$this->addTag('real', $i_arrayID, '', $m_item);
				break;
				case 'integer' :
					$this->addTag('integer', $i_arrayID, '', $m_item);
				break;
				case 'boolean' :
					$this->addTag((($m_item) ? 'true' : 'false'), $i_arrayID);
				break;
				case 'string' : // TO DO: analyze string to determine between string, date and data
					// for instance look at the beginning of m_item for a code that defines dates and data, the rest is treated as string?
					$s_tmp = substr($m_item, 0, 19);
					if ($s_tmp == '[[SssS_Plist_data]]') {
						$m_item = ($bMB_substr) // shouln't be multibyte string...
							? mb_substr($m_item, 19, strlen($m_item), $this->s_charset)
							: substr($m_item, 19);
						// add == if not ending with one
						$m_item = trim($m_item);
						if ('=' != substr($m_item, -1)) $m_item .= '==';
						$s_tmp = 'data';
					} elseif ($s_tmp == '[[SssS_Plist_date]]') {
						$m_item = ($bMB_substr)
							? mb_substr($m_item, 19, strlen($m_item), $this->s_charset)
							: substr($m_item, 19);
						$s_tmp = 'date';
					} else $s_tmp = 'string';
					$this->addTag($s_tmp, $i_arrayID, '', $m_item);
				break;
				case  'NULL': // remove item by ignoring it?
				// not Apple compatible
//					$this->addTag('null', $i_arrayID);
				break;
				default:
				$sDid = 'ignored your request to set value.';
				if (is_object($m_item)) {
					if (method_exists($m_item, '__toPlistArray')) {
						$sDid = 'used value returned by __toPlistArray() and added as array.';
						$this->parseArrayToTag($m_item->__toPlistArray(), $i_arrayID);

					} else if (method_exists($m_item, '__toDataBase64')) {
						$sDid = 'used value returned by __toDataBase64() and added as data.';
						$s_tmp = 'data';
						$this->addTag('data', $i_arrayID, '', $m_item->__toDataBase64());

					} else if (method_exists($m_item, '__toString')) {
						$sDid = 'used value returned by __toString() and added as string.';
						$s_tmp = 'string';
						$this->addTag('string', $i_arrayID, '', $m_item->__toString());

					} else {
						$sDid = 'serialized and base64 encoded yourObject and added as data.';
						$s_tmp = 'data';
						$this->addTag('data', $i_arrayID, '',
										self::serializeAndEncodeObject($m_item));
					} // if responds to methods
				} // if object
				trigger_error('unknown type found in SssS_Plist::parseArrayToTag() type is: ' . gettype($m_item) . ' for key: ' . $m_key . chr(10) . chr(10) . 'Valid Plist values are strings, numbers and arrays. There is some support for dates and base64 encoded data. You cannot add objects but you can pass them but be aware you may not be able to retrieve them!' . chr(10) . 'Objects that respond to __toPlistArray() will be added as the returned array containing plist valid values.' . chr(10) . 'Then checks if object responds to __toDataBase64() which should be a base64 encoded string' . chr(10) . 'Then checks if object responds to __toString and adds it as that string' . chr(10) . 'Then the object is serialized and added as base64 encoded data string.' . chr(10) . 'When retrieving objects be aware of this, as you must recreate them yourself from the given string.' . chr(10) . 'Plists are not intended to hold objects rather just small portions of values.' . chr(10) . chr(10) . 'This time I ' . $sDid . chr(10) . chr(10), E_USER_NOTICE);
				debug_print_backtrace();
	}	}	} // parseArrayToTag

	/* * *
	 * * array function parseXMLStringToTag(string s_plist, int i_addTo)
	 * *
	 * * parses xml string into our XML object
	 * * s_plist is expected to be 100% strict, best without DOCTYPE and xml declaration tags
	 * * these should have been removed and brought to use when importing the string
	 * * with initFromString(s_plist)
	 * * This is NOT an HTML-parser!!!! -> regex
	 * * To parse html and other obscure xml, please use a parser provided by your system.
	 * * I wrote this parser with small plist files in mind, where there are no notes.
	 * * I was forced to write this parser for a system that had no functioning parser and
	 * * had to work as standalone with no network connection to another machine which
	 * * could translate. The project is SwissalpS.mgp.php where MgpDB.classphp is
	 * * where to look for an example where this class has been put to use.
	 * *
	 * * version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * parses xml string into our XML object
	 *
	 * $s_plist is expected to be 100% strict, best without DOCTYPE and xml declaration tags
	 * these should have been removed and brought to use when importing the string
	 * with SssS_Plist::initFromString($s_plist)
	 * This is NOT an HTML-parser!!!! -> regex
	 *
	 * To parse html and other obscure xml, please use a parser provided by your system.
	 * I wrote this parser with small plist files in mind, where there are no notes (comments).
	 * I was forced to write this parser for a system that had no functioning parser and
	 * had to work as standalone with no network connection to another machine which
	 * could translate. The project is SwissalpS.mgp.php where MgpDB.classphp is
	 * where to look for an example where this class has been put to use.
	 *
	 * NOTE: the ids of parents and children in the returned array refer to the respective of the returned array NOT SssS_XHTML::$a_tags or SssS_Plist::$a_plist
	 * @return array (plist) array of the newly parsed xml
	 *
	 * @param string $s_plist XML (plist) string to be inserted into the tree as child of $i_addTo
	 * @param int tag id (receiver)
	 *
	 * @uses SssS_XHTML::$s_textNodeName to create proper textnodes
	 * @uses SssS_XHTML::addTextNode() to add leading text or more likely contents of a node
	 * @uses SssS_XHTML::addTag() to add found tags
	 *
	 * @see SssS_XHTML::$a_tags
	 * @see SssS_Plist::initFromString()
	 * @version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 */
	// void parseXMLStringToTag(string s_plist)
	function parseXMLStringToTag($s_plist, $i_addTo) {
		$a_nodePath = array('byID' => array(0), 'byName' => array('root'));
		$a_allNodes = array(array(
				'tag' 		=> 'root',
				'typeflag'	=> -1,
				'level'		=> 0,
				'parent'	=> false,
				'children'	=> array(),
				'pathbyid'	=> 0,
				'pathbyname'=> 'root'));
		/* this regex is good and was usefull to get started with
		$s_search = "/(<([\w]+)[^>]*>)(.*)(<\/\\2>)/";
		this regex is a lot better, I copied from it too
		$s_search = "/<\/?\w+((\s+\w+(\s*=\s*(?:\".*?\"|\'.*?\'|[^\'\">\s]+))?)+\s*|\s*)\/?>/";
		and got caught up with this:
		$s_search = '_'
			. '(([^<])*?)'			// leading text
			. '(<'					// open tag
			. '(\w+)'				// tag name
			. '(('					// attributes or whitespace...
			. '(\s+\w+(\s*=\s*'		// attribute name
			. '(?:\".*?\"'			// value in double quotes
			. '|\'.*?\''			// or value in single quotes
			. '|[^\'\">\s]+'		// or value without quotes (not strict XML, the only unstrictness we'll have pass)
			. '))?)+\s*'			// ...multiple attributes and possibly trailing whitespace...
			. ')|\s*'				// ...or whitespace
			. ')'					// ...attributes or whitespace close
			. '((/>)|(>'			// close or pause tag
			. '(.*)'				// contents of tag
			. '</\4>)))'			// end tag
			. '((.*)?)'				// remaining text
			. '_si';
		which isn't bad but still far from the real thing. Many have tried to regex xml, some may have gone way farther than I, only to aknowledge
		that this task is too complex to actually consider transitional coding and other malformations in html and other arbitrary humanly malhandled xml.
		Using this class you must ensure that any files you read have critical cells htmlentities encoded. Specifically with ENT_QUOTES flag.
		I settled with the following pattern: */
		$s_search = '_'
			. '(<\?.*\?>)*?\s*'		// 8 xml declaration *we find it but don't use it, we have fetchXMLDeclarationFromS_PLIST()
			. '(<\!.*?>)*?\s*'		// 16 doctype declaration / comment.doc[^>] *same here, we have fetchDocTypeFromS_PLIST()
			. '(([^<])*?)'			// 0 leading text aka contents of previously opened tag
			. '(<'					// open tag...
			. '(\w+)'				// tag name
			. '(('					// attributes or whitespace...
			. '(\s+\w+(\s*=\s*'		// attribute name
			. '(?:\".*?\"'			// value in double quotes
			. '|\'.*?\''			// or value in single quotes
			. '|[^\'\">\s]+'		// or value without quotes (not strict XML, the only unstrictness we'll have pass)
			. '))?)+\s*'			// ...multiple attributes and possibly trailing whitespace...
			. ')|\s*'				// ...or whitespace
			. ')'					// ...attributes or whitespace close
			. '((/>)|(>))'			// 4 solo or 2 open
			. '|(</(\w+)\s*>))'		// or 1 close
			. '_si';
		/* finding node parts of types:
			16	<! >	doctype declaration / comment
			8	<? ?>	xml declaration
			4	< />	solo
			2	< >	open
			1	</ >	close
			0	(>) (<)	text
		*/

		$a_matches = array();
		preg_match_all($s_search, $s_plist, $a_matches, PREG_SET_ORDER);
/*
echo '
****************************
' . $s_plist
. '**********************
';
var_dump($a_matches);
die();
*/
		foreach ($a_matches as $a_match) {
			// xml declaration or php block which has nothing to do in a plist
			$m_8xmlDecl		= ('' != $a_match[1]) ? $a_match[1] : false;
			// or comment, as we are ignoring those at the mo, it's ok if they happen to appear in leading text but strictly there are no comments in plists as they are supposed to be structured and named in a comprehensible way
			$m_16docDecl	= ('' != $a_match[2]) ? $a_match[2] : false;
			// comment, but generally it's the contents of the previously opened tag
			$m_0leadingText	= ('' != trim($a_match[3])) ? $a_match[3] : false;
			// last character of s_0leadingText
			$s_leadingTextL = $a_match[4];
			// solo or open or close full -> our main search condition -> must be set or it's not in a_matches which would mean an error in preg_match_all()
			$s_wholeTag		= $a_match[5];
			// solo or open tag name else false
			$m_2a4tag		= (isset($a_match[6]) && ('' != $a_match[6])) ? $a_match[6] : false;
			// $a_match[7] holds all attributes or whitespace when solo tag -> uninteresting as we have all in 8
			// all attributes and values of solo or open else false. This is irelevant in plists except for the root tag itself.
			$m_wholeAtt		= (isset($a_match[8]) && ('' != trim($a_match[8]))) ? $a_match[8] : false;
			// $a_match[9] holds last attribute -> uninteresting as we have all in 8
			// $a_match[10] holds last attribute value part -> uninteresting as we have all in 8
			// $a_match[11] holds either /> or > (solo or open) we can detect what it is in 12 and 13
			$b_4sigSolo		= (isset($a_match[12]) && ('' != $a_match[12])) ? true : false;
			$b_2sigOpen		= (isset($a_match[13]) && ('' != $a_match[13])) ? true : false;
			$m_1wholeClose	= (isset($a_match[14])) ? $a_match[14] : false;
			$m_1tagClose	= (isset($a_match[15])) ? $a_match[15] : false;
/*
var_dump($a_match);
var_dump(array('m_8xmlDecl' => $m_8xmlDecl, 'm_16docDecl' => $m_16docDecl, 'm_0leadingText' => $m_0leadingText
	, 's_leadingTextL' => $s_leadingTextL, 's_wholeTag' => $s_wholeTag, 'm_2a4tag' => $m_2a4tag
	, 'm_wholeAtt' => $m_wholeAtt, 'b_4sigSolo' => $b_4sigSolo, 'b_2sigOpen' => $b_2sigOpen
	, 'm_1wholeClose' => $m_1wholeClose, 'm_1tagClose' => $m_1tagClose));
die();
*/
			// deal with leading text also can be looked at as contents of open tag
			if (false !== $m_0leadingText) { // m_0leadingText could be 'false', '0', 'null' which could possibly match a != or none
				$i_thisID		= count($a_allNodes);
				$i_thisParent = $a_nodePath['byID'][(count($a_nodePath['byID'])-1)];
				$a_thisNode = array(
					'tag' 		=> $this->s_textNodeName,
					'typeflag'	=> 0,
					'level'		=> count($a_nodePath['byID']) -1,
					'parent'	=> $i_thisParent,
					'children'	=> false,
					'attributes'=> false,
					'pathbyid'	=> (implode('/', $a_nodePath['byID']) . '/' . $i_thisID),
					'pathbyname'=> (implode('/', $a_nodePath['byName']) . '/' . $this->s_textNodeName),
					'contents'	=> $m_0leadingText,
					'lastchar'	=> $s_leadingTextL
				);
				// add as child
				$a_allNodes[$i_thisParent]['children'][] = $i_thisID;
				$this->addTextNode($i_thisParent -1, $m_0leadingText);
				// add node
				$a_allNodes[] = $a_thisNode;
			} // m_0leadingText

			// close tag -> move a level up by removing the current node from the paths
			if (false !== $m_1tagClose) { array_pop($a_nodePath['byID']); array_pop($a_nodePath['byName']); }

			// open or solo tag
			elseif ((false !== $b_4sigSolo) || (false !== $b_2sigOpen)) {
				$i_thisID		= count($a_allNodes);
				$i_thisParent	= $a_nodePath['byID'][(count($a_nodePath['byID']) -1)];
				$a_thisNode = array(
					'tag' 		=> $m_2a4tag,
					'typeflag'	=> 4,
					'level'		=> count($a_nodePath['byID']) -1,
					'parent'	=> $i_thisParent,
					'children'	=> false,
					'attributes'=> $m_wholeAtt,
					'pathbyid'	=> (implode('/', $a_nodePath['byID']) . '/' . $i_thisID),
					'pathbyname'=> (implode('/', $a_nodePath['byName']) . '/' . $m_2a4tag)
				);

				if (false !== $b_2sigOpen) { // open tag
					$a_thisNode['typeflag'] = 2;
					$a_thisNode['children'] = array();
					// add to path
					$a_nodePath['byID'][] = $i_thisID;
					$a_nodePath['byName'][] = $m_2a4tag;
				}

				// add as child
				$a_allNodes[$i_thisParent]['children'][] = $i_thisID;
				$this->addTag($a_thisNode['tag'], $i_thisParent -1, $a_thisNode['attributes']);
				// add node
				$a_allNodes[] = $a_thisNode;
			} // open or solo tag
		} // foreach match
		// NOTE: the ids of parents and children in this array refer to the respective of this array NOT $this->a_tags
		return $a_allNodes;
	} // parseXMLStringToTag

	private function pathChecks($s_path, $bCreate = false) {
/*echo '

pathChecks ' . $s_path . '

';
*/
		// does it exist?
		if (file_exists($s_path) && is_file($s_path)) {
			// yes, can we read?
			$mXML = @file_get_contents_safely($s_path);
			if (false !== $mXML) {
				// yes, read.
				// set global path
				$this->s_path = $s_path;
				return $mXML;
			} // if could read
				// no, can't read oops -> ...
		} else {
			if ($bCreate) {
				// no doesn't exist, can we create?
				$mRes = @file_put_contents_safely($s_path, '<plist>
	<string>forgot to save?</string>
</plist>', 'wb');
				if (false !== $mRes) {
					// yes, set global path
					$this->s_path = $s_path;
					unlink($s_path); // remove the dummy file again
					return '';
				} // if could create
					// no, can't create -> ...
			} // if try to create
		} // if file doesn't exist
		return false; // nothing goes
	} // pathChecks

	public function save() {
		return @file_put_contents_safely(
				$this->s_path, $this->initFromArray($this->a_plist), 'wb');
	} // save

	public function saveAs($sPath) {
		$mRes = @file_put_contents_safely(
				$sPath, $this->initFromArray($this->a_plist), 'wb');
		if (false != $mRes) $this->s_path = $sPath;
		return $mRes;
	} // saveAs

	public function saveTo($sPath) {
		return @file_put_contents_safely(
				$sPath, $this->initFromArray($this->a_plist), 'wb');
	} // saveTo

	/* * *
	 * * string function XML (
	 * *		integer i_tag	tag index to output
	 * *		integer i_level	amount of base inset
	 * *		string s_spacer	the whitespace to use as inset)
	 * *
	 * * returns tidy xml string ready for output to file or browser
	 * *
	 * * version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * returns tidy xml string ready for output to file or browser
	 *
	 * with xml and doctype declarations in place
	 * @return string xml string ready for output
	 * @param int $i_tag optional tag id to output from. Defaults to root ie plist tag. (you may set up sandbox/alternative roots or when using setters ForKeyPath root may move)
	 * @param int $i_level optional starting indentation level. Defaults to 0
	 * @param string $s_spacer optional indentation string. Defaults to 4 spaces
	 *
	 * @uses SssS_Plist::outputPlist()
	 * @uses SssS_XHTML::$s_XMLdeclaration to output
	 * @uses SssS_XHTML::$s_docType for output
	 *
	 * @version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 */
	public function XML($i_tag = -1, $i_level = 0, $s_spacer = '    ') {
		$i_tag = (-1 == $i_tag) ? $this->i_plist : $i_tag;
		$s_return = $this->s_XMLdeclaration . chr(10)
				. $this->s_docType . chr(10)
				. $this->outputPlist($i_tag, $i_level, $s_spacer);
		return $s_return;
	} // XML
	/* * *\ XML (CC)2008 Luke JZ aka SwissalpS /* * */
/* /ValueForKeyPath FUNCTIONS BEGIN\ */

	private function fArrayizeKeyPath($mKeyPath = '') {
		$sType = gettype($mKeyPath);
		switch($sType) {
			case 'array': return $mKeyPath;
			break;
			case 'string': return explode('/', str_replace(' ', '', $mKeyPath));
			break;
			default:
				trigger_error('invalid argument type passed in SssS_::fArrayizeKeyPath('
						. $mKeyPath	. ') type is: ' . $sType
						. ' should be array or string', E_USER_NOTICE);
				return false;
		} // switch
	} // fArrayizeKeyPath

	public function getBool($mKeyPath = '', $mDefaultValue = false, $iStartID = -1) {
		$mRes = $this->get($mKeyPath, $mDefaultValue, $iStartID);
		return (null !== $mRes && is_bool($mRes)) ? $mRes : $mDefaultValue;
		return false; // nothing found
	} // getBoolForKeyPath

	public function getBoolForKeyPath($mKeyPath = '', $iStartID = -1) {
		$aRes = $this->getValueForKeyPath($mKeyPath, $iStartID);
		if ($aRes['found'] && is_bool($aRes['value'])) return $aRes['value'];
		return false; // nothing found
	} // getBoolForKeyPath

	// I haven't worked with data objects -> so pls advise if you think it's
	// not as usefull as it could be
	public function getDataForKeyPath($mKeyPath = '', $iStartID = -1) {
		$aRes = $this->getValueForKeyPath($mKeyPath, $iStartID);
		if ($aRes['found']) return str_replace(
				array('[[SssS_Plist_data]]', '=='), '', $aRes['value']);
		return ""; // nothing found
	} // getDataForKeyPath

	public function getDateForKeyPath($mKeyPath = '', $sFormat = null,
			$iStartID = -1) {

		$aRes = $this->getValueForKeyPath($mKeyPath, $iStartID);
		if ($aRes['found']) return $this->formatDate(
				str_replace('[[SssS_Plist_date]]', '', $aRes['value']),
						$sFormat);
		return ""; // nothing found
	} // getDateForKeyPath

	public function getDouble($mKeyPath = '', $mDefaultValue = 0.0, $iStartID = -1) {
		return $this->getFloat($mKeyPath, $mDefaultValue, $iStartID);
	} // getDouble

	public function getFloat($mKeyPath = '', $mDefaultValue = 0.0, $iStartID = -1) {
		$mRes = $this->get($mKeyPath, $mDefaultValue, $iStartID);
		return (null !== $mRes) ? floatval($mRes) : $mDefaultValue;
	} // getFloat

	public function getFloatForKeyPath($mKeyPath = '', $iStartID = -1) {
		$aRes = $this->getValueForKeyPath($mKeyPath, $iStartID);
		if ($aRes['found']) return floatval($aRes['value']);
		return (float)0.0; // nothing found
	} // getFloatForKeyPath

	public function getInt($mKeyPath = '', $mDefaultValue = 0, $iStartID = -1) {
		$mRes = $this->get($mKeyPath, $mDefaultValue, $iStartID);
		return (null !== $mRes) ? intval($mRes) : $mDefaultValue;
	} // getInt

	public function getInteger($mKeyPath = '', $mDefaultValue = 0, $iStartID = -1) {
		return $this->getInt($mKeyPath, $mDefaultValue, $iStartID);
	} // getInteger

	public function getIntegerForKeyPath($mKeyPath = '', $iStartID = -1) {
		$aRes = $this->getValueForKeyPath($mKeyPath, $iStartID);
		if ($aRes['found']) return intval($aRes['value']);
		return (int)0; // nothing found
	} // getIntegerForKeyPath

	public function getIntForKeyPath($mKeyPath = '', $iStartID = -1) {
		return $this->getIntegerForKeyPath($mKeyPath, $iStartID);
	} // getIntForKeyPath

	// shortcut for getObjectForKeyPath() with optional default value param
	public function get($mKeyPath = '', $mDefaultValue = null, $iStartID = -1) {
		$mRes = $this->getObjectForKeyPath($mKeyPath, $iStartID);
		return (null !== $mRes) ? $mRes : $mDefaultValue;
	} // get

	public function getObjectForKeyPath($mKeyPath = '', $iStartID = -1) {
		$aRes = $this->getValueForKeyPath($mKeyPath, $iStartID);
		if ($aRes['found']) return $aRes['value'];
		return null; // nothing found
	} // getObjectForKeyPath

	public function getOrSet($mKeyPath = '', $mDefaultValue = null,
			$iStartID = -1) {

		$mRes = $this->getObjectForKeyPath($mKeyPath, $iStartID);
		if (null !== $mRes) return $mRes;
		if (null !== $mDefaultValue) {
			$mRes = $this->setObjectForKeyPath(
					$mDefaultValue, $mKeyPath, $iStartID);

			return (!$mRes) ? null : $mDefaultValue;
		} // if default value given
		return null;
	} // getOrSet

	public function getStr($mKeyPath = '', $mDefaultValue = '', $iStartID = -1) {
		$mRes = $this->get($mKeyPath, $mDefaultValue, $iStartID);
		return (null !== $mRes) ? $mRes : $mDefaultValue;
	} // getStr

	public function getStrForKeyPath($mKeyPath = '', $iStartID = -1) {
		return $this->getStringForKeyPath($mKeyPath, $iStartID);
	} // getStrForKeyPath

	public function getString($mKeyPath = '', $mDefaultValue = '', $iStartID = -1) {
		return getStr($mKeyPath, $mDefaultValue, $iStartID);
	} // getStringForKeyPath

	public function getStringForKeyPath($mKeyPath = '', $iStartID = -1) {
		$aRes = $this->getValueForKeyPath($mKeyPath, $iStartID);
		if ($aRes['found']) return strval($aRes['value']);
		return (string)''; // nothing found
	} // getStringForKeyPath

	function getValueForKeyPath($mKeyPath = '', $iStartID = -1) {
		// default return array
		$aRet = array('found' => false, 'value' => null);
		// split the path into an array
		$aPathParts = $this->fArrayizeKeyPath($mKeyPath);
		if (false == $aPathParts) return $aRet;
		// now we have an array with paths

		if (-1 == $iStartID) $iStartID = $this->i_plist;
		// start at first item inside iStartID
// TODO: when, why and what to do when invalid index?
		if (isset($this->a_tags[$iStartID]['children'][0])) {
			$iStrain = $this->a_tags[$iStartID]['children'][0];
		} elseif ($this->isMT()) {
			// no sweat
			$iStrain = null;
		} else {
			$iStrain = null;
			echo "\n" . 'strange occurance in SssS_Plist::getValueForKeyPath'
					. "\n";
			var_dump($this->a_tags[$iStartID]['children'], ($iStrain = $this->a_tags[$iStartID]['children'][0]));
		} // if got kids, MT or strange

		// set the base array to search in
		$mValue = $this->outputPlistArray($iStrain);

		// traverse the arrays and return the error array
		while(0 < count($aPathParts)) {
			$sKey = array_shift($aPathParts);
			if ('' != $sKey) {
				if (isset($mValue[$sKey])) {
					// good, key found
					// move up if it's an array or last one, ok
					if (('array' == gettype($mValue[$sKey]))
							or (0 == count($aPathParts))) {
						$mValue = $mValue[$sKey];
					} else {
						// error
						trigger_error('invalid type in key path in SssS_::getValueForKeyPath('
							. $mKeyPath	. ') at key: ' . $sKey . ' should be: '
							. gettype($mValue[$sKey]) . ' line: ' . __LINE__, E_USER_NOTICE);
						return $aRet;
					} // if array, last one or error
				} else {
					// bad, no key
#					trigger_error('key path not found in'
#							. ' SssS_::getValueForKeyPath(' . $mKeyPath
#							. ') remaining path: '
#							. '"' . implode('/', $aPathParts) . '"'
#							. ' line: ' . __LINE__
#							. "\n" . debug_print_backtrace(), E_USER_NOTICE);
					return $aRet;
				} // if found key or not
			} // if skip empty first or other empty due to // in path
		} // while got parts

		// if we got here, we must have the value in mValue
		return array('found' => true, 'value' => $mValue);
	} // getValueForKeyPath

	// conveinience function NOTE arguments are shuffeld in order to set...ForKeyPath syntax
	public function setBool($mKeyPath = '', $bBool = false,	$iStartID = -1) {
		return $this->setBoolForKeyPath($bBool, $mKeyPath,	$iStartID);
	} // setBool

	public function setBoolForKeyPath($bBool = false, $mKeyPath = '',
			$iStartID = -1) {

		return $this->setObjectForKeyPath((bool)(true === $bBool), $mKeyPath,
				$iStartID);
	} // setBoolForKeyPath

	// I haven't worked with data objects -> so pls advise if you think it's
	// not as usefull as it could be
	public function setDataForKeyPath($sData = '', $mKeyPath = '',
			$iStartID = -1) {

		$sData .= ('=' != substr($sData, -1)) ? '=' : '';
		$sData .= ('==' != substr($sData, -2)) ? '=' : '';
		if ($this->b_identifyDataAndDatesInOutputArray) $sData =
				'[[SssS_Plist_data]]' . $sData;
		return $this->setObjectForKeyPath((string)$sData, $mKeyPath, $iStartID);
	} // setDataForKeyPath

	public function setDate($mKeyPath = '',	$mDate = 0, $sFormat = null,
			$iStartID = -1) {
		return $this->setDataForKeyPath($mDate, $mKeyPath, $sFormat, $iStartID);
	} // setDate

	public function setDateForKeyPath($mDate = 0, $mKeyPath = '',
			$sFormat = null, $iStartID = -1) {

		// convert first
		$sDate = ('integer' == gettype($mDate))
				? gmdate('Y-m-d\TH:i:s\Z', $mDate)
				: $mDate;
		// and format if requested
		$sDate = $this->formatDate($sDate, $sFormat);
		if ($this->b_identifyDataAndDatesInOutputArray) $sDate =
				'[[SssS_Plist_date]]' . $sDate;
		return $this->setObjectForKeyPath((string)$sDate, $mKeyPath, $iStartID);
	} // setDateForKeyPath

	public function setDouble($mKeyPath = '', $fFloat = 0.0, $iStartID = -1) {
		return $this->setFloat($mKeyPath, $fFloat, $iStartID);
	} // setDouble

	public function setFloat($mKeyPath = '', $fFloat = 0.0, $iStartID = -1) {
		return $this->setFloatForKeyPath($fFloat, $mKeyPath, $iStartID);
	} // setFloat

	public function setFloatForKeyPath($fFloat = 0.0, $mKeyPath = '',
			$iStartID = -1) {

		return $this->setObjectForKeyPath((float)floatval($fFloat), $mKeyPath, $iStartID);
	} // setFloatForKeyPath

	public function setInt($mKeyPath = '', $iInteger = 0, $iStartID = -1) {
		return $this->setIntegerForKeyPath($iInteger, $mKeyPath, $iStartID);
	} // setInt

	public function setInteger($mKeyPath = '', $iInteger = 0, $iStartID = -1) {
		return $this->setIntegerForKeyPath($iInteger, $mKeyPath, $iStartID);
	} // setInteger

	public function setIntegerForKeyPath($iInteger = 0, $mKeyPath = '',
			$iStartID = -1) {

		return $this->setObjectForKeyPath((int)intval($iInteger), $mKeyPath, $iStartID);
	} // setIntegerForKeyPath

	public function setIntForKeyPath($iInteger = 0, $mKeyPath = '',
			$iStartID = -1) {

		return $this->setIntegerForKeyPath($iInteger, $mKeyPath, $iStartID);
	} // setIntForKeyPath

	// shortcut for setObjectForKeyPath() with different argument sorting
	public function set($mKeyPath = '', $mObject, $iStartID = -1) {
		return $this->setObjectForKeyPath($mObject, $mKeyPath, $iStartID);
	} // set

	// shortcut for setObjectForKeyPath() with different argument sorting
	public function setObject($mKeyPath = '', $mObject, $iStartID = -1) {
		return $this->setObjectForKeyPath($mObject, $mKeyPath, $iStartID);
	} // setObject

	public function setObjectForKeyPath($mObject, $mKeyPath = '',
			$iStartID = -1) {

		$aRes = $this->setValueForKeyPath($mObject, $mKeyPath, $iStartID);
		if ($aRes['set']) return true;
		return null; // unable to set
	} // setObjectForKeyPath

	public function setStr($mKeyPath = '', $sString = '', $iStartID = -1) {
		return $this->setStringForKeyPath($sString, $mKeyPath, $iStartID);
	} // setStr

	public function setStrForKeyPath($sString = '', $mKeyPath = '',
			$iStartID = -1) {

		return $this->setStringForKeyPath($sString, $mKeyPath, $iStartID);
	} // setStrForKeyPath

	public function setString($mKeyPath = '', $sString = '', $iStartID = -1) {
		return $this->setStringForKeyPath($sString, $mKeyPath, $iStartID);
	} // setString

	public function setStringForKeyPath($sString = '', $mKeyPath = '',
			$iStartID = -1) {

		return $this->setObjectForKeyPath((string)strval($sString), $mKeyPath, $iStartID);
	} // setStringForKeyPath

	function setValueForKeyPath($mObject, $mKeyPath = '', $iStartID = -1) {
		// default return array
		$aRet = array('set' => false, 'value' => null);
		// split the path into an array
		$aPathParts = $this->fArrayizeKeyPath($mKeyPath);
		if (false == $aPathParts) return $aRet;
		// now we have an array with paths

		if (-1 == $iStartID) $iStartID = $this->i_plist;
		// start at first item inside iStartID
// TODO: when, why and what to do when invalid index?
		if (isset($this->a_tags[$iStartID]['children'][0])) {
			$iStrain = $this->a_tags[$iStartID]['children'][0];
		} elseif ($this->isMT()) {
			// no sweat
			$iStrain = null;
		} else {
			$iStrain = null;
			echo "\n" . 'strange occurance in SssS_Plist::setValueForKeyPath'
					. "\n";
			var_dump($this->a_tags[$iStartID]['children'], ($iStrain = $this->a_tags[$iStartID]['children'][0]));
		} // if got kids, MT or strange

		// set the base array to search in
		$mValue = $this->outputPlistArray($iStrain);

		$aUperLayeredArrays = array();
		// traverse the arrays and return the error array
		while(0 < count($aPathParts)) {
			$sKey = array_shift($aPathParts);
			if ('' != $sKey) {
				if (isset($mValue[$sKey])) {
					// good, key found
					// move up if it's an array or last one, ok
					if (('array' == gettype($mValue[$sKey]))
							or (0 == count($aPathParts))) {
						$aUperLayeredArrays[] = array('value' => $mValue, 'key2Lower' => $sKey);
						$mValue = $mValue[$sKey];
						if (0 == count($aPathParts))
								$aRet = array('set' => true, 'value' => $mValue);
					} else {
						// error
						trigger_error('invalid type in key path in SssS_::setValueForKeyPath('
							. $mKeyPath	. ') at key: ' . $sKey . ' should be: '
							. gettype($mValue[$sKey]) . ' line: ' . __LINE__, E_USER_NOTICE);
						return $aRet;
					} // if array, last one or error
				} else {
					// bad, no key
					// make new? if we're at end of path yes!
					if (0 == count($aPathParts)) {
						$aUperLayeredArrays[] = array('value' => $mValue, 'key2Lower' => $sKey);
						$mValue[$sKey] = $mObject;
						$aRet = array('set' => true, 'value' => $mObject);
					} else {
						// make missing ala mkdir -p
						$aUperLayeredArrays[] = array('value' => $mValue, 'key2Lower' => $sKey);
						$mValue[$sKey] = array();
						$mValue = array();
#						trigger_error('key path not found in'
#								. ' SssS_::setValueForKeyPath(' . $mKeyPath
#								. '), made it up, remaining path: '
#								. '"' . implode('/', $aPathParts) . '"'
#								. ' line: ' . __LINE__
#								. "\n" . debug_print_backtrace(), E_USER_NOTICE);
						//return $aRet;
					} // if at paths end -> new key or invalid path
				} // if found key or not
			} // if skip empty first or other empty due to // in path
		} // while got parts

		// if we got here, we must have the current value in mValue
		// traverse up the tree again
		$aNew = array();
		$mValue = $mObject;
		while(0 < count($aUperLayeredArrays)) {
			// cut off this layer
			$aThisLayer = array_pop($aUperLayeredArrays);
			// extract the value of the level
			$mLayerValue = $aThisLayer['value'];
			// place the value in upper levels value
			$mLayerValue[$aThisLayer['key2Lower']] = $mValue;
			// step up
			$mValue = $mLayerValue;
		} // while got higher level

		// first remove the children from iStartID
		$this->a_tags[$iStartID]['children'] = array();

/*
	TODO: test and check how this works when not starting path at -1

$iParent = $this->a_tags[$iStartID]['parent'];
if(0 > $iParent) {
echo '

parent not cool

';
$iParent = 0;
} else {
		$aNewChildren = array();
		foreach($this->a_tags[$iParent]['children'] as $iChild) {
			if ($iChild != $iStartID) $aNewChildren[] = $iChild;
		} // foreach child cleaning out the starter
		$this->a_tags[$iParent]['children'] = $aNewChildren;
}
echo '


plist dump

';
debug_print_backtrace();
var_dump("iPlist:", $this->i_plist,"iParent:", $iParent,"i:", $i);//, $this->a_tags[$this->i_plist]['children']

		// in case we modified root plist (iStartID = 1)
		if (1 == $iStartID) {
			echo '
			********************herere
			';
			var_dump($this->a_tags[$this->i_plist]['children']);
			echo '
			herere********************
			';
			// reasign it's pointer
			$this->i_plist = $this->a_tags[$this->i_plist]['children'][0];
		} else {
			echo '
			there *****************
			';
		} // if 1 == startID
*/

		// add the modified array
		$this->parseArrayToTag($mValue, $iStartID);

		// reinit a_plist
		$this->a_plist = $this->outputPlistArray(
				$this->a_tags[$this->i_plist]['children'][0]);

		// saving will flush the doubles and reinit the ids of tags
		return $aRet;
	} // setValueForKeyPath

/* \ValueForKeyPath FUNCTIONS END/ */

	// 20100503_123217 added __toString() for easy info
	function __toString() { return $this->XML(); } // __toString
	// for plist mergance
	function __toPlistArray() {
		return ($this->a_plist) ? $this->a_plist : array();
	} // __toPlistArray
	// to add serialisation, one of the reasons I started writing SssS_Plist
	// was to avoid needing this and I still don't recomend it.
	function __toDataBase64() {
		return self::serializeAndEncodeObject($this);
	} // __toDataBase64
	static function serializeAndEncodeObject($o) {
		return base64_encode(serialize($o)) . '==';
	} // serializeAndEncodeObject

} // SssS_Plist
/* * *\ SssS_Plist.inc (CC)2008 Luke JZ aka SwissalpS /* * */
?>
