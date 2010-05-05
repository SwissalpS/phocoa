<?php
/* @version 20091024_201727 + file_get/put_contents_safely */
if (!defined('AUTO_INCLUDING')) {
	require_once('SssS_xhtml.inc');
} // if not autoincluding but on php's include path

// extends the base class
class SssS_Xtend extends SssS_XHTML {

	var $a_clean		= array();	// data to be transmited
	var $a_post			= array();	// the POST data
	var $a_cookies		= array();	// the cookies
	var $s_method		= "";		// method of passing data (a_clean)
	var $s_cn			= 'PHP_SwissalpSseSSion'; // cookie name
//	var $b_ssCookies	= false;	// undated cookies permited
//	var $b_stCookies	= false;	// dated cookies permited
	var $b_sCookie		= false;	// session cookie available
	var $b_js			= false;	// remote uses javascript
	var $s_ip	 		= false;	// the remote ip
	var $s_bs			= "";		// browser short for e.g. link styles
	var $i_bv			= "";		// browser version as float
	var $i_generator	= null;		// tag id of meta generator tag
	var $s_myIP			= "1.0.0.1";// set with getMyIP()
	var $s_myURL		= "";		// set with getMyURL()
	var $s_myPathFile	= "";		// set with setMyPathFile()
	var $a_supportedLanguages = array('en'); // must have at least one
	var $s_langPath		= "./SssS_lang/";
	var $a_GeoLocResponse = array();	// for geo location

	function SssS_Xtend($s_charset = "", $s_lang = "en"
			, $s_XMLversion = '1.0', $i_DTDflag = 0) {
		$s_charset = ('' == $s_charset) ? 'utf-8' : $s_charset;
		$this->SssS_XHTML($s_charset, $s_lang, $s_XMLversion, $i_DTDflag);
		$this->addBasicHead();
	} // initialize SssS_Xtend

	function setTitle($s_title) {
		// yeah, that's why I made a function of this task one liner or not, this would most often render syntax errors if you couldn't just call this handy function
		$this->a_tags[$this->a_tags[$this->i_title]['children'][0]]['contents'] = $s_title;
	}

	function getTitle() {
		// yeah, that's why I made a function of this task
		return $this->a_tags[$this->a_tags[$this->i_title]['children'][0]]['contents'];
	}

	function getBS() {
		$s_ua = (isset($_SERVER["HTTP_USER_AGENT"]))
				? $_SERVER["HTTP_USER_AGENT"] : 'na';
		if (strpos($s_ua, "Safari")) {
			$this->s_bs = "saf";
			$this->i_bv = floatval(
					substr($s_ua, strpos($s_ua, "") + 7));
		} elseif (strpos($s_ua, "iPhone")) {
			$this->s_bs = "ips"; // iPhone Safari
			$this->i_bv = floatval(
					substr($s_ua, strpos($s_ua, "Safari") + 7));
		} elseif (strpos($s_ua, "CFNetwork")) {
			$this->s_bs = "ipi"; // iPhone AppTapp/Installer
			$this->i_bv = floatval(
					substr($s_ua, strpos($s_ua, "CFNetwork") + 10));
		} else {
			$this->s_bs = "gen";
			$this->i_bv = 5;
		}
		return $this->s_bs;
	}

	function getCookies() {
		global $HTTP_COOKIE_VARS;
		if (isset($_COOKIE)) {
			$this->a_cookies 	= $_COOKIE;
		} elseif (isset($HTTP_COOKIE_VARS)) {
			$this->a_cookies 	= $HTTP_COOKIE_VARS;
		} elseif (isset($GLOBALS['HTTP_COOKIE_VARS'])) {
			$this->a_cookies 	= $GLOBALS['HTTP_COOKIE_VARS'];
		} else {
			$this->a_cookies 	= array();
		}
		return $this->a_cookies;
	}

	function getMyIP() {
		if (isset($_SERVER["SERVER_ADDR"])) {
			$this->s_myURL = $_SERVER["SERVER_ADDR"];
		}
		return $this->s_myIP;
	}

	function getMyPathFile() {
		$this->s_myPathFile = (isset($_SERVER["SCRIPT_FILENAME"]))
				? $_SERVER["SCRIPT_FILENAME"]
				: die('Fatal error SssS_Xtend::getMyPathFile()');
		return $this->s_myPathFile;
	}

	function getMyURL() {
		if (isset($_SERVER["SCRIPT_URI"])) {
			$this->s_myURL = $_SERVER["SCRIPT_URI"];
		} elseif ((isset($_SERVER['HTTP_HOST'])
				|| isset($_SERVER['SERVER_NAME']))
				&& isset($_SERVER['REQUEST_URI'])) {
			$this->s_myURL = (isset($_SERVER["HTTP_HOST"]))
					? ('http' . ($this->isSSL() ? 's' : '') . '://') . $_SERVER["HTTP_HOST"]
						. $_SERVER['REQUEST_URI']
					: die('Fatal error '
						. 'SssS_Xtend::getMyURL()::HTTP_HOST');
		} else {
			die('Fatal error SssS_Xtend::getMyURL()::whoami');
			// $_SERVER["DOCUMENT_ROOT"];
		}
		return $this->s_myURL;
	}

