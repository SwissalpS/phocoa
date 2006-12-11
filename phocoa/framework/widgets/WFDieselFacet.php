<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
/**
 * @package UI
 * @subpackage Widgets
 * @copyright Copyright (c) 2005 Alan Pinstein. All Rights Reserved.
 * @version $Id: kvcoding.php,v 1.3 2004/12/12 02:44:09 alanpinstein Exp $
 * @author Alan Pinstein <apinstein@mac.com>                        
 */

/**
 * A Dieselpoint Facet widget for our framework.
 * 
 * <b>PHOCOA Builder Setup:</b>
 *
 * <b>Required:</b><br>
 * - {@link WFDieselFacet::$attributeID attributeID}
 * - {@link WFDieselFacet::$dieselSearch dieselSearch}
 * 
 * <b>Optional:</b><br>
 * - {@link WFWidget::$formatter formatter} Any formatter assigned to the WFDieselFacet will be used to format the facet data and current selection data.
 * - {@link WFDieselFacet::$facetStyle facetStyle}
 * - {@link WFDieselFacet::$fakeOpenEndedRange fakeOpenEndedRange}
 * - {@link WFDieselFacet::$rangeCount rangeCount}
 * - {@link WFDieselFacet::$showItemCounts showItemCounts}
 * - {@link WFDieselFacet::$maxHits maxHits}
 * - {@link WFDieselFacet::$maxRows maxRows}
 * - {@link WFDieselFacet::$label label}
 * - {@link WFDieselFacet::$sortByFrequency sortByFrequency}
 * - {@link WFDieselFacet::$class class}
 *
 * @todo Implement TREEs.
 * @todo Implement multi-select facets: AND vs. OR.
 * @todo check out FacetGenerator.hasMore()... maybe good for long-ass lists. maybe switch from sortByFrequency to alpha once "more" clicked? use with popup edit
 * @todo Facets seem to be generating the link to themselves... is this a DP bug? Why would one want to show the current selection? Is it confusing?
 * @todo $facet used throughout our WFDieselFacet is actual an AttributeValue object. Confusing; need to re-name.
 * @todo treeRoot cookie trail linking isn't quite perfect yet...
 */
class WFDieselFacet extends WFWidget
{
    const STYLE_LIST = 'list';
    const STYLE_MENU = 'menu';
    const STYLE_TREE = 'tree';

    const UNLIMITED_ROWS = 1000;

    protected $isPopup;
    protected $popupSelections;
    
    /**
     * @var string The facet style. One of the WFDieselFacet::STYLE* constants.
     */
    protected $facetStyle;
    /**
     * @var string The root of the tree to start at. Only applies with STYLE_TREE. Default NULL (start at base).
     */
    protected $treeRoot;
    protected $defaultTreePosition;
    /**
     * @var string The Dieselpoint attribute id that this facet shows.
     */
    protected $attributeID;
    /**
     * @var object WFDieselSearch The WFDieselSearch object that this facet is linked to.
     */
    protected $dieselSearch;
    /**
     * @var int The number of ranges to show for the facet. Presently this will create N facets each containing approximately equal numbers of items. Default is 0, which disables range mode.
     */
    protected $rangeCount;
    /**
     * @var boolean True to implement fake open-ended ranges corrections. With DP < 4.0, there is no range supprort, so we fake it with multi-value attr categories. IE Bedrooms 1+, 2+, 3+. 
     *              The downside of this is that the facets don't adjust as choices are selected (b/c 3+ includes 1+ and 2+).
     *              The fakeOpenEndedRange support will correct the facet display by eliminating choices less than the current value.
     */
    protected $fakeOpenEndedRange;
    /**
     * @var boolean If true, the number of items in each facet will be shown as well. Default is TRUE. Performance will be faster if this is set to FALSE.
     */
    protected $showItemCounts;
    /**
     * @var int The maximum number of hits to count for each facet. Defaults to SHOW EXACT COUNT (Integer.MAX_VALUE). Set to a lower number if you don't care about more than a certain number, like 1000. Set to 1 for maxium performance. NOTE: setting showItemCounts to FALSE will automatically set maxHits to 1.
     */
    protected $maxHits;
    /**
     * @var int The maximum number of facets to show. Defaults to -1, which means SHOW ALL ROWS.
     */
    protected $maxRows;
    /**
     * @var string The label to show for the facet.
     */
    protected $label;
    /**
     * @var boolean If true, facets will be sorted by frequency of each facet. In this case, the facet with the most "hits" will be first, etc. If false, facets are sorted by the value they represent. Default is TRUE.
     */
    protected $sortByFrequency;
    /**
     * @var string The text to display for the "Show All" link. Default is "Show All".
     */
    protected $showAllText;
    /**
     * @var integer Number of chars after which the facet value string should be ellipsised. Default 0 (UNLIMITED).
     */
	protected $ellipsisAfterChars;
    /**
     * @var string For ajax callback of tree data... the path to the tree node whose kids need to be returned.
     */
    protected $treeDataPath;
    protected $isTaxonomyAttribute;
    protected $width;

