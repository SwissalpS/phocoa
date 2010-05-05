<?php
/* * * *
 * * SssS_xhtml.inc
 * *
 * * class SssS_XHTML(
 * *		  s_charset		= 'UTF-8'	charset
 * *		, s_lang 			= 'en'	2 character language id
 * *		, $s_XMLversion 	= '1.0'	xml version for xml declaration
 * *		, i_DTDflag		= 0)
 * *
 * * class to build xhtml page with doctype and xml declarations
 * * predefines the base tags: html, head, title, body and a meta tag
 * * declaring the charset
 * *
 * * see example.php for usage
 * * @version 20091024_201456 + require functions file_get/put_contents_safely
 * * @version 20090803_220831 (CC)2008 Luke JZ aka SwissalpS mb_ detection
 * * @version 0.2 (CC)2008 Luke JZ aka SwissalpS (modified outputTag and added XHTMLaa to enable automatic insertion of id, name and class attributes. In the hope that this makes css and scripting easier for certain people like myself when planing dynamic content. Also added double check on tag and attribute names to ensure that they are lowercase)
 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
 * * * */
/**
 * class to build xhtml page with doctype and xml declarations
 * predefines the base tags: html, head, title, body and a meta tag
 * declaring the charset
 * this class was written with old systems in mind. Systems that don't have fancy xml libraries
 * other criteria was to have a sound base for creating html content dynamically
 * no script dependency, neither php nor javascript
 * extendable to easily add above features eg SssS_Xtend which includes client sniffer and lots more
 * or going in the other xml direction SssS_Plist which adds reading capabilities of the tightly defined plist format. Mac's default property list style. (don't confuse with the binary plist format)
 *
 * Features:
 * - output correct human readable xhtml code with declarations
 * - output correct human readable xhtml code without declarations from any node
 * - adds basic tags automatically (html, head, title, meta charset, body)
 * - manages scripts and styles adding them as tag or coalascating with existing tags in the head
 * - a basic DOM tree with paths (SssS_DOM {subclass} is planned to include a getElementsOfPath() and node duplicating functions)
 */
require_once('SssS_FileGetPutContents.inc');
class SssS_XHTML {
	/* base vars */
 	/**
 	 * @var array array tree of tags (DOM) can hold several root tags
 	 */
	var $a_tags;		// the main array holding all tags
 	/**
 	 * @var string defaults to 'UTF-8'
 	 */
	var $s_charset;		// 'UTF-8' etc.
 	/**
 	 * @var string language identifier string ('en', 'de', 'fr',...)
 	 */
	var $s_lang;		// 'en', 'de', 'fr' etc.
 	/**
 	 * @var int used to set the XHTML declaration read SssS_XHTML::SssS_XHTML() and SssS_XHTML::getDocType()
 	 */
	var $i_DTDflag;		// read description of function SssS_xhtml and look at getDocType
 	/**
 	 * @var string the full doctype declaration
 	 */
	var $s_docType;		// the doctype declaration string used by XHTML()
 	/**
 	 * @var string XML declaration string
 	 */
	var $s_XMLdeclaration; // the xml declaration string used by XHTML()
 	/**
 	 * @var string XML version, used for XML declaration
 	 */
	var $s_XMLversion;	// the xml version to use when making declaration

	/* basic tags' id */
 	/**
 	 * @var int id of root html tag
 	 */
	var $i_html;		// id of root html tag
 	/**
 	 * @var int id of head tag
 	 */
	var $i_head;		// id of head tag
 	/**
 	 * @var int id of title tag
 	 */
	var $i_title;		// id of title tag
 	/**
 	 * @var int id of style tag - use addStyle()
 	 */
	var $i_style;		// id of style tag - use SssS_XHTML::addStyle()
 	/**
 	 * @var int id of script tag - use addScript()
 	 */
	var $i_script;		// id of script tag - use SssS_XHTML::addScript()
 	/**
 	 * @var int id of meta tag charset
 	 */
	var $i_mChar;		// id of meta tag charset
 	/**
 	 * @var int id of body tag
 	 */
	var $i_body;		// id of body tag

	/*controllers*/
 	/**
 	 * @var array array of strings of tags which will be ignored when using SssS_XHTML::XHTMLaa() or the newline-flag in SssS_XHTML::outputTag()
 	 */
	var $a_skipTheseFromFlagIdNameClass; // array that holds tags which will be ignored when using SssS_XHTML::XHTMLaa() or the newline-flag in SssS_XHTML::outputTag()
 	/**
 	 * @var string textnodes need a common tag to help sort and filter them, this should be some tag you don't use elsewhere. Defaults to "ssss_xml_textnode"
 	 */
	var $s_textNodeName;	// #textnodes need a common tag to help sort and filter them, this should be some tag you don't use elsewhere
 	/**
 	 * @var array array of strings of tags which take a new line after they close
 	 */
	var $a_newLineAfterCloseTags;
 	/**
 	 * @var array array of strings of tags which take a new line after them
 	 * @deprecated no longer used as it isn't as simple as that... will be removed soon
 	 */
	var $a_newLineAfterTags;
 	/**
 	 * @var array array of strings of tags which take a new line after they open
 	 */
	var $a_newLineAfterOpenTags;