	function getPost() {
		if (isset($_POST)) {
			$aP 	= $_POST;
		} elseif (isset($HTTP_POST_VARS)) { // depricated as of 4.1.0
			$aP 	= $HTTP_POST_VARS;
		} elseif (isset($GLOBALS['HTTP_POST_VARS'])) {
			$aP 	= $GLOBALS['HTTP_POST_VARS'];
		} else {
			$aP 	= array();
		}
		return $this->a_post = $aP;
	} // fGetPost

	function getRemIP() {
		if (isset($_SERVER["REMOTE_ADDR"])) {
			$this->s_ip = $_SERVER["REMOTE_ADDR"];
		} elseif (isset($_SERVER['HTTP_CLIENT_IP'])) {
			$this->s_ip = $_SERVER['HTTP_CLIENT_IP'];
		} else $this->s_ip = '1.0.0.1';

		return $this->s_ip;
	}

	function getRemLang() {
		$s_l = strtolower((isset($_SERVER["HTTP_ACCEPT_LANGUAGE"]))
				? substr($_SERVER["HTTP_ACCEPT_LANGUAGE"], 0, 2)
				: $this->s_lang);
		$this->s_lang =	(in_array($s_l, $this->a_supportedLanguages))
				? $s_l : $this->a_supportedLanguages[1];
		return $this->s_lang;
	}

/* TO DO:
        function isRemoteOnLAN() { // isClientOnLAN?
            $bRet = false;
            $sRemIP = $this->getRemIP();
            $sMyIP = '192.168.222.125';

            return $bRet;
        } // isRemoteOnLAN

isRemoteLocalhost?
*/
	/**
	 * addBasicCSS($sPath = '')
	 * add basic css for iPhones
	 * expects to find a image at ../img/link.png representing a gt '>'
	 *
	 * @param string $sPath (optional) read style from file
	 *
	 * @return void
	 *
	 * @uses SssS_XHTML::addStyle - to add the styles
	 *
	 * @version 20090728_000829 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addBasicCSS($sPath = '') {
		$sStyle = '';
		if ('' != $sPath) {
			if (is_file($sPath) && is_readable($sPath)) $sStyle = trim(
					file_get_contents_safely($sPath));
		} // if path argument given, read from file

		if ('' == $sStyle) $sStyle = '/*
Author: SwissalpS Xtended XHTML generator
http://SwissalpS.ws/OSprojects/PHPtoolbox
http://SwissalpS.ws/OSprojects/PHPtoolbox/classes/Xtend
Version: 0.3
Date: 20091014
Please use and change freely. Please also rememeber that
we depend on your donation especially if you are using
our scripts to generate income yourself.
https://cash.SwissalpS.biz/donate
*/';

