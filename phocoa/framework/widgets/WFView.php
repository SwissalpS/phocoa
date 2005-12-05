<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
/**
 * @package UI
 * @subpackage Views
 * @copyright Copyright (c) 2005 Alan Pinstein. All Rights Reserved.
 * @version $Id: kvcoding.php,v 1.3 2004/12/12 02:44:09 alanpinstein Exp $
 * @author Alan Pinstein <apinstein@mac.com>                        
 */

/**
 * The base "view" class. Views are the components that make up a WFPage. Each page has a root view, and then is made up of various subviews in
 * a view hierarchy. There is also a well-defined class hierarchy for WFView that contains all components that are used in creating a web page.
 *
 * <pre>WFView - Views are just generalized components for displaying on screen. NSView is abstract.
 *  |
 *  `-WFTabView - A tabbed view containing other pages. (NOT YET IMPLEMENTED)
 *  `-WFBoxView - A simple box containing content from another page. (NOT YET IMPLEMENTED)
 *  |
 *  `-WFWidget - A specialized view for dealing with displaying editale values or actionable controls.
 *      |                   Also abstract. Adds data get/set methods.
 *      |                   Add support for editable, error tracking, and formatters.
 *      `-WFForm - A Form object.
 *      `-WFTextField - A textfield item.
 *      `-WFCheckbox - A checkbox item.
 *      `-WFLabel - For displaying uneditable text, but can use bindings and formatters.
 *      (etc)</pre>
 *
 * Since we're a web framework, all views eventually know how to render themselves into HTML.
 * Views have no maintained state. Anything that needs to maintain state should be a {@link WFWidget} subclass.
 */
abstract class WFView extends WFObject
{
    /**
      * @var string The ID of the view. Used both internally {@link outlet} and externally as in the HTML id.
      */
    protected $id;
    /**
      * @var object WFView The parent of this view, or NULL if there is no parent.
      */
    protected $parent;
    /**
      * @var assoc_array Placeholder for additional HTML name/value pairs.
      */
    protected $children;
    /**
      * @var object The WFPage object that contains this view.
      */
    protected $page;

    /**
      * Constructor.
      *
      * Sets up the smarty object for this module.
      */
    function __construct($id, $page)
    {
        parent::__construct();

        if (is_null($id)) throw( new Exception("id required for new " . get_class($this) . '.') );
        if (!($page instanceof WFPage)) throw( new Exception("page must be a WFPage.") );

        $this->id = $id;
        $this->children = array();
        $this->parent = NULL;
        $this->page = $page;
        $this->setId($id);
    }

    /**
     *  Create a clone of the WFView with a new ID.
     *
     *  This will give you a copy of WFView that has been registered with the page.
     *
     *  @param 
     *  @return
     *  @throws
     */
    function cloneWithID($id)
    {
        $newView = clone($this);
        $newView->setId($id);
        return $newView;
    }

    /**
     *  Set the unique ID of this widget.
     *
     *  ID's are unique within a page; this function will tell the page of the new instance ID so it can be registered.
     *
     *  @param string The ID of the WFView instance.
     */
    function setId($id)
    {
        $this->id = $id;
        // add the widget to the page's widget list
        $this->page->addInstance($this->id, $this);
    }

    /**
      * Get the id of this view. All id's on a single page are unique.
      *
      * @return string The unique id of this view.
      */
    function id()
    {
        return $this->id;
    }
    
    /**
     *  Get a relative URL path to the public www dir for graphics for this widget.
     *
     *  @return string The URL to directory containing www items for this widget.
     */
    function getWidgetWWWDir()
    {
        return WWW_ROOT . '/www/framework/widgets/' . get_class($this);
    }
    
    /**
      * Get the {@link WFPage} object that this view belongs to.
      */
    function page()
    {
        return $this->page;
    }

    /**
     *  Set the parent view for this view.
     *
     *  @param object WFView The parent object.
     */
    function setParent($parent)
    {
        $this->parent = $parent;
    }

    /**
     *  Get the parent view for this view.
     *
     *  @return object WFView The parent object.
     */
    function parent()
    {
        return $this->parent;
    }

    /**
      * Add a child view to this view.
      *
      * @param object A WFView object to add.
      */
    function addChild(WFView $view)
    {
        $this->children[$view->id()] = $view;
        $view->setParent($this);
    }
    /**
      * Get all child views of this view.
      *
      * @return array An array of WFView objects.
      */
    function children()
    {
        return $this->children;
    }

    /**
      * Render the view into HTML.
      *
      * @param string $blockContent For block views (ie have open and close tags)r, the ready-to-use HTML content that goes inside this view.
      *                             For non-block views (ie single tag only) will always be null.
      * @return string The final HTML output for the view.
      */
    abstract function render($blockContent = NULL);
}

?>