	/* * *
	 * * void function SssS_XHTML(
	 * *		  s_charset	= 'UTF-8'	charset
	 * *		, s_lang 		= 'en'	2 character language id
	 * *		, s_XMLversion = '1.0'	xml version for declaration
	 * *		, i_DTDflag	= 0)	table bellow
	 * *	0 = XHTML1.0 transitional
	 * *	1 = XHTML1.0 frameset
	 * *	2 = XHTML1.0 strict
	 * *	3 = XHTML1.1
	 * *	4 = PLIST (I know, this has nothing to do with HTML. I use it often, that's why I included it. Use the subclass SssS_Plist though to get good results including import
	 * *	(5 = XHTML2)
	 * *
	 * * inits the class
	 * * sets the base tags and their id vars:
	 * * 	s_charset, s_lang, s_XMLversion, s_docType, s_XMLdeclaration,
	 * *	i_html, i_head, i_title, i_body, i_mChar
	 * *
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS (added a_skipTheseFromFlagIdNameClass; needed for the new outputTag and XHTMLaa)
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	 /**
	  * constructs the SssS_XHTML object
	  *
	  * $i_DTDflag value table:
	  *
	  *	--   0 = XHTML1.0 transitional
	  *
	  *	--   1 = XHTML1.0 frameset
	  *
	  *	--   2 = XHTML1.0 strict
	  *
	  * --   3 = XHTML1.1
	  *
	  * --   4 = PLIST (I know, this has nothing to do with HTML. I use it often, that's why I included it. Use the subclass SssS_Plist though to get good results including import(!)
	  *
	  * --   (5 = XHTML2) not implemented due to change as can all higher values, I'll add them as I need them, for now I can cover theoretically every xml variation as I can wrap an arbitray branch into whatever format. Tags don't have to be (x)html, xml is the format, freedom your limit.
	  *
	  *
	  * @param string $s_charset (optional) the encoding to use, defaults to "UTF-8"
	  * @param string $s_lang (optional) the language identifier, defaults to "en"
	  * @param string $s_XMLversion (optional) xml version string, defaults to "1.0"
	  * @param int $i_DTDflag (optional) defaults to 0 (XHTML1.0 transitional)
	  *
	  * @uses SssS_XHTML::resetDeclarations()
	  * @uses SssS_XHTML::addTag() to add basic xhtml tags
	  * @uses SssS_XHTML::$s_charset sets it with the passed parameter or default ("UTF-8") and then to handle string conversion later on
	  * @uses SssS_XHTML::$s_lang set to lowercase of passed value
	  * @uses SssS_XHTML::$s_XMLversion set to "1.0" or the passed value
	  * @uses SssS_XHTML::$i_DTDflag set to passed value or if invalid to 3 -> XHTML1.1
	  * @uses SssS_XHTML::$a_tags resets it to empty array
	  * @uses SssS_XHTML::$a_skipTheseFromFlagIdNameClass set to array("br") (you might want to add others like "hr")
	  * @uses SssS_XHTML::$a_newLineAfterOpenTags preset with defaults (echo var_dump to see which)
	  * @uses SssS_XHTML::$a_newLineAfterCloseTags preset with defaults (echo var_dump to see which)
	  * @uses SssS_XHTML::$s_textNodeName set to "ssss_xml_textnode"
	  * @uses SssS_XHTML::$i_html set to root id (0)
	  * @uses SssS_XHTML::$i_head set id
	  * @uses SssS_XHTML::$i_title set id and title tag content
	  * @uses SssS_XHTML::$i_body set id
	  * @uses SssS_XHTML::$i_mChar set id and charset meta tag
	  *
	  * @version 0.2 (CC)2008 Luke JZ aka SwissalpS ( added SssS_XHTML::$a_skipTheseFromFlagIdNameClass; needed for the new SssS_XHTML::outputTag() and SssS_XHTML::XHTMLaa() )
	  * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	  */
	  // TO DO: read the defaults from files in a conf directory to emphasise the modularity and simple management. On the other hand, this class is designed to fit into larger projects which probably already have those abilities and reuse them to integrate/bootstrap these defaults. Stay *NIX SwissalpS!
	function SssS_XHTML($s_charset = 'UTF-8', $s_lang = 'en'
			, $s_XMLversion = '1.0', $i_DTDflag = 0) {

		static $bMB_strtolower; // TODO: global? class static?
		if (!isset($bMB_strtolower))
				$bMB_strtolower = function_exists('mb_strtolower');

		$this->s_charset = strtoupper($s_charset);
		$this->s_lang = ($bMB_strtolower)
				? mb_strtolower($s_lang, $s_charset)
				: strtolower($s_lang);
		$this->s_XMLversion = ('' == $s_XMLversion) ? '1.0' : $s_XMLversion;
		$this->i_DTDflag = ((-1 < $i_DTDflag) && (5 > $i_DTDflag))
			? $i_DTDflag : 3;

		$this->a_tags = array();

		$this->a_skipTheseFromFlagIdNameClass = array('br');
		$this->a_newLineAfterOpenTags = array('pre', 'p', 'div', 'link', 'head', 'html', 'meta', 'title', 'table', 'form', 'style', 'script', 'br', 'tr', 'hr', 'td');
		$this->a_newLineAfterCloseTags = array('pre', 'p', 'div', 'link', 'head', 'html', 'meta', 'title', 'table', 'form', 'style', 'script', 'br');
		//
		$this->s_textNodeName = 'ssss_xml_textnode';

		/* * add basic xhtml tags * */
		$this->resetDeclarations();
		$a_args = array('xmlns'			=> 'http://www.w3.org/1999/xhtml'
						, 'xml:lang'	=> $this->s_lang);
		if (3 > $this->i_DTDflag) $a_args['lang'] = $this->s_lang;
		$this->i_html = $this->addTag('html', -1, $a_args);
		$this->i_head	= $this->addTag('head');
		$this->i_title	= $this->addTag('title', $this->i_head, ''
				, 'xhtml made with php class by SwissalpS.Luke');
		$this->i_body 	= $this->addTag('body');
		$this->i_mChar 	= $this->addTag('meta', $this->i_head
				, array('http-equiv'	=> 'content-type'
					, 'content'			=> 'text/html;charset='
											. $this->s_charset));
		unset($a_args);
	}
	/* * *\ SssS_XHTML (CC)2007 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * boolean function addAttribute(
	 * *		  i_tag = -1			the tag id to add attribute to
	 * *		, s_attValue = ""		the attribute's value
	 * *		, s_attName = 'id')	the attribute's name
	 * *
	 * * adds or changes the value of an attribute
	 * *
	 * * returns true on success and false if i_tag isn't legal
	 * *
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS (now makes sure attribute name is lowercase already when adding. B4 it was only lowercased when outputing)
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * adds or changes the value of a tag's attribute
	 *
	 * @param int $i_tag tag id whose attributes we are changing
	 * @param string $s_attValue the value of the attribute (defaults to "")
	 * @param string $s_attName the attributes name (defaults to "id")
	 * @return bool true on success and false if $i_tag isn't valid or a text node
	 *
	 * @uses SssS_XHTML::isValidTagID() to check $i_tag
	 * @uses SssS_XHTML::$a_tags read and write access
	 *
	 * @todo mb-support(? not really, would anyone use mb chars in a tag? that wouldn't be clean, but then again we do specify the charset... thoughts?)
	 *
	 * @version 0.2 (CC)2008 Luke JZ aka SwissalpS (now makes sure attribute name is lowercase already when adding. B4 it was only lowercased when outputing)
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addAttribute($i_tag = -1, $s_attValue = '', $s_attName = 'id') {
		if (!$this->isValidTagID($i_tag)) return false;
		// text nodes don't have attributes
		if ($this->s_textNodeName == $this->a_tags[$i_tag]['tag']) return false;

		$this->a_tags[$i_tag]['attributes'][strtolower($s_attName)]	= $s_attValue;
		return true;
	}
	/* * *\ addAttribute (CC)2007 Luke JZ aka SwissalpS /* * */