    /**
      * Constructor.
      */
    function __construct($id, $page)
    {
        parent::__construct($id, $page);
        $this->attributeID = NULL;
        $this->rangeCount = 0;
        $Integer = new JavaClass('java.lang.Integer');
        $this->maxHits = $Integer->MAX_VALUE;
        $this->maxRows = WFDieselFacet::UNLIMITED_ROWS;            // unlimited by default
        $this->showItemCounts = true;
        $this->label = NULL;
        $this->sortByFrequency = true;
        $this->showAllText = 'Show All';
        $this->ellipsisAfterChars = 0;
        $this->fakeOpenEndedRange = false;
        $this->facetStyle = WFDieselFacet::STYLE_LIST;
        $this->treeRoot = NULL;
        $this->defaultTreePosition = NULL;
        $this->isTaxonomyAttribute = NULL;
        $this->isPopup = false;
        $this->popupSelections = array();
        $this->treeDataPath = null;
        $this->width = NULL;
    }

    public static function exposedProperties()
    {
        $items = parent::exposedProperties();
        return array_merge($items, array(
            'facetStyle' => array('list', 'menu', 'tree'),
            'treeRoot',
            'defaultTreePosition',
            'attributeID',
            'dieselSearch',
            'rangeCount',
            'fakeOpenEndedRange' => array('list', 'menu', 'tree'),
            'showItemCounts',
            'maxHits',
            'maxRows',
            'label',
            'sortByFrequency' => array('list', 'menu', 'tree'),
            'ellipsisAfterChars',
            'width',
            ));
    }

    function setTreeDataPath($path)
    {
        $this->treeDataPath = $path;
    }

    function setPopupSelections($selections)
    {
         $items = split(', ', urldecode($selections));
         foreach ($items as $item) {
             $this->popupSelections[$item] = 1;
         }
    }

    function setIsPopup($b)
    {
        $this->isPopup = $b;
        $this->maxRows = WFDieselFacet::UNLIMITED_ROWS;
        $this->sortByFrequency = false;
    }

    function setFacetStyle($style)
    {
        if (!in_array($style, array(WFDieselFacet::STYLE_LIST, WFDieselFacet::STYLE_TREE, WFDieselFacet::STYLE_MENU))) throw( new Exception("Invalid facetStyle: $style. Try passing one of the STYLE_* constants.") );
        $this->facetStyle = $style;
    }

    function facetStyle()
    {
        return $this->facetStyle;
    }

    function label()
    {
        return $this->label;
    }

    function attributeID()
    {
        return $this->attributeID;
    }

    function setAttributeId($id)
    {
        $this->attributeID = $id;
    }

    function setShowItemCounts($show)
    {
        $this->showItemCounts = $show;
        if ($this->showItemCounts === false)
        {
            $this->maxHits = 1; // makes facet generation faster
        }
    }

    function setDieselSearch($ds)
    {
        $this->dieselSearch = $ds;
    }

    function setRangeCount($rc)
    {
        $this->rangeCount = $rc;
    }

    /**
     *  Path, for taxonomy facets. Takes into account treeRoot.
     *
     *  @return string The current path.
     *  @throws object Exception If not a taxonomy facet.
     */
    function path()
    {
        if (!$this->isTaxonomyAttribute()) throw( new Exception("Path only meaningful for taxonomy facets.") );
        if ($this->treeDataPath)
        {
            if ($this->treeRoot)
            {
                return $this->treeRoot . "\t" . $this->treeDataPath;
            }
            else
            {
                return $this->treeDataPath;
            }
        }
        if ($this->treeRoot and strlen($this->treeRoot) > strlen($this->dieselSearch->getAttributeSelection($this->attributeID)))
        {
            return $this->treeRoot;
        }
        else
        {
            return $this->dieselSearch->getAttributeSelection($this->attributeID);
        }
    }