		$this->addStyle($sStyle);
	} // addBasicCSS

	/**
	 * addBasicHead()
	 * add the basic meta tags and CSS
	 *
	 * @return void
	 *
	 * @uses addBasicCSS - add default CSS
	 * @uses i_generator - to reference the generator tag id
	 * @uses setViewWidthScale - set default scale
	 *
	 * @version 20091012_152025 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addBasicHead() {
		// add basic css
		$this->addBasicCSS();

		// add creator meta tag
		$this->i_generator = $this->addTag('meta', $this->i_head, 'name="generator" content="SwissalpS Xtended XHTML"');

		// add scaling options
		$this->setViewWidthScale('device-width', false, 1.0, 1.0, 1.0);
	} // addBasicHead

	/* * *
	 * * integer addScrollBox (
	 * * 		  i_attachTo = 0		the tag id to attach scrollBox to
	 * *		, a_items	 = array()	an array of items arrays: bellow
	 * *		, s_class = null)		additional scrollbox class
	 * *
	 * * 	a_items = array(
	 * *	['title' => array(
	 * *		   'text'		=> 'title text'
	 * *		[, 'class'		=> 'title additional class']
	 * *		[, 'href'		=> 'title link to'
	 * *		[, 'title'		=> 'title title' || NULL {to suppress}]
	 * *		[, 'target'		=> 'title link target frame']
	 * *		[, 'aClass'		=> 'additional class of title link']]
	 * *		[, 'checkbox'	=> array(
	 * *			   'name'		=> 'checkbox name for form'
	 * *			 , 'value'		=> 'value if checked'
	 * *			[, 'class'		=> 'additional class of checkbox']
	 * *			[, 'checked'	=> TRUE || FALSE {initially checked?}]
	 * *		)]
	 * *	)],
	 * *	0..n	 => array(
	 * *		   'text'		=> 'item text'
	 * *		[, 'class'		=> 'additional class of item div']
	 * *		[, 'href'		=> 'item links to'
	 * *		[, 'title'		=> 'item title' || NULL {to suppress}]
	 * *		[, 'target'		=> 'item link target frame']
	 * *		[, 'aClass'		=> 'additional class of item link']]
	 * *		[, 'checkbox'	=> array(
	 * *			   'name'		=> 'checkbox name for form'
	 * *			 , 'value'		=> 'value when checked'
	 * *			[, 'class'		=> 'additional class of checkbox']
	 * *			[, 'checked'	=> TRUE || FALSE { default is false }]
	 * *		)]
	 * *	))
	 * * */
	function addScrollBox ($i_attachTo = 0, $a_items = array(), $s_class = null) {
		// prepare attributes of ScrollBox
		$a_atts = array('class' => "SssS_ScrollBoxContainer");
		// add aditional class s_class
		if (isset($s_class) && ('' != trim($s_class)))
			$a_atts['class'] .= ' ' . $s_class;
		// make container div whose index we will return
		$i_container = $this->addTag('div', $i_attachTo, $a_atts);

		// title of scrollBox
		if (isset($a_items['title']) && ('array' == gettype($a_items['title']))
				&& isset($a_items['title']['text'])) {
			$a_title = $a_items['title'];
			unset($a_items['title']);
			// make title div to contain checkbox and title tag
			$i_title = $this->addTag('div', $i_container
				, array('class' => "SssS_ScrollBoxTitle"));
			// checkbox for title
			if (isset($a_title['checkbox'])
					&& (isset($a_title['checkbox']['name']))
					&& (isset($a_title['checkbox']['value']))) {
				// prepare basic attributes of checkbox for title
				$a_atts = array(
					  'name'  => $a_title['checkbox']['name']
					, 'value' => $a_title['checkbox']['value']);
				// add class attribute
				if (isset($a_title['checkbox']['class'])
					&& ('' != trim($a_title['checkbox']['class'])))
					$a_atts['class'] = $a_title['checkbox']['class'];
				// checked attribute
				if (isset($a_title['checkbox']['checked'])
					&& (true === $a_title['checkbox']['checked']))
					$a_atts['checked'] = "checked";
				// add checkbox
				$this->addTag('input', $i_title, $a_atts);
			} // checkbox for title
			// prepare title text
			$s_text = ('' == trim($a_title['text'])) ? "&nbsp;"
				: $a_title['text'];
			// prepare title attributes
			$a_attsH2 = (isset($a_title['class']))
				? array('class' => $a_title['class']) : array();
			// does our title tag enclose a link anchor?
			if (isset($a_title['href'])) {
				// prepare attributes for href
				$a_atts = array('href' => $a_title['href']);
				// title attribute
				if (isset($a_title['title'])) $a_atts['title'] = $a_title['title'];
				// target attribute
				if (isset($a_title['target'])) $a_atts['target'] = $a_title['target'];
				// class attribute with optional aditional class
				$a_atts['class'] = "SssS_ScrollBoxTitleLink";
				if (isset($a_title['aClass']))
					$a_atts['class'] .= " " . $a_title['aClass'];
				// add title tag with enclosed anchor tag containing text
				$this->addTag('a', $this->addTag('h2', $i_title, $a_attsH2)
					, $a_atts, $s_text);
			} else {
				// add title tag without link
				$this->addTag('h2', $i_title, $a_attsH2, $s_text);
			} // link in title?
		} // title of scrollBox?

		// make the srollBoxDiv
		$i_scrollBox = $this->addTag('div', $i_container
			, array('class' => 'SssS_ScrollBox'));
		// now in the same way as we did the title add all the items
		while ($a_item = array_shift($a_items)) {
			// prepare attributes of ScrollBoxItem
			$a_atts = array('class' => "SssS_ScrollBoxItem");
			// add aditional item class
			if (isset($a_item['class']) && ('' != trim($a_item['class'])))
				$a_atts['class'] .= ' ' . $a_item['class'];
			// make item div to contain checkbox and span tag
			$i_item = $this->addTag('div', $i_scrollBox, $a_atts);
			// checkbox for item
			if (isset($a_item['checkbox'])
					&& (isset($a_item['checkbox']['name']))
					&& (isset($a_item['checkbox']['value']))) {
				// prepare basic attributes of checkbox for title
				$a_atts = array(
					  'name'  => $a_item['checkbox']['name']
					, 'value' => $a_item['checkbox']['value']);
				// add class attribute
				if (isset($a_item['checkbox']['class'])
					&& ('' != trim($a_item['checkbox']['class'])))
					$a_atts['class'] = $a_item['checkbox']['class'];
				// checked attribute
				if (isset($a_item['checkbox']['checked'])
					&& (true === $a_item['checkbox']['checked']))
					$a_atts['checked'] = "checked";
				// add checkbox
				$this->addTag('input', $i_item, $a_atts);
			} // checkbox for item
			// prepare item text
			$s_text = ('' == trim($a_item['text'])) ? "&nbsp;"
				: $a_item['text'];
			// prepare span attributes
			$a_attsSpan = (isset($a_item['class']))
				? array('class' => $a_item['class']) : array();
			// does our span tag enclose a link anchor?
			if (isset($a_item['href'])) {
				// prepare attributes for href
				$a_atts = array('href' => $a_item['href']);
				// title attribute
				if (isset($a_item['title'])) $a_atts['title'] = $a_item['title'];
				// target attribute
				if (isset($a_item['target'])) $a_atts['target'] = $a_item['target'];
				// class attribute with optional aditional class
				$a_atts['class'] = "SssS_ScrollBoxTitleLink";
				if (isset($a_item['aClass']))
					$a_atts['class'] .= " " . $a_item['aClass'];
				// add span tag with enclosed anchor tag containing text
				$this->addTag('a', $this->addTag('span', $i_item, $a_attsSpan)
					, $a_atts, $s_text);
			} else {
				// add span tag without link
				$this->addTag('span', $i_item, $a_attsSpan, $s_text);
			} // link in item span?
		} // scrollBoxItems
		return $i_container;
	} // addScrollBox