	/**
	 * addClassAttribute($i_tag = -1, $s_className = '')
	 * adds aditional class to class attribute
	 *
	 * @param int $i_tag tag id whose class attribute we are changing
	 * @param string $s_className name of the class (defaults to: '')
	 *
	 * @return bool true on success
	 *
	 * @uses SssS_XHTML::isValidTagID() to check $i_tag
	 * @uses SssS_XHTML::$a_tags read and write access
	 *
	 * @version 20090728_024004 (CC)2009 Luke JZ aka SwissalpS
	 */
	function addClassAttribute($i_tag = -1, $s_className = '') {
		if (!$this->isValidTagID($i_tag)) return false;
		// text nodes don't have attributes
		if ($this->s_textNodeName == $this->a_tags[$i_tag]['tag']) return false;

		if (isset($this->a_tags[$i_tag]['attributes']['class'])) {
			// already set, let's add
			$this->a_tags[$i_tag]['attributes']['class'] = trim($this->a_tags[$i_tag]['attributes']['class'] . ' ' . strval($s_className));
		} else {
			// not yet set, let's add new one
			$this->addAttribute($i_tag, trim(strval($s_className)), 'class');
		} // if - add new or append to class attribute
		return true;
	} // addClassAttribute

	/* integer or string addScript(string s_script		optional script as string in '<>' or not or empty when suplying s_url
	 * *					, integer i_attachTo	optional id of tag to attach this script to. Default is i_head
	 * *					, string s_url)		optional url of script
	 * *
	 * *	returns integer id of script's tag if it has been added to something as standalone or textnode
	 * *	returns string s_script if script was added to the collective script-tag in i_head
	 * *
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS (now we sort text nodes and inner nodes correctly, changes shouldn't affect scripts using 0.1 )
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * handles script adding
	 *
	 * - add a script to the global script section in head: addScript("your javascript code")
	 * - add a script link in head: addScript("", "", "http://other.domain.tld/other.js)
	 * - add a javascript tag to a tag: addScript("your javascript code", int_id_of_receiving_tag)
	 * - add an inline script with custom language and attributes (copy paste plugins): addScript("<script type="text/javascript">The custom script</script>")
	 * - add an inline script with custom language and attributes to a tag (copy paste plugins): addScript("<script type="text/javascript">The custom script</script>", int_id_of_receiving_tag)
	 *
	 * @param string $s_script the script (assuming javascript if it doesn't begin with a '<' which assumes to be tagged, attributed and closed correctly)
	 * @param int $i_attachTo (optional) tag id to attach $s_script to
	 * @param string $s_url (optional) include this script url in head. The other two arguments should be "" if this is used
	 *
	 * @return int|string id of script tag if it has been added to something as standalone or textnode or string $s_script if script was added to the collective script-tag in SssS_XHTML::$i_head
	 *
	 * @uses SssS_XHTML::addTextNode() to add $s_script that has own tags ie begins with "<". Or to add $s_script to existing script tag in header
	 * @uses SssS_XHTML::addTag() to add a new script tag in header or attached to another tag but also to add the script url to head
	 * @uses SssS_XHTML::$i_head reads
	 * @uses SssS_XHTML::$a_tags reads (writes with handlers)
	 * @uses SssS_XHTML::$s_textNodeName reads to identify text nodes
	 * @uses SssS_XHTML::$i_script to initialize and use
	 *
	 * @version 0.2 (CC)2008 Luke JZ aka SwissalpS (now we sort text nodes and inner nodes correctly, changes shouldn't affect scripts using 0.1 )
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addScript($s_script = '', $i_attachTo = '', $s_url = '') {
		$b_urlIsSet = ('' !== trim($s_url)) ? true : false;
		$b_scriptIsSet = ('' !== trim($s_script)) ? true : false;
		$i_tag = $this->i_head;
		$i_countTags = count($this->a_tags);
		// TO DO: these two lines look weird: b_attachToIstextNode is used uninitialized to influence it's initial value
		$b_isValidAttachTo = (('' !== trim($i_attachTo)) && (0 < $i_attachTo)
				&& ($i_attachTo < $i_countTags) && $b_attachToIstextNode) ? true : false;
		$b_attachToIsTextNode = ($b_isValidAttachTo && ($this->s_textNodeName == $this->a_tags[$i_attachTo]['tag'])) ? true : false;
		if ($b_attachToIsTextNode) {
			// if (parent exists) $i_tag = first possible parent
			if ($b_isValidAttachTo) $i_tag = $this->a_tags[$i_attachTo]['parent'];
			else $b_isValidAttachTo = false;
		}

		if ($b_scriptIsSet || $b_urlIsSet) {
			if ((isset($s_script[0])) && ('<' == $s_script[0])) { // s_script begins with'<', a good sign for custom script header, something quick or from a template
				return $this->addTextNode($i_tag, chr(10) . $s_script);
				/* 0.1 we still didn't have sorted textnodes
				$this->a_tags[$i_tag]['contents'] .= chr(10) . $s_script;
				return $i_tag;*/
			} else { // not valid s_script in some sense
				if ($b_urlIsSet) { // url is given
					return $this->addTag('script', $i_tag
							, array('type' => 'text/javascript'
									, 'src' => $s_url), ' ');
				} else { // url wasn't supplied and s_script isn't a standalone tag set
					if ($b_isValidAttachTo) { // at least i_attachTo is valid
						return $this->addTag('script', $i_tag
								, array('type' => 'text/javascript')
								, $s_script);
					} else { // no where else to put this script but in the head
						if (!isset($this->i_script)) { // create script tag in head
							$this->i_script = $this->addTag('script', $this->i_head
									, array('type' => 'text/javascript'));
						}
						$this->addTextNode($this->i_script, chr(10) . $s_script);
							/* 0.1 we didn't have textnodes
							$this->a_tags[$this->i_script]['contents'] .= chr(10) . $s_script;*/
						return $this->i_script;
		}	}	}	}
		return false;
	} // addScript


	/**
	 * handles style adding
	 *
	 * - add a style portion to the global style section in head: addStyle("your css code")
	 * - add a style portion to the global style section in head reading style from file: addStyle("", "/path/to/your/style.css")
	 * - add a style link in head: addStyle("", "", "http://other.domain.tld/other.css)
	 *
	 * - add a style attribute to a tag: addAttribute(int_id_of_receiving_tag, 'css code', 'style')
	 * - add an inline style tag with custom attributes (copy paste css): addTag('style', int_id_of_receiving_tag, 'attribute="value"', 'The custom style')
	 *
	 * @param string $s_style (optional) the style (assuming css) if the other two parameters don't result in *something* then $s_style is added as child text node of global style declaration of head tag.
	 * @param string $s_path (optional) path to read $s_style from, doesn't failsafe to passed $s_style. Only processed if $s_url isn't supplied.
	 * @param string $s_url (optional) include this style url in head as link tag. The other two arguments are ignored.
	 *
	 * @return int|bool int id of script text node tag if style has been added to global script tag or the id of the new link tag of the head tag when passing $s_url. false if nothing fit.
	 *
	 * @uses SssS_XHTML::addTextNode() to add $s_style (from file or passed) to existing style tag in header
	 * @uses SssS_XHTML::addTag() to add a new style or link tag in header
	 * @uses SssS_XHTML::$i_head reads
	 * @uses SssS_XHTML::$i_style to initialize and use
	 *
	 * @version 0.2 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addStyle($s_style = '', $s_path = '', $s_url = '') {
		if ('' !== trim($s_url)) { // url suplied
				return $this->addTag('link', $this->i_head, array(
					  'type' => 'text/css'	, 'media' => 'screen'
					, 'rel'  => 'stylesheet', 'href'  => $s_url));
		}
		if ('' !== trim($s_path)) $s_style = @file_get_contents_safely($s_path);
		if (('' !== trim($s_style)) && (false !== $s_style)) { // read ok and not MT
			if (!isset($this->i_style)) { // make i_style
				$this->i_style = $this->addTag('style', $this->i_head
						, array('type'  => 'text/css', 'media' => 'screen'));
			} // make i_style
			return $this->addTextNode($this->i_style, chr(10) . $s_style);
				/* 0.2 is removing contents and using textnodes instead to guarant order. Text nodes are the only nodes with contents set
				this is how it was done in 0.1 to help understand when updating scripts using 0.1
				$this->a_tags[$this->i_style]['contents'] .= chr(10) . $s_style;*/
		}
		return false;
	} // addStyle

	/* * *
	 * * integer function addTag(
	 * *	  s_tag			= "span"	tag name
	 * *	, i_parent		= 0			the id to add a new tag to
	 * *	, m_args		= ""		array("attName" => "attValue")
	 * *	, $s_contents 	= "")		text/xhtml
	 * *
	 * * adds a child to i_parent
	 * * attributes can also be added later with addAttribute()
	 * * TO DO: make it possible to pass atts as string done as of 0.2
	 * *
	 * * returns the new tag's id as unsigned integer
	 * *
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS (now makes sure attributes are lowercase, it is now possible to pass attributes as array or as string, also the tag is now positioned inside it's parents contents-> won't work for nodes in root)
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * adds a child tag to tag with id $i_parent
	 *
	 * the tags attributes can be added later with SssS_XHTML::addAttribute()
	 *
	 * @param string $s_tag (optional, defaults to "span") tag label/name
	 * @param int $i_parent (optional, defaults to 0 ie root tag) tag id of receiving tag
	 * @param string|array $m_args (optional, defaults to "") attributes passed as key-value array eg: array('height' => 456, 'width' => 321) or xml attribute string eg: 'height="456" width="321"'
	 * @param string $s_contents (optional, defaults to "") the tags 'innerHTML' is added as a textnode
	 * @return int the new tag's id as (unsigned) integer
	 *
	 * @todo there are still some strings being processed without mb-support: make a way to toggle as some older systems (our targets don't all have mb_substr and partners available.
	 *
	 * @uses SssS_XHTML::isValidTagID() to check $i_parent
	 * @uses SssS_XHTML::addTextNode() to add loose text
	 * @uses SssS_XHTML::$a_tags read and write access
	 * @uses SssS_XHTML::$s_textNodeName to detect text node tag
	 *
	 * @version 0.3 (CC)2009 Luke JZ aka SwissalpS checks $i_parent for validity with SssS_XHTML::isValidTagID()
	 * @version 0.2 (CC)2008 Luke JZ aka SwissalpS (now makes sure attributes are lowercase, it is now possible to pass attributes as array or as string, also the tag is now positioned inside it's parents contents-> won't work for nodes in root)
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function addTag($s_tag = 'span', $i_parent = 0, $m_args = '', $s_contents = '') {
		$a_attributes = $a_args = array();

		/* 090518 SwissalpS -> ver 0.3
		// clean parent id by setting it to -1 if given id isn't valid and positive (due to become valid after adding more tags) */
		if (!$this->isValidTagID($i_parent) && (-1 < $i_parent)) $i_parent = -1;

		// are there args and how have they been supplied
		if (is_array($m_args)) $a_args = $m_args; // attributes passed as array
		elseif ('' != trim($m_args)) { // attributes passed as string
			$s_args = $m_args;
			$s_search =	  '_'		//
				. '(\w+\s*=\s*(?:\".*?\"|\'.*?\'|[^\'\">\s]+))+'	// attribute
				. '_si';
			preg_match_all($s_search, $s_args, $a_matches, PREG_SET_ORDER);
			foreach ($a_matches as $a_match) { $s_match = $a_match[0];
				$i_split = stripos($s_match, '='); if (!$i_split) break;
				$s_attribute = trim(substr($s_match, 0, $i_split));
				$s_value = trim(substr($s_match, $i_split +1, -1));
				$s_value = str_replace(array('"', "'"), '', $s_value);
				$a_args[$s_attribute] = $s_value;
		}	} // attributes passed as string

		// set a_attributes with lowercasse attributes
		foreach ($a_args as $s_name => $s_value) { $a_attributes[strtolower($s_name)] = $s_value; }

		// next id
		$i_thisID = count($this->a_tags);

		// add the tag to main array
		$this->a_tags[] = array(
				'tag' 			=> strtolower($s_tag)
				, 'attributes'	=> $a_attributes
				, 'contents' 	=> ''					// default is '' only textnodes have some
				, 'parent' 		=> $i_parent
				, 'children' 	=> array()
				, 'level'		=> 0
				, 'pathbyid'	=> '/0'
				, 'pathbyname'	=> '/' . strtolower($s_tag)
		);

		// base tag has no parent, orphans don't either, filter them
		if (-1 < $i_parent) {
			// add child to parent
			$this->a_tags[$i_parent]['children'][] = $i_thisID;
			// add stuff we only add to lower levels
			$this->a_tags[$i_thisID]['level'] = $this->a_tags[$i_parent]['level']+1;
			$this->a_tags[$i_thisID]['pathbyid'] = $this->a_tags[$i_parent]['pathbyid'] . '/' . $i_thisID;
			$this->a_tags[$i_thisID]['pathbyname'] = $this->a_tags[$i_parent]['pathbyname'] . '/' . $this->a_tags[$i_thisID]['tag'];
		}

		if ('' != trim($s_contents)) { // we have some loose text
			if ($this->s_textNodeName == $s_tag) $this->a_tags[$i_thisID]['contents'] = $s_contents;
			else $this->addTextNode($i_thisID, $s_contents);
		}
		return $i_thisID;
	}
	/* * *\ addTag (CC)2007 Luke JZ aka SwissalpS /* * */

	/**
	 * adds a text node to $i_attachTo
	 *
	 * @param int $i_attachTo the tag id to attach the text node to. If this is a text node itself, then the new node is attached to $i_attachTo's parent as another child
	 * @param string $s_contents the string contents, this may be just text, xml, xhtml or the output of another tag -> SssS_XHTML::outputTag()
	 *
	 * @return bool|int false if $i_attachTo isn't a valid id otherwise returns id of new tag
	 *
	 * @uses SssS_XHTML::isValidTagID() to check $i_attachTo
	 * @uses SssS_XHTML::addTag() to add the textnode
	 * @uses SssS_XHTML::$s_textNodeName to add the textnode
	 * @uses SssS_XHTML::$a_tags for reading
	 *
	 * @version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 */
	// integer or boolean false addTextNode(i_attachTo, s_contents)
	function addTextNode($i_attachTo, $s_contents) {
		if ($this->isValidTagID($i_attachTo)) {
			if ($this->s_textNodeName == $this->a_tags[$i_attachTo]['tag']) $i_attachTo = $this->a_tags[$i_attachTo]['parent'];
			return $this->addTag($this->s_textNodeName, $i_attachTo, '', $s_contents);
		}
		return false;
	} // addTextNode

	/* * *
	 * * string function getAttributes(i_tag = -1)
	 * *
	 * * returns attributes in a string ready for html
	 * *
	 * * TO DO: test if it throws an error when wrong input
	 * *
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * returns attributes of a tag in a string ready for html
	 *
	 * @param int $i_tag tag id to read attributes from
	 *
	 * @return string ready for html insertion
	 *
	 * @uses SssS_XHTML::isValidTagID() to check $i_tag
	 * @uses SssS_XHTML::$a_tags to read attributes
	 *
	 * @todo test if it throws an error when wrong input
	 * @todo check if we need to use mb_strtolower and substr
	 *
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function getAttributes($i_tag = -1) {
		$s_return = '';

		if ($this->isValidTagID($i_tag)) {
			foreach ($this->a_tags[$i_tag]['attributes'] as $att => $val) {
				if ('' != $val) {
					$s_return .= strtolower($att) . '="' . $val . '" ';
		}	}	}
		return substr($s_return, 0, -1);
	}
	/* * *\ getAttributes (CC)2007 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * string function getDocType()
	 * *
	 * * returns xhtml doctype declaration as a string ready for html
	 * *
	 * * TO DO: allow other flavours -> done, now we need to keep a few standards in an array ready for quick access, at least have the 3 standard html doctypes handy
	 * * <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">
	 * * <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
	 * * <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
	 * * this is what we should use:
	 * * <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
	 * *
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * xhtml doctype declaration as a string ready for html
	 *
	 * @uses SssS_XHTML::$s_lang read to complete the doctype declaration
	 * @uses SssS_XHTML::$i_DTDflag read to determine which template to use
	 *
	 * @return string xhtml doctype declaration as a string ready for html
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function getDocType() {
		$a_DTDs = array(
			0 => array( // XHTML1.0 transitional
			'root'		// root element
				=> 'html',
			'systemID'	// PUBLIC or PRIVATE
				=> 'PUBLIC',
			'FPI1'		// if above is PUBLIC this is the first part of the DTD declaration
				=> 'W3C',
			'FPI2'		// FPI if PUBLIC  (Formal Public Identifier) in xhtml1.1 it can be 1
				=> 'DTD XHTML 1.0 Transitional',
			'URI'		// dtd uri
				=> 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'),
			1 => array( // XHTML1.0 strict
			'root'
				=> 'html',
			'systemID'
				=> 'PUBLIC',
			'FPI1'
				=> 'W3C',
			'FPI2'
				=> 'DTD XHTML 1.0 Strict',
			'URI'
				=> 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd'),
			2 => array( // XHTML1.0 frameset
			'root'
				=> 'html',
			'systemID'
				=> 'PUBLIC',
			'FPI1'
				=> 'W3C',
			'FPI2'
				=> 'DTD XHTML 1.0 Frameset',
			'URI'
				=> 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd'),
			3 => array( // XHTML1.1
			'root'		// root element
				=> 'html',
			'systemID'	// PUBLIC or PRIVATE
				=> 'PUBLIC',
			'FPI1'		// if above is PUBLIC this is the first part of the DTD declaration
				=> 'W3C',
			'FPI2'		// FPI if PUBLIC  (Formal Public Identifier) in xhtml1.1 it can be 1
				=> 'DTD XHTML 1.1',
			'URI'		// dtd uri
				=> 'http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd'),
			4 => array( // PLIST
			'root'
				=> 'plist',
			'systemID'
				=> 'PUBLIC',
			'FPI1'
				=> 'Apple Computer',
			'FPI2'
				=> 'DTD PLIST 1.0',
			'URI'
				=> 'http://www.apple.com/DTDs/PropertyList-1.0.dtd')
		);
		$a_dt = $a_DTDs[$this->i_DTDflag];
		$s_return = sprintf('<!DOCTYPE %1$s %2$s',
			$a_dt['root'], $a_dt['systemID']);
		$s_return .= ('PUBLIC' == $a_dt['systemID'])
			? sprintf(' "-//%1$s//%2$s//%3$s" "%4$s">',
				$a_dt['FPI1'], $a_dt['FPI2'],
				strtoupper($this->s_lang), // I'm not sure this is correct, maybe we should have EN forced...
				$a_dt['URI'])
			: '>';
		unset($a_dt, $a_DTDs);
		return $s_return;
	}
	/* * *\ getDocType (CC)2007 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * string function getXMLdeclaration()
	 * *
	 * * returns xml declaration as a string ready for html
	 * *
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * returns xml declaration as a string ready for html
	 *
	 * @return string xml declaration as a string ready for html
	 * @uses SssS_XHTML::$s_XMLversion for output
	 * @uses SssS_XHTML::$s_charset for output
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function getXMLdeclaration() {
		return sprintf('<?xml version="%1$s" encoding="%2$s"?>'
				, $this->s_XMLversion, $this->s_charset);
	}
	/* * *\ getXMLdeclaration (CC)2007 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * boolean function isValidTagID(
	 * *	  i_tag = -1}		the tag id integer
	 * *
	 * * returns true if i_tag is in range of defined tags else false
	 * *
	 * * version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * true if $i_tag is in range of defined tags else false
	 *
	 * note: Although you may have orphans/alternative roots with negative id's. This practice is deprecated, rather instantiate another object
	 *
	 * note: this method doesn't actually check if the key is present or not
	 *
	 * @param int tag id to be checked
	 *
	 * @return bool true if $i_tag is in range of defined tags else false
	 *
	 * @uses SssS_XHTML::$a_tags to count entries
	 *
	 * @version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 */
	function isValidTagID($i_tag = -1) {
		return ((-1 < $i_tag) && ($i_tag < count($this->a_tags))) ? true : false;
	}
	/* * *\ isValidTagID(CC)2008 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * string function outputTag(
	 * *	  i_tag = -1		the tag id integer
	 * *	, i_level = 0		inset level to start with
	 * *	, s_spacer = "\t"	inset char(s) defaults to 1 tab
	 * *	, i_flagIdNameClass = 0) 1 = add ID if none present; 2 = add Name if none present; 4 = add class if none has been set
	 * *		you can add to set multiples: 3 = add id and name attributes; 7 = add all missing attributes
	 * *	note: br-tag is ignored by default. this behaviour is controlled by a_skipTheseFromFlagIdNameClass
	 * *
	 * * returns content of tag including children as xml
	 * * as a string ready for html
	 * *
	 * * TO DO: make it possible to position children in contents: DONE as of 0.2 only now TO DO: clean output
	 * * version 0.2 (CC)2008 Luke JZ aka SwissalpS (added i_flagIdNameClass functionality)
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * content of tag including children as xml string ready for html
	 *
	 * note: by default $i_flagIdNameClass ignores br-tag. Which tags are ignored is controlled by SssS_XHTML::$a_skipTheseFromFlagIdNameClass
	 *
	 * @param int $i_tag tag id of node to be outputed
	 * @param int $i_level indentation level (defaults to 0)
	 * @param string $s_spacer indentation string (defaults to 1 tab)
	 * @param int $i_flagIdNameClass (default 0, off) autofill attribute level: 1 = add ID if none present; 2 = add Name if none present; 4 = add class if none has been set. You can add to set multiples: 3 = add id and name attributes; 7 = add all missing attributes
	 *
	 * @return string
	 *
	 * @uses SssS_XHTML::isValidTagID() check validity of $i_tag
	 * @uses SssS_XHTML::getAttributes() for tags attributes
	 * @uses SssS_XHTML::outputTag() to loop tags children
	 * @uses SssS_XHTML::$a_tags reading
	 * @uses SssS_XHTML::$s_textNodeName to identify text nodes
	 * @uses SssS_XHTML::$a_skipTheseFromFlagIdNameClass to know which tags to ignore
	 * @uses SssS_XHTML::$a_newLineAfterOpenTags to know which tags have trailing new lines after opening tag
	 * @uses SssS_XHTML::$a_newLineAfterCloseTags to know which tags have trailing new lines after closing tag
	 *
	 * @todo make it possible to position children in contents: DONE as of 0.2 only now TO DO: clean output
	 *
	 * @version 0.2 (CC)2008 Luke JZ aka SwissalpS (added $i_flagIdNameClass functionality)
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function outputTag($i_tag = -1, $i_level = 0, $s_spacer = "\t", $i_flagIdNameClass = 0) {
		static $b_leadIn = false;
		$s_return = '';

		// filter invalid ids
		if ($this->isValidTagID($i_tag)) {
			$a_thisTag	= $this->a_tags[$i_tag];

			// filter text nodes
			if ($this->s_textNodeName == $a_thisTag['tag']) {
				$s_contents = $a_thisTag['contents'];
				$b_leadIn = false; //(chr(10) == mb_substr($a_thisTag['contents'], -1, 1, $this->s_charset)) ? true : false;
				return $s_contents;
			}

			$s_return  .= ($b_leadIn && ('a' != $a_thisTag['tag'])) ? chr(10) . str_repeat($s_spacer, $i_level) : '';	// lead in spacers
			$s_return  .= '<' . $a_thisTag['tag'];					// open tag
			$s_atts 	= $this->getAttributes($i_tag);				// get attributes
			$s_return  .= ('' != $s_atts) ? ' ' . $s_atts : '';		// attributes
			// add missing attributes if flags are set for automatic attribute completion
			if ((0 != $i_flagIdNameClass) && (!in_array($a_thisTag['tag'], $this->a_skipTheseFromFlagIdNameClass))) {
				// add missing attributes {id, name, class}
				if ((1 & $i_flagIdNameClass) && ((!isset($a_thisTag['attributes']['id'])) || ((isset($a_thisTag['attributes']['id']))) && ('' == $a_thisTag['attributes']['id']))) { // add id?
					$s_return .= ' id="auto' . $i_tag . '"';
				} // add id
				if ((2 & $i_flagIdNameClass) && (!isset($a_thisTag['attributes']['name']))) { // add name?
					$s_return .= ' name="auto' . $i_tag . '"';
				} // add name
				if ((4 & $i_flagIdNameClass) && (!isset($a_thisTag['attributes']['class']))) { // add class?
					$s_return .= ' class="auto' . $i_tag . '"';
				} // add class
			} // add missing js, css attributes

			// determine if tag is empty (MT)
			$b_MTtag = (0 < count($a_thisTag['children'])) ? false : true;
			/* 0.1 still had no text nodes:
			$b_MTtag	= (("" != $a_thisTag['contents'])
				|| (0 < count($a_thisTag['children']))) ? false : true; */