    /**
     *  Prepare the list of facets for the current widget.
     *
     *  @return array An array of facet objects (via the java bridge)
     *  @throws Excpetion if there are any errors.
     */
    function prepareFacets()
    {
        // load facet data
        $facetGenerator = $this->dieselSearch->getGeneratorObject();
        if ($this->rangeCount)
        {
            $facetGenerator->setRangeCount($this->rangeCount);
        }
        if ($this->isTaxonomyAttribute())
        {
            // determine "open branch"
            $cVal = $this->path();
            if ($cVal)
            {
                $openToBuf = new Java('com.dieselpoint.util.FastStringBuffer', $cVal);
            }
            else if ($this->defaultTreePosition)
            {
                $openToBuf = new Java('com.dieselpoint.util.FastStringBuffer', $this->defaultTreePosition);
            }
            else
            {
                $openToBuf = new Java('com.dieselpoint.util.FastStringBuffer', '');
            }
            // determine tree root
            if ($this->facetStyle == WFDieselFacet::STYLE_TREE)
            {
                if ($this->treeDataPath)
                {
                    $treeRootPath = ($this->treeRoot ? $this->treeRoot . "\t" . $this->treeDataPath : $this->treeDataPath);
                    $treeRootBuf = new Java('com.dieselpoint.util.FastStringBuffer', $treeRootPath);
                }
                else
                {
                    $treeRootBuf = new Java('com.dieselpoint.util.FastStringBuffer', ($this->treeRoot ? $this->treeRoot : ""));
                }
            }
            else
            {
                $treeRootBuf = new Java('com.dieselpoint.util.FastStringBuffer', $cVal ? $cVal : "");
            }
            $facets = $facetGenerator->getTaxonomyTree($this->attributeID, $openToBuf, $treeRootBuf, $this->maxHits);
            if (count($facets) == 1 and $facets[0]->getAttributeValue()->equals('')) // needed to extract facets from trees
            {
                $facets = $facets[0]->getChildren();
            }
            if (!$facets)
            {
                $facets = array();
            }
            //print "OTB: $openToBuf, TRB: $treeRootBuf<BR>";
            //if ($this->attributeID == 'location') $this->printAttributeValue($facets);
//                    //print "Tree has " . $tree->length() . " items.<BR>\n";
//                    if (count($tree) == 1)
//                    {
//                        //print "Tree has 1 item: p/v/c: " . $tree[0]->getPath() . " / " . $tree[0]->getAttributeValue() . ' / ' .  count($tree[0]->getChildren()) . "<BR>\n";
//                        if ($tree[0]->getAttributeValue()->equals(""))
//                        {
//                            $facets = $tree[0]->getChildren();
//                            if (!$facets) $facets = array();
//                        }
//                        else
//                        {
//                            $facets = $tree;
//                        }
//                    }
//                    else if (count($tree) == 0)
//                    {
//                        print "Tree has 0 items: ";
//                        $facets = array();
//                    }
//                    else
//                    {
//                        throw( new Exception("Tree has more than one item at root... what does this mean?") );
//                    }
        }
        else
        {
            $facets = $facetGenerator->getList($this->attributeID, $this->maxRows, $this->sortByFrequency, $this->maxHits);
        }
        return $facets;
    }