/* * * * * /from Xphone.inc\



	/**
	 * addCheckbox($iParent = null, $mArgs = '')
	 * add a checkbox input to iParent. Set name, state with mArgs
	 *
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param mixed $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag
	 * @uses SssS_XHTML::addAttribute
	 * @uses SssS_XHTML::addClassAttribute
	 *
	 * @version 20090728_153600 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addCheckbox($iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$i = $this->addTag('input', $iParent, $mArgs);
		$this->addClassAttribute($i, 'checkbox');
		$this->addAttribute($i, 'checkbox', 'type');
		return $i;
	} // addCheckbox


	/**
	 * addHeader($sTitle = ' - ', $iParent = null, $mArgs = '')
	 * adds a h1 tag with given params to iParent
	 *
	 * @param string $sTitle text to display (defaults to: ' - ')
	 * @param int $iParent tag id to add to  (defaults to: body)
	 * @param mixed $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag - duh
	 *
	 * @version 20090728_011313 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addHeader($sTitle = ' - ', $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		return $this->addTag('h1', $iParent, $mArgs, $sTitle);
	} // addHeader


	/**
	 * addHidden($iParent = null, $sValue = '', $sName = 'hiddenInput', $mArgs = '')
	 * add a hidden input to iParent. Set name, state with mArgs
	 *
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $sValue the value of this input (defaults to: '')
	 * @param string $sName name (defaults to: 'hiddenInput')
	 * @param mixed $mArgs arguments as array or html-ready string (defaults to: '') if value, name or value attributes are passed in this, then they will override those passed with sName, sValue unless they are MT strings
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag
	 * @uses SssS_XHTML::addAttribute
	 * @uses SssS_XHTML::addClassAttribute
	 *
	 * @version 20090728_153600 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addHidden($iParent = null, $sValue = '', $sName = 'hiddenInput', $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$i = $this->addTag('input', $iParent, $mArgs);

		// check if name has already been set and isn't MT
		if ((!isset($this->a_tags[$i]['attributes']['name'])) || (isset($this->a_tags[$i]['attributes']['name']) && '' == $this->a_tags[$i]['attributes']['name'])) $this->addAttribute($i, $sName, 'name');

		// check if value has already been set and isn't MT
		if ((!isset($this->a_tags[$i]['attributes']['value'])) || (isset($this->a_tags[$i]['attributes']['value']) && '' == $this->a_tags[$i]['attributes']['value'])) $this->addAttribute($i, $sValue, 'value');

		// override possibly set type in mArgs
		$this->addAttribute($i, 'hidden', 'type');

		return $i;
	} // addHidden


	/**
	 * addLabel($sTitle = ' - ', $iParent = null, $mArgs = '')
	 * adds a h2 tag with given params to iParent
	 *
	 * @param string $sTitle text to display (defaults to: ' - ')
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 *
	 * @uses SssS_XHTML::addTag - duh
	 *
	 * @version 20090728_012622 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addLabel($sTitle = ' - ', $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		return $this->addTag('h2', $iParent, $mArgs, $sTitle);
	} // addLabel


	/**
	 * addListOrdered($aListItems, $iParent = null, $mArgs = '')
	 * add ol tag with an li tag for each item in aListItems
	 *
	 * @param array $aListItems items for list
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_014724 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addListOrdered($aListItems, $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$iOL = $this->addTag('ol', $iParent, $mArgs);
		foreach($aListItems as $mItem) {
			$this->addTag('li', $iOL, '', strval($mItem));
		} // foreach item
		return $iOL;
	} // addListOrdered


	/**
	 * addListUnordered($aListItems, $iParent = null, $mArgs = '')
	 * add ul tag with an li tag for each item in aListItems
	 *
	 * @param array $aListItems items for list
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_014724 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addListUnordered($aListItems, $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$iUL = $this->addTag('ul', $iParent, $mArgs);
		foreach($aListItems as $mItem) {
			$this->addTag('li', $iUL, '', strval($mItem));
		} // foreach item
		return $iUL;
	} // addListUnordered


	/**
	 * addParagraph($sContents = '', $iParent = null, $mArgs = '')
	 * add a p tag with given params to iParent
	 *
	 * @param string $sContents paragraph text (defaults to: '')
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_013257 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addParagraph($sContents = '', $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		return $this->addTag('p', $iParent, $mArgs, $sContents);
	} // addParagraph

	/**
	 * addPre($sContents = '', $iParent = null, $mArgs = '')
	 * add a pre tag with given params to iParent
	 *
	 * @param string $sContents preformated text (defaults to: '')
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20091012_210900 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addPre($sContents = '', $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		return $this->addTag('pre', $iParent, $mArgs, $sContents);
	} // addPre

	/**
	 * addRadio($iParent = null, $mArgs = '')
	 * add a radio input to iParent. Set name, state with mArgs
	 *
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param mixed $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag
	 * @uses SssS_XHTML::addAttribute
	 * @uses SssS_XHTML::addClassAttribute
	 *
	 * @version 20090728_153600 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addRadio($iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$i = $this->addTag('input', $iParent, $mArgs);
		$this->addClassAttribute($i, 'radio');
		$this->addAttribute($i, 'radio', 'type');
		return $i;
	} // addRadio


	/**
	 * addRectangle($iParent = null, $mArgs = '')
	 * add a div of class rectangle
	 *
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_020332 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addRectangle($iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$i = $this->addTag('div', $iParent, $mArgs);
		$this->addClassAttribute($i, 'rectangle');
		return $i;
	} // addRectangle


	/**
	 * addRectangleWithContentDIV($iParent = null, $mArgsRect = '', $mArgsCont = '')
	 * add a div of class content to a div of class rectangle
	 *
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgsRect arguments for rectangle div tag, as array or html-ready string (defaults to: '')
	 * @param string $mArgsCont arguments for content div tag, as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id of content div (subtract 1 for rectangle div id)
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_020332 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addRectangleWithContentDIV($iParent = null, $mArgsRect = '', $mArgsCont = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$iR = $this->addRectangle($iParent, $mArgsRect);
		$iC = $this->addContentDIV($iR, $mArgsCont);
		return $iC;
	} // addRectangleWithContentDIV


	function addContentDIV($iParent = null, $mArgsCont = '') {
		$iC = $this->addTag('div', $iParent, $mArgsCont);
		$this->addClassAttribute($iC, 'content');
		return $iC;
	} // addContentDIV

	/**
	 * addWheel($aChoices, $mArgs)
	 * select choices tag (on certain browser it's displayed as a wheel rather than a drop-down of choices)
	 *
	 * @param array $aChoices array with options
	 * @param int $iParent (optional) integer id of tag to add wheel to. defaults to body
	 * @param mixed $mArgs (optional) arguments to pass to addTag may be an array or string. eg: 'id="am2" name="am2" onchange="wheel2text();false;" style="width:80px;"'
	 *
	 * @return int tag id of the 'wheel' (select tag)
	 *
	 *
	 * @uses SssS_XHTML::addTag to add the tags
	 *
	 * @version 20090728_003123 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addWheel($aChoices, $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$iSel = $this->addTag('select', $iParent, $mArgs);
		foreach($aChoices as $mChoice) {
			$this->addTag('option', $iSel, '', strval($mChoice));
		} // foreach choice
		return $iSel;
	} // addWheel

/* * \from Xphone.inc/ * * * * * */

	/* * *
	 * * integer function addForm(
	 * *		  i_attachTo = 0		the tag id to attach form into
	 * *		, s_method	 = "post"	the method post/get
	 * *		, s_action	 = _SELF	the url to call
	 * *		, s_enctype	 = "application/x-www-form-urlencoded"
	 * *								the text encoding
	 * *		, s_accChSet = s_charset the charset(s) to use
	 * *		, s_target	 = "_self"	the window to open the result in
	 * *		, s_contents = ""		the form's contents
	 * *
	 * * adds a form tag
	 * *
	 * * returns true on success and false if i_attachTo isn't legal
	 * *
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	function addForm($i_attachTo = 0, $s_action = "", $s_method = "post"
			, $s_enctype = "application/x-www-form-urlencoded"
			, $s_accChSet = "", $s_target = "_self", $s_contents = "") {
		if ((-1 < $i_attachTo) && ($i_attachTo < count($this->a_tags))) {
			$s_method = ("" == $s_method)
					? "post" : $s_method;
			$s_enctype = ("" == $s_enctype)
					? "application/x-www-form-urlencoded" : $s_enctype;
			$s_accChSet = ("" == $s_accChSet)
					? $this->s_charset : $s_accChSet;
			$s_action = ("" == $s_action)
					? $this->getMyURL() : $s_action;
			$a_attributes = array('action' => $s_action
					, 'accept-charset' => $s_accChSet
					, 'enctype' => $s_enctype
					, 'method' => $s_method
					, 'target' => $s_target);

			//$this->a_tags[$i_attachTo]['attributes'][$s_attName]
			//		= $s_attValue;
			return $this->addTag('form', $i_attachTo
					, $a_attributes, $s_contents);
		}
		return false;
	}
	/* * *\ addForm (CC)2007 Luke JZ aka SwissalpS /* * */

	function addClientSniffer($i_idletime = 0, $s_url = ""
			, $s_methods = 'lang,js,cookie,java,fl,qt,wm') {
		$a_methods	= explode(',', str_replace(' ', '', $s_methods));

		/* language part * /
		if (in_array("lang", $a_methods)) {
			$this->getRemLang();
			$s_url = this->f_add2url($s_url, 'l', $this->s_lang);
		} / * lang */

		/* javaScript part */
		if (in_array("js", $a_methods)) {
			// set head meta reload
			$this->addTag('meta', $this->i_head, array(
					'http-equiv' => 'refresh'
					, 'content' => $i_idletime . ";url='"
						. $this->f_add2url($s_url, "js", 0) . "'")
			);

			// set js reload
			$this->addTag('script', $this->i_head
					, array('type' => 'text/javascript')
					, 'window.location="'
						. $this->f_add2url($s_url, "js", 1) . '";'
			);

			// add some style to the message
			$this->addTag('style', $this->i_head
					, array('type' => 'text/css', 'media' => 'screen')
					, '#id_wrap_reload {'		. chr(10)
					. '	color: 888888;'			. chr(10)
					. '	text-align: center;'	. chr(10)
					. '}'
			);

			// give user a message and a link just in case
			$this->addTag('div', $this->i_body
					, array('id' => 'id_wrap_reload')
					, 'If the page doesn\'t refresh, please
				<a href="' . $this->f_add2url($s_url, "js", 0) . '">
					Click Here
				</a>'
			);
		} /* javaScript */

		/* cookie part */
		if (in_array("cookie", $a_methods)) {
			if (!$this->s_ip) $this->getRemIP();
			$s_id1 = 'SssSsniffUnlimited'
					. str_replace('.', '_', $this->s_ip);
			$s_id2 = 'SssSsniffAnual'
					. str_replace('.', '_', $this->s_ip);

			setcookie($s_id1, 'ss', 0, '/');
			setcookie($s_id2, 'st', time()+3600*24*365, '/');
		} /* cookie */

		/* java part */
		if (in_array("java", $a_methods)) {
		} /* java */

		/* flash part */
		if (in_array("fl", $a_methods)) {
		} /* flash */

		/* QuickTime part */
		if (in_array("qt", $a_methods)) {
		} /* QuickTime */

		/* Windows Media Player part */
		if (in_array("wm", $a_methods)) {
		} /* Windows Media Player */
	} /* addClientSniffer */

	function sniffer_readCookies() {
		// get cookies
		$this->getCookies();
		// get remote ip
		if (!$this->s_ip) $this->getRemIP();

		// is there already a session cookie
		$this->b_sCookie = isset($this->a_cookies[$this->s_cn])
				? true : false;
		if (!$this->b_sCookie) {
			// is there a cookie testing cookie
			$s_id1 = 'PHP_SssSsniffUnlimited'
					. str_replace('.', '_', $this->s_ip);
			$s_id2 = 'PHP_SssSsniffAnual'
					. str_replace('.', '_', $this->s_ip);

			$this->b_ssCookies = isset($a_cookies[$s_id1])
					? true : false;
			$this->b_stCookies = isset($a_cookies[$s_id2])
					? true : false;

			// delete cookies
			/* error with headers being sent? seems to be resolved? */
			setcookie($s_id1, '', 0, '/');
			setcookie($s_id2, '', 0, '/');


			// figure out which method we can use
			if ($this->b_stCookies) { // expiring / timed
				$this->s_method = "ct";
			} elseif ($this->b_ssCookies) { // unlimited
				$this->s_method = "cu";
			} elseif (isset($_GET[$this->s_cn])) { // _GET
				$this->s_method = "gt";
			} else { // 1st pass
				$this->s_method = "p1";
			}
		}

		// start session env
		$i_time = ("cu" == $this->s_method) ? 0 : time()+3600*24*21;
		session_name($this->s_cn);
		session_set_cookie_params($i_time, '/');
		session_start();

		if ("p1" == $this->s_method) { // 1st pass
			// add Client Sniffer
			$this->addClientSniffer(0); // 0sec reload if js doesn't work
			$this->resetDeclarations();
			echo $this->XHTML();
			exit;
		}

		// if there was already a valid session, then a var was set
		if (isset($_SESSION['s_method'])) {
			$this->s_method = $_SESSION['s_method'];

			if (isset($_REQUEST['l'])) {
				$s_l = $_REQUEST['l'];
				$this->s_lang = $_SESSION['l'] = (in_array($s_l,
						$this->a_supportedLanguages))
						? $s_l : $this->a_supportedLanguages[1];
			} else $this->s_lang = $_SESSION['l'];

			if (isset($_REQUEST['js'])) {
				$this->b_js = $_SESSION['js'] = (1 == $_REQUEST['js'])
						? true : false;
			} else $this->b_js = $_SESSION['js'];
		} else { // 2nd call - fill the session vars
			$_SESSION['s_method'] = ("" != $this->s_method)
					? $this->s_method : "ct";
			$_SESSION['l'] = $this->getRemLang();
			$_SESSION['js'] = (isset($_GET['js'])) ? ((1 == $_GET['js'])
					? true : false) : false;
			$this->b_js = $_SESSION['js'];
		}
		$this->resetDeclarations();
	} /* sniffer_readCookies */

	function is_cooked() {
		return $this->cooked;
	}

	/**
	 * isSSL()
	 * call on https line?
	 *
	 * @return bool true if https connection, false otherwise
	 *
	 * @version 20090725_152803 (CC)2007 Luke JZ aka SwissalpS
	 */
	function isSSL() {
		$bHTTPS = false;
		if (isset($_SERVER['HTTPS']) && 'on' == $_SERVER['HTTPS']) $bHTTPS = true;
		return $bHTTPS;
	} // isSSL

	/* * *
	 * * function f_add2url(s_url = "", s_setTag = "", s_setVal = "")
	 * *
	 * * edited version to fit into class architecture
	 * * may only be called after session_start() or SID won't exist
	 * * version 0.1. (CC) 2007 Luke JZ aka SwissalpS
	 * * */
	function f_add2url($s_url = "", $s_setTag = "", $s_setVal = "") {
		$a_url = explode("?", $s_url);
		if (1 == count($a_url)) {
			$s_url .= "?" . $s_setTag . "=" . $s_setVal;
			if ("gt" == $this->s_method) $s_url .= "&" . SID;
			return $s_url;
		}

		$s_attributes = $s_found = "";
		foreach(explode("&", $a_url[(count($a_url)-1)])
				as $s_thisAttribute) {
			$a_thisAttribute = explode("=", $s_thisAttribute);
			if ($s_setTag == $a_thisAttribute[0]) {
				$s_found = "Y";
				$s_attributes .= "&" . $s_setTag . "=" . $s_setVal;
			} else $s_attributes .= "&" . $s_thisAttribute;
		}
		$s_attributes .= ("Y" == $s_found) ? ""
				: "&" . $s_setTag . "=" . 		$s_setVal;
		if (("gt" == $this->s_method)
				&& (false == strpos($s_attributes, SID)))
					$s_url .= "&" . SID;
		return $a_url[0] . "?" . substr($s_attributes, 1);
	}
	/* * *\ f_add2url END /* * */

	function bla($s_locale_id = "l", $i_quote_style = ENT_COMPAT
			, $s_enc = "") {
		global $a_locale;
		if ( (!isset($s_enc)) || ("" == $s_enc)) {
			$s_enc = $this->s_charset;
		}
		$sFilePath = $this->s_langPath . $this->s_lang . '.inc';
		if (file_exists($sFilePath)) {
			require_once($sFilePath);
			return (isset($a_locale[$s_locale_id]))
				? (("" == $s_enc)
					? $a_locale[$s_locale_id]
					: htmlentities($a_locale[$s_locale_id], $i_quote_style, $s_enc))
				: $a_locale['l'] . '_' . $s_locale_id;
		} else return $s_locale_id;
	}
	/* * *\ bla END /* * */

	function f_homeURL($s_myURL = "http://swissalps.biz/errors/on=srpskytrck_f_homeURL&external=SRP/") {
		$s_myURL = explode("?", $s_myURL);
		$s_myURL = $s_myURL[0];
		return substr($s_myURL, 0
				, 1 + strrpos($s_myURL, strrchr($s_myURL, "/")));
	} // f_homeURL

	function f_sisterURL($s_pageName) {
		if ((!isset($this->s_myURL))
				|| ("" == $this->s_myURL)) $this->getMyURL();
		$a_myURL = explode("?", $this->s_myURL);
		return f_homeURL($this->s_myURL) . $s_pageName
				. ((isset($a_myURL[1])) ? "?" . $a_myURL[1] : "");
	} // f_sisterURL

	function f_clean($s_varName, $m_returnTypes) {
		/* TO DO: all of it, actually - this is only the structure */
		if (isset($_REQUEST[$s_varName])) {
			/* clean and return */
			return $_REQUEST[$s_varName];
		} elseif (isset($_GET[$s_varName])) {
			/* clean and return */
			return $_GET[$s_varName];
		} elseif (isset($_POST[$s_varName])) {
			/* clean and return */
			return $_POST[$s_varName];
		// cookie
		} else {
			/* clean and return */
			return false;
		}
	} // f_clean

	function setViewWidthScale($iWidth = 980, $bScale = true
		, $iDefault = '1.0', $iMin = '0.25', $iMax = '1.6') {

		$sScale = 'width=' . $iWidth
				. ', user-scalable=' . (($bScale) ? 'yes' : 'no')
				. ', initial-scale=' . $iDefault
				. ', maximum-scale=' . $iMax
				. ', minimum-scale=' . $iMin;

		if (isset($this->i_viewWidthScale)) {
			$this->addAttribute($this->i_viewWidthScale, $sScale, 'content');
		} else {
			$this->i_viewWidthScale = $this->addTag('meta', $this->i_head
					, 'name="viewport" content="' . $sScale . '"');
		} // if add or change tag
	} // setViewWidthScale