$b_leadIn = true;
$b_leadIn = (in_array($a_thisTag['tag'], $this->a_newLineAfterOpenTags));
			if ($b_MTtag) { // close eMpTy tag
				$s_return .= ' />';
			} else { // pause tag
				$s_return .= '>';

				/* 0.1 had no text nodes, now only text nodes have contents and we filtered those earlier
				// insert contents
				if ("" != $a_thisTag['contents']) $s_return .= $a_thisTag['contents'];
				else $s_return .= chr(10);*/

				// insert children
				foreach ($a_thisTag['children'] as $i_child) {
					$s_return .= $this->outputTag($i_child, $i_level + 1, $s_spacer, $i_flagIdNameClass);
				}

				// close tag
				if ($b_leadIn) $s_return .= chr(10) . str_repeat($s_spacer, $i_level);
				$s_return .= '</' . $a_thisTag['tag'] . '>';

				// add comment to signal end of block
				$s_return .= (isset($a_thisTag['attributes']['id'])
					&& ('' != $a_thisTag['attributes']['id']))
					? ' <!-- id: ' . $a_thisTag['attributes']['id']
							. ' END -->' . chr(10)
					: '';//chr(10);
			} // close tag
$b_leadIn = (in_array($a_thisTag['tag'], $this->a_newLineAfterCloseTags));
		} // valid tag
		return $s_return;
	}
	/* * *\ outputTag (CC)2007 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * void function resetDeclarations()
	 * *
	 * * resets docType and XMLdeclaration strings according
	 * * to the current settings i.e. language
	 * *
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * resets docType and XMLdeclaration strings according to the current settings i.e. language
	 *
	 * @uses SssS_XHTML::getDocType() to reset SssS_XHTML::$s_docType
	 * @uses SssS_XHTML::getXMLdeclaration() to reset SssS_XHTML::$s_XMLdeclaration
	 * @uses SssS_XHTML::$s_docType set to value returned from SssS_XHTML::getDocType()
	 * @uses SssS_XHTML::$s_XMLdeclaration set to value returned from SssS_XHTML::getXMLdeclaration()
	 *
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function resetDeclarations() {
		$this->s_docType = $this->getDocType();
		$this->s_XMLdeclaration = $this->getXMLdeclaration();
	}
	/* * *\ resetDeclarations (CC)2007 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * string function XHTML()
	 * *
	 * * returns tidy xhtml string ready for output to file or browser
	 * *
	 * * version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 * * */
	/**
	 * tidy xhtml string ready for output to file or browser
	 *
	 * complete with doctype and xml declarations
	 *
	 * @return string tidy xhtml string ready for output to file or browser
	 *
	 * @uses SssS_XHTML::outputTag() for output
	 * @uses SssS_XHTML::$i_DTDflag to determine if SssS_XHTML::$s_XMLdeclaration is required
	 * @uses SssS_XHTML::$s_XMLdeclaration for output if SssS_XHTML::$i_DTDflag == 4 -> Plist
	 * @uses SssS_XHTML::$s_docType for output
	 * @uses SssS_XHTML::$i_html as parameter for SssS_XHTML::outputTag()
	 *
	 * @version 0.1 (CC)2007 Luke JZ aka SwissalpS
	 */
	function XHTML() {
		$s_return = (4 == $this->i_DTDflag)
			? $this->s_XMLdeclaration . chr(10) : '';
		$s_return .= $this->s_docType . chr(10)
				. $this->outputTag($this->i_html);
		return $s_return;
	}
	/* * *\ XHTML (CC)2007 Luke JZ aka SwissalpS /* * */

	/* * *
	 * * string function XHTMLaa(
	 * *	i_flagIdNameClass = 0)
	 * *		1 = add 'id' attribute where none present
	 * *		2 = add 'name' attribute where none present
	 * *		3 = both above
	 * *		4 = add class if none has been set
	 * *		5 =  1 + 4
	 * *		6 = 2 + 4
	 * *		7 = 3 + 4 = 1 + 2 + 4
	 * *
	 * *	note: tags in a_skipTheseFromFlagIdNameClass are ignored
	 * *
	 * * returns tidy xhtml string ready for output to file or browser
	 * * with Automatic Attributes.
	 * *
	 * * version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 * * */
	/**
	 * tidy xhtml string ready for output to file or browser
	 * (with Automatic Attributes)
	 *
	 * complete with doctype and xml declarations
	 *
	 * @param int $i_flagIdNameClass (default 0, off) autofill attribute level: 1 = add ID if none present; 2 = add Name if none present; 4 = add class if none has been set. You can add to set multiples: 3 = add id and name attributes; 7 = add all missing attributes
	 *
	 * @return string tidy xhtml string ready for output to file or browser
	 *
	 * @uses SssS_XHTML::outputTag() for output
	 * @uses SssS_XHTML::$s_docType for output
	 * @uses SssS_XHTML::$s_XMLdeclaration for output
	 *
	 * @version 0.1 (CC)2008 Luke JZ aka SwissalpS
	 */
	function XHTMLaa($i_flagIdNameClass = 0) {
		$s_return = $this->s_docType . chr(10)
				. $this->s_XMLdeclaration . chr(10)
				. $this->outputTag($this->i_html, 0, "\t", $i_flagIdNameClass);
		return $s_return;
	}
	/* * *\ XHTML aa(CC)2008 Luke JZ aka SwissalpS /* * */

	// 20100503_123217 added __toString() for easy info
	function __toString() { return $this->XHTML(); } // __toString
	// for plist mergance
	function __toPlistArray() {
		return ($this->a_tags) ? $this->a_tags : array();
	} // __toPlistArray
}
/* * * *\ SssS_XHTML (CC)2007 Luke JZ aka SwissalpS /* * * */
?>