    function render($blockContent = NULL)
    {
        if ($this->hidden or $this->dieselSearch->isFilteringOnAttribute($this->attributeID))
        {
            return NULL;
        }
        else
        {
            // set up stuff we'll use while rendering
            if ($this->class)
            {
                $classHTML = " class=\"{$this->class}\" ";
            }

            // output facet nav
            try {
                $Array = new JavaClass("java.lang.reflect.Array");

                $facets = $this->prepareFacets();
                if (gettype($facets) == 'array')
                {
                    // need to fall through if facets is a php array (this is what we get if there are no kids)
                    // maybe need to return null if not in "send child data" mode?
                }
                else if ($Array->getLength($facets) == 0)
                {
                    return NULL;
                }

                // sanity check
                if ($this->facetStyle == WFDieselFacet::STYLE_MENU and $this->isTaxonomyAttribute()) throw( new Exception("STYLE_MENU does not support taxonomy attributes.") );
                if ($this->facetStyle == WFDieselFacet::STYLE_MENU and $this->rangeCount) throw( new Exception("STYLE_MENU does not support range display.") );

                // output!
                $html = '';
                if ($this->label)
                {
                    $html .= "<b>{$this->label}</b><br />\n";
                }
                $width = ($this->width ? "width: {$this->width};" : NULL );
                $html .= '<div style="height: ' . ($this->isPopup ? '300px' : $this->parent()->facetNavHeight()) . '; overflow: auto; ' . $width . '">' . "\n";
                    
                // actual facets
                switch ($this->facetStyle) {
                    case WFDieselFacet::STYLE_MENU:
                        $html .= $this->facetMenuHTML($facets);
                        break;
                    case WFDieselFacet::STYLE_TREE:
                        // set up "root" YUITree
                        $tree = new WFYAHOO_widget_TreeView('tree_' . $this->id(), $this->page);
                        $items = array();
                        foreach ($facets as $facet) {
                            $label = str_replace("\n", '', $this->facetHTML($facet));
                            $item = new WFYAHOO_widget_TreeViewNode( (string) $facet->getAttributeValue(), $label);
                            $items[] = $item;
                        }
                        $tree->setValue($items);
                        $tree->setDynamicCallback($this->parent()->baseURL() . '/' . $this->dieselSearch->getQueryState($this->attributeID()) . '//' . $this->id() . '|');
                        if ($this->treeDataPath)
                        {
                            // ajax callback -- send data
                            WFYAHOO_widget_TreeView::sendTree($items);
                        }
                        else
                        {
                            // initial display of first row of items
                            $html .= $tree->render();
                        }
                        break;
                    default:
                        foreach ($facets as $facet) {
                            $html .= $this->facetHTML($facet);
                        }
                        if ($this->maxRows != WFDieselFacet::UNLIMITED_ROWS and $Array->getLength($facets) == $this->maxRows)
                        {
                            $html .= $this->editFacetLink('More...', $this->class);
                        }
                        else if ($this->maxRows == WFDieselFacet::UNLIMITED_ROWS and $Array->getLength($facets) == $this->maxRows)
                        {
                            $html .= "<p>There are too many choices to dislpay at this time. Please narrow your search by other criteria and try again.</p>";
                        }
                        break;
                }

                $html .= '</div>';
                return $html;
            } catch (JavaException $e) {
                $trace = new java("java.io.ByteArrayOutputStream");
                $e->printStackTrace(new java("java.io.PrintStream", $trace));
                throw( new Exception("java stack trace:<pre> $trace </pre>\n") );
            }
        }
    }

    function facetSelectionHTML()
    {
        $html = NULL;
        // SHOW CURRENT SELECTION
        if ($this->dieselSearch->isFilteringOnAttribute($this->attributeID))
        {
            if ($this->isTaxonomyAttribute())
            {
                // COOKIE TRAIL
                $cPath = split("\t", $this->dieselSearch->getAttributeSelection($this->attributeID));
                $cPathCount = count($cPath);
                if ($cPathCount > 0)
                {
                    $needsSeparator = false;
                    for ($i = 0; $i < $cPathCount; $i++) {
                        $upParts = array_slice($cPath, 0, $i + 1);
                        $upPath = join("\t", $upParts);
                        $upLabel = $cPath[$i];
                        if ($needsSeparator)
                        {
                            $html .= " &gt; ";
                        }
                        $html .= $upLabel;

                        $needsSeparator = true; // need sep always after 1st run
                    }
                    $html .= "\n";
                }
            }
            else
            {
                $html .= $this->dieselSearch->getAttributeSelection($this->attributeID, $this->formatter);
            }
        }
        return $html;
    }
    
    function removeFacetLink($linkText = "Remove")
    {
        $showLoadingJS = NULL;
        if ($this->parent()->showLoadingMessage())
        {
            $showLoadingJS = " onClick=\"showLoading();\" ";
        }
        return "<a {$showLoadingJS} href=\"" . $this->parent()->baseURL() . '/' . $this->dieselSearch->getQueryState($this->attributeID()) . "\">{$linkText}</a>";
    }