/* * * * /geographic location by ip * * * extension BEGIN\ * * * */
	/**
	 * isIP($sIP = '')
	 * is the given string a valid ip
	 *
	 * @param string $sIP ip to check for validity (defaults to: '')
	 *
	 * @return bool false if not IP
	 *
	 * @version 20091011_145745 (CC)2007 Luke JZ aka SwissalpS
	 */
	function isIP($sIP = '') {
		if ($sIP && '' != trim($sIP)) {
			if (preg_match('/^(?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)(?:[.](?:25[0-5]|2[0-4]\d|1\d\d|[1-9]\d|\d)){3}$/', $sIP)) return true;
		} // not null, false or MT

		return false;
	} // isIP

	/**
	 * parseGeoLocIPinfoDBReply($sXML = '')
	 * parse the xml from the db query and return an array
	 *
	 * @param string $sXML the result of http://ipinfodb.com/ip_query.php or ...ip_query_country.php (defaults to: '')
	 *
	 * @return array a_GeoLocResponse
	 *
	 *
	 * @uses SssS_Plist - to parse the xml
	 *
	 * @version 20091011_160554 (CC)2007 Luke JZ aka SwissalpS
	 */
	function parseGeoLocIPinfoDBReply($sXML = '') {
		require_once('SssS_Plist.inc');
		$oP = new SssS_Plist($sXML, $s_charset = '', $s_lang = 'en', $s_XMLversion);

		// the plist parser downcases tags
		if ('response' == $oP->a_tags[0]['tag']) {
			$aChildrenIDs = $oP->a_tags[0]['children'];
			// extract the returned data
			$aResponses = array();
			foreach($aChildrenIDs as $iChild) {
				$aResponses[$oP->a_tags[$iChild]['tag']] = $oP->a_tags[$oP->a_tags[$iChild]['children'][0]]['contents'];
			} // foreach child

			// clean the data and add to self
			foreach($aResponses as $sKey => $mValue) {
				$this->a_GeoLocResponse[$sKey] = (null == $mValue) ? 'n.a' : $mValue;
			} // foreach response - clean
		} // if reply has response base tag

		unset($oP);
		return $this->a_GeoLocResponse;
	} // parseGeoLocIPinfoDBReply

	/**
	 * geoLocIPinfoDBcity($sIP = '')
	 * For city precision, simply do a query at this address
	 * http://ipinfodb.com/ip_query.php?ip=74.125.45.100
	 * (if you omit the IP parameter it will check with your own IP ie the servers ip)
	 *
	 * @param string $sIP ip to lookup
	 *
	 * @return array hash containing the results from ipinfodb.com
	 *
	 *
	 * @uses something - uses_desciption_here
	 *
	 * @version 20091011_143220 (CC)2007 Luke JZ aka SwissalpS
	 */
	function geoLocIPinfoDBcity($sIP = '') {
		$sURL = 'http://ipinfodb.com/ip_query.php' . ( $this->isIP($sIP) ? '?ip=' . $sIP : '');
		$sReply = @file_get_contents_safely($sURL);
		return $this->parseGeoLocIPinfoDBReply($sReply);
	} // geoLocIPinfoDBcity

	/**
	 * geoLocIPinfoDBcountry($sIP)
	 * For country precision (faster), simply do a query at this address
	 * http://ipinfodb.com/ip_query_country.php?ip=74.125.45.100
	 * (if you omit the IP parameter it will check with your own IP ie the servers ip)
	 *
	 * @param string $sIP ip to lookup
	 *
	 * @return array hash containing the results from ipinfodb.com
	 *
	 *
	 * @uses something - uses_desciption_here
	 *
	 * @version 20091011_143255 (CC)2007 Luke JZ aka SwissalpS
	 */
	function geoLocIPinfoDBcountry($sIP = '') {
		$sURL = 'http://ipinfodb.com/ip_query_country.php' . ( $this->isIP($sIP) ? '?ip=' . $sIP : '');
		$sReply = @file_get_contents_safely($sURL);
		return $this->parseGeoLocIPinfoDBReply($sReply);
	} // geoLocIPinfoDBcountry
/* * * * \geographic location by ip * * * extension END/ * * * */
} // SssS_Xtend
?>
