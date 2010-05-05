<?php
/** SssS_Xphone.inc
 * @version 20091024_201959 + file_get/put_contents_safely
 * @version 20090727_235422
 *
 * resources:
 * http://developer.apple.com/safari/library/documentation/AppleApplications/Reference/SafariHTMLRef/Articles/MetaTags.html
**/
if (!defined('AUTO_INCLUDING')) {
	require_once('SssS_Xtend.inc');
} // if not autoincluding but on php's include path

/**
 * extends SssS_XHTML with iPhone specific css and functions to add it's elements.
 * radio input tags take attribute class="radio"
 * checkbox input tags take attribute class="checkbox"
 */
// extends the base class
class SssS_Xphone extends SssS_Xtend {
	// constructor
	function SssS_Xphone($s_charset = '', $s_lang = 'en', $s_XMLversion = '1.0', $i_DTDflag = 0) {
		$s_charset = ('' == $s_charset) ? 'utf-8' : $s_charset;
		$this->SssS_Xtend($s_charset, $s_lang, $s_XMLversion, $i_DTDflag);
		$this->addBasicHead();
	} // constructor


	function addBasicHead() {
		// add basic css
		$this->addBasicCSSiPhone();

		// add creator meta tag
		$this->i_generator = $this->addTag('meta', $this->i_head, 'name="generator" content="SwissalpS Xphone"');

		// add scaling options
		$this->setViewWidthScale('device-width', false, 1.0, 1.0, 1.0);
	} // addBasicHead


	/**
	 * addBasicCSSiPhone($sPath = '')
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
	function addBasicCSSiPhone($sPath = '') {
		$sStyle = '';
		if ('' != $sPath) {
			if (is_file($sPath) && is_readable($sPath)) $sStyle = trim(@file_get_contents_safely($sPath));
		} // if path argument given, read from file

		if ('' == $sStyle) $sStyle = '/*
Author: Brad Micallef
http://www.manage-your-project.com
Version: 1.0
Date: 20070728

This template may be used as-is, modified, and replicated and made into a derivitive work, so long
as this comment block remains intact.

HTML & CSS Template for iPhone
*/
/*Layout and presentation*/
body, html
{
	border: 0;
	margin: 0;
	padding: 0;
	background-color: rgb( 197, 204, 211 );
}
body
{
	padding: 10px;
}
div.content
{
	padding: 10px;
}
/*Font family & sizes*/
div.content, h1, div.rectangle h2, div.cell
{
	font-family: helvetica;
	font-size: 17px;
	font-weight: bold;
	color: #000;
}
div.rectangle p, div.rectangle a, div.rectangle li, div.content, input, textarea, select
{
	font-family: helvetica;
	font-weight: normal;
	font-size: 15px;	/* Apple specifies inner-rectangle text to be 17px 	*/
						/* However in practice this does not appear to be 	*/
						/* correct. Feel free to change this to suit.		*/
}
div.rectangle span
{
	font-family: helvetica;
	font-weight: bold;
	font-size: 15px;
}
/*HTML elements*/
div.rectangle
{
	overflow: hidden;	/* Prevents long unbroken text strings from */
						/* pushing the viewport off-screen			*/
	margin-bottom: 7px;
	background-color: #fff;
	border: 1px solid rgb( 217, 217, 217 );
	-moz-border-radius: 5px;
	-webkit-border-radius: 5px;
}
h1, div.rectangle h2, div.rectangle span, div.rectangle p, div.rectangle a, div.rectangle li
{
	padding: 0;
	margin: 0;
}
h1
{
	font-weight: bold;
	color: rgb( 76, 86, 108 );
}
div.rectangle p
{
	margin-bottom: 7px;
}
div.rectangle h2
{
	font-weight: bold;
}
div.rectangle a
{
	color: rgb( 76, 86, 108 );
}
hr
{
	border: 0;
	height: 1px;
	background-color: rgb( 197, 204, 211 );
	clear: both;
}
/*Field elements*/
input, textarea, select
{
	width: 95%;
}
input.radio, input.checkbox, input.button, input.submit
{
	width: auto;
}
input.button, input.submit
{
	margin-bottom: 10px;
}
input.button
{
	float: left;
}
input.submit
{
	float: right;
}
/*iPhone Cell element*/
div.cell
{
	overflow: hidden;
	padding: 10px;
	border-bottom: 1px solid rgb( 197, 204, 211 );
	position: relative;
}
	div.cell div.link
	{
		position: absolute;
		right: 10px;
		top: 10px;
		width: 20px;
		height: 20px;
		background-image: url( ../img/link.png );
		background-repeat: no-repeat;
		background-position: 0 0;
	}

/* CSS by Brad Micallef ends here */';