    function editFacetLink($linkText = "Edit", $class = NULL)
    {
        if ($class)
        {
            $class = " class=\"{$class}\" ";
        }
        // _nogo is to prevent browser from "scrolling" to this link; _nogo isn't a valid id.
        // return false on the onClick to prevent the js action from adding to the browser history
        return "<a href=\"#{$this->id}_nogo\" {$class} onClick=\"doPopup('" . $this->id() . "', '" . $this->dieselSearch->getQueryState($this->attributeID()) . "', '" . addslashes($this->dieselSearch->getAttributeSelection($this->attributeID())) . "'); return false;\">{$linkText}</a>";
    }

    private function facetMenuHTML($facets)
    {
        $baseLink = $this->parent()->baseURL() . '/' . $this->dieselSearch->getQueryState($this->attributeID);
        $showLoadingJS = NULL;
        if ($this->parent()->showLoadingMessage())
        {
            if ($this->isPopup)
            {
                $showLoadingJS .= "cancelPopup();\nshowLoading();";
            }
            else
            {
                $showLoadingJS = "showLoading();";
            }
        }
        $html = '
            <script language="JavaScript">
            <!--
            function __phocoaWFDieselSearchMenuSelected_' . $this->attributeID . '(select)
            {
                var index;
                var initialSelection = \'' . $this->value . '\';

                for(index=0; index<select.options.length; index++)
                    if(select.options[index].selected)
                    {
                        if(select.options[index].value != initialSelection)
                        {
                            newURL = "' . $baseLink . '|EQ_' . $this->attributeID . '=" + select.options[index].value; 
                            ' . $showLoadingJS . '
                            window.location.href = newURL;
                        }
                        break;
                    }
            }
            -->
            </script>
            ';
        $html .= "<select id=\"{$this->id}\" name=\"{$this->id}\" onChange=\"__phocoaWFDieselSearchMenuSelected_{$this->attributeID}(this);\" >\n";
        // add "show all" choice
        $html .= "<option value=\"\">{$this->showAllText}</option>\n";

        // selected value?
        $selection = $this->dieselSearch->getAttributeSelection($this->attributeID);
        foreach ($facets as $facet) {
            $attributeValue = $facet->getAttributeValue();
            if ($selection == $attributeValue)
            {
                $selected = 'selected';
            }
            else
            {
                $selected = NULL;
            }
            $label = '';
            if ($this->formatter)
            {
                $label .= $this->formatter->stringForValue($attributeValue);
            }
            else
            {
                $label .= $attributeValue;
            }
            if ($this->showItemCounts)
            {
                $label .= ' (' . $facet->getHits() . ')';
            }

            $html .= "<option value=\"{$attributeValue}\" {$selected}>{$label}</option>\n";
        }
        $html .= "</select>\n";
        return $html;
    }

    function restoreState()
    {
        //  must call super
        //parent::restoreState();   // no! because we need to add our params after _PageDidLoad and if we call super then we won't be called again

        if (isset($_REQUEST[$this->name]) and !empty($_REQUEST[$this->name]))
        {
            // kill existing selection
            $this->dieselSearch->clearAttributeQueries($this->attributeID);

            // set current query
            if (is_array($_REQUEST[$this->name]))
            {
                foreach ($_REQUEST[$this->name] as $value) {
                    $this->dieselSearch->addAttributeQuery($this->attributeID, 'EQ', $value);
                }
            }
            else
            {
                $this->dieselSearch->addAttributeQuery($this->attributeID, 'EQ', $_REQUEST[$this->name]);
            }
        }
    }