		$this->addStyle($sStyle);
	} // addBasicCSSiPhone


	/**
	 * addBoldSpan($sContents = '', $iParent = null, $mArgs = '')
	 * add a span tag with given params to iParent
	 *
	 * @param string $sContents bold text (defaults to: '')
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as array or html-ready string (defaults to: '')
	 *
	 * @return int tag id
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_013257 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addBoldSpan($sContents = '', $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		return $this->addTag('span', $iParent, $mArgs, $sContents);
	} // addBoldSpan


	/**
	 * addCell($sContents = '', $iParent = null, $mArgs = '')
	 * Function_description_here
	 *
	 * @param string $sContents text (defaults to: '')
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgs arguments as hash or html-ready string (defaults to: '')
	 *
	 * @return int return_description_here
	 *
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_030440 (CC)2009 Luke JZ aka SwissalpS
	 */
	function addCell($sContents = '', $iParent = null, $mArgs = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$iC = $this->addTag('div', $iParent, $mArgs, $sContents);
		$this->addClassAttribute($iC, 'cell');
		return $iC;
	} // addCell


	/**
	 * addCellWithLink($sContents = '', $iParent = null, $mArgsAnchor = '', $mArgsCell = '', $mArgsLink = '')
	 * Function_description_here
	 *
	 * @param string $sContents text (defaults to: '')
	 * @param string $iParent tag id to add to (defaults to: body)
	 * @param string $mArgsAnchor arguments as hash or html-ready string for <a> tag (defaults to: '') such as href, target etc.
	 * @param string $mArgsCell arguments as hash or html-ready string for cell (defaults to: '')
	 * @param string $mArgsLink arguments as hash or html-ready string for link div (defaults to: '')
	 *
	 * @return int return_description_here
	 *
	 * @uses SssS_Xphone::addCell
	 * @uses SssS_XHTML::addClassAttribute
	 * @uses SssS_XHTML::addTag
	 *
	 * @version 20090728_030440 (CC)2009 Luke JZ aka SwissalpS
	 */
	function addCellWithLink($sContents = '', $iParent = null, $mArgsAnchor = '', $mArgsCell = '', $mArgsLink = '') {
		if (null == $iParent) $iParent = $this->i_body;
		$iH = $this->addTag('a', $iParent, $mArgsAnchor);
		$iC = $this->addCell($sContents, $iH, $mArgsCell);
		$iL = $this->addTag('div', $iC, $mArgsLink);
		$this->addClassAttribute($iL, 'link');
		return $iC;
	} // addCellWithLink


	// addSearch
	// TODO: http://developer.apple.com/safari/library/documentation/AppleApplications/Reference/SafariHTMLRef/Articles/InputTypes.html#//apple_ref/doc/uid/TP40008055

	/**
	 * addSlider($iMin = 0, $iMax = 100, $iStep = 1)
	 * Function_description_here
	 *
	 * @param int $iMin parameter_description_here (defaults to: 0)
	 * @param int $iMax parameter_description_here (defaults to: 100)
	 * @param int $iStep parameter_description_here (defaults to: 1)
)
	 *
	 * @return void return_description_here
	 *
	 *
	 * @uses something - uses_desciption_here
	 *
	 * @version 20090919_050847 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addSlider($iParent = null, $mArgs = '', $iMin = 0, $iMax = 100, $iStep = 1) {
		if (null == $iParent) $iParent = $this->i_body;
		$iSlider = $this->addTag('input', $iParent, $mArgs);
		$this->addClassAttribute($iSlider, 'range');
		$this->addAttribute($iSlider, 'range', 'type');
		$this->addAttribute($iSlider, $iMin, 'min');
		$this->addAttribute($iSlider, $iMax, 'max');
		$this->addAttribute($iSlider, $iStep, 'step');
		return $iSlider;
	} // addSlider


	function setTelephoneDetection($bSwitch = true) {
		$sTag = 'telephone=' . (($bSwitch) ? 'yes' : 'no');
		if (isset($this->i_telDetection)) {
			$this->addAttribute($this->i_telDetection, $sTag, 'content');
		} else {
			$this->i_telDetection = $this->addTag('meta', $this->i_head
					, array('name' => 'format-detection', 'content' => $sTag));
		} // if add or change tag
	} // setTelephoneDetection


	function setWebAppStatusBar($sStyle = 'default') {
		// quote from SRL
		/* This meta tag has no effect unless you first specify full-screen mode as described in “apple-mobile-web-app-capable.”
If content is set to default, the status bar appears normal.
If set to black, the status bar has a black background.
If set to black-translucent, the status bar is black and translucent.
If set to default or black, the web content is displayed below the status bar.
If set to black-translucent, the web content is displayed on the entire screen, partially obscured by the status bar. The default value is default.*/
		if (!in_array($sStyle, array('black', 'black-translucent'))) $sStyle = 'default';

		if (isset($this->i_wapFullScreen)) {
			//$this->addAttribute($this->i_wapFullScreen, 'yes', 'content');
		} else {
			$this->i_wapFullScreen = $this->addTag('meta', $this->i_head
					, array('name' => 'apple-mobile-web-app-capable', 'content' => 'yes'));
		} // if add or change tag1

		if (isset($this->i_wapStatusBarStyle)) {
			$this->addAttribute($this->i_wapStatusBarStyle, $sStyle, 'content');
		} else {
			$this->i_wapStatusBarStyle = $this->addTag('meta', $this->i_head
					, array('name' => 'apple-mobile-web-app-status-bar-style', 'content' => $sStyle));
		} // if add or change tag2
	} // setWebAppStatusBar

	// defaults for iPhone according to Apple guidlines
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
} // SssS_Xphone
/* * *\ SssS_Xphone.inc (CC)2009 Luke JZ aka SwissalpS /* * */
?>