    function popupAttributeValueIsSelected($attributeValue)
    {
        //print "Checking '$attributeValue' in: <pre>" . print_r($this->popupSelections, true) . "</pre>";
        if (isset($this->popupSelections[$attributeValue]))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    private function facetHTML($facet)
    {
        // setup
        $classHTML = '';
        if ($this->class)
        {
            $classHTML = " class=\"{$this->class}\" ";
        }
        $html = '';

        $attributeValue = $facet->getAttributeValue();

        // support for fake open-ended ranges with mutli-value hack
        if ($this->fakeOpenEndedRange)
        {
            $currentVal = $this->dieselSearch->getAttributeSelection($this->attributeID);
            if ($attributeValue < $currentVal) return NULL;
        }

        $newAttrQueries = array();
        if ($this->rangeCount)
        {
            if ($facet->getEndValue())
            {
                $newAttrQueries[] = "GE_{$this->attributeID}=" . $attributeValue;
                $newAttrQueries[] = "LE_{$this->attributeID}=" . $facet->getEndValue();
            }
            else
            {
                $newAttrQueries[] = "EQ_{$this->attributeID}=" . $attributeValue;
            }
        }
        else
        {
            if ($this->isTaxonomyAttribute())
            {
                $newAttrQueries = array("EQ_{$this->attributeID}=" . $facet->getPath());
            }
            else
            {
                $newAttrQueries = array("EQ_{$this->attributeID}=" . $attributeValue);
            }
        }

        $label = '';
        if ($this->formatter)
        {
            $label .= $this->formatter->stringForValue($attributeValue);
        }
        else
        {
            $label .= $attributeValue;
        }
        if ($this->rangeCount and $facet->getEndValue())
        {
            $label .= ' - ';
            if ($this->formatter)
            {
                $label .= $this->formatter->stringForValue($facet->getEndValue());
            }
            else
            {
                $label .= $facet->getEndValue();
            }
        }

        $fullLabelAsTooltip = '';
        if ( $this->ellipsisAfterChars and (strlen($label) > $this->ellipsisAfterChars) )
        {
            $fullLabelAsTooltip = ' title="' . $label . '"';
            $label = substr($label, 0, $this->ellipsisAfterChars) . '...';
        }

        $showLoadingJS = NULL;
        if ($this->parent()->showLoadingMessage())
        {
            if ($this->isPopup)
            {
                $showLoadingJS = " onClick=\"cancelPopup(); showLoading();\" ";
            }
            else
            {
                $showLoadingJS = " onClick=\"showLoading();\" ";
            }
        }
        if ($this->isPopup and !($this->facetStyle == WFDieselFacet::STYLE_TREE) and !$this->fakeOpenEndedRange)
        {
            $selected = $this->popupAttributeValueIsSelected((string) $attributeValue);
            $html .= "<span {$classHTML}><input type=\"checkbox\" name=\"{$this->name}[]\" value=\"{$attributeValue}\" id=\"{$this->id}_{$attributeValue}\" " . ($selected == true ? 'checked="checked"' : '') . "/><label for=\"{$this->id}_{$attributeValue}\">{$label}</label>";
            if ($this->showItemCounts)
            {
                $html .= ' (' . $facet->getHits() . ')';
            }
            $html .= "</span><br />\n";
        }
        else
        {
            $link = $this->parent()->baseURL() . '/' . $this->dieselSearch->getQueryState($this->attributeID, $newAttrQueries);
            $html .= "<span {$classHTML}><a {$showLoadingJS} href=\"{$link}\"$fullLabelAsTooltip>{$label}</a>";
            if ($this->showItemCounts)
            {
                $html .= ' (' . $facet->getHits() . ')';
            }
            $html .= "</span><br />\n";
        }
//        if ($this->facetStyle == WFDieselFacet::STYLE_TREE)
//        {
//            //$this->printAttributeValue(array($facet));
//            $children = $facet->getChildren();
//            if ($children)
//            {
//                foreach ($children as $childFacet) {
//                    $html .= $this->facetHTML($childFacet, $facetDepth + 1);
//                }
//            }
//        }
        return $html;
    }

    private function isTaxonomyAttribute()
    {
        if (!is_null($this->isTaxonomyAttribute)) return $this->isTaxonomyAttribute;

        $row = $this->dieselSearch->index()->getAttribute()->getRowByAttribute_id($this->attributeID);
        if (!$row) throw( new Exception("Couldn't find attribute: {$this->attributeID}") );
        $Attribute = new JavaClass('com.dieselpoint.search.Attribute');
        $this->isTaxonomyAttribute = ($row->getDataType()->equals($Attribute->DATATYPE_TAXONOMY));

        return $this->isTaxonomyAttribute;
    }

    function printAttributeValue($avArray, $indent = 0)
    {
        if ($indent == 0) print "FacetTree:<br />";
        if ($avArray == NULL) return;

        foreach ($avArray as $av) {
            $children = $av->getChildren();
            print "Attribute '" . $av->getAttributeValue() . "', path '" . $av->getPath() . "', has " . count($children) . " children.<BR>\n";
            if (count($children) > 0)
            {
                $this->printAttributeValue($children, $indent + 1);
            }
        }
    }

    function canPushValueBinding() { return false; }
}

?>
