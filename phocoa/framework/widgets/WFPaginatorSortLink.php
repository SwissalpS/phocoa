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
 * Includes
 */
require_once('framework/widgets/WFView.php');

/**
 * A Paginator sort link widget. 
 *
 * Creates a link along the lines of:
 *
 * Price (+/-)
 *
 * Where +/- is a graphic indicating ascending or descending sort. The text will be BOLD if the data is currently sorted by this key.
 *
 * The WFPaginatorSortLink supports only single-key sorting. Multi-key userland sorting must be done with another method.
 *
 * <b>PHOCOA Builder Setup:</b>
 *
 * <b>Required:</b><br>
 * - {@link WFWidget::$value value} The value is the sort key that this item represents (setup without the +/-).
 * - {@link WFPaginatorPageInfo::$paginator Paginator}
 * 
 * <b>Optional:</b><br>
 * None.
 *
 * @todo Move the MODE stuff into WFPaginator as it's set only once per paginator, and multiple widgets need access to it.
 */
class WFPaginatorSortLink extends WFWidget
{
    /**
     * @var string The base URL of the link to use in MODE_URL.
     */
    private $baseURL;
    /**
     * @var object WFPaginator The paginator object that we will draw navigation for.
     */
    protected $paginator;

    /**
      * Constructor.
      */
    function __construct($id, $page)
    {
        parent::__construct($id, $page);
        $this->paginator = NULL;

        if ($this->page->module()->invocation()->targetRootModule() and !$this->page->module()->invocation()->isRootInvocation())
        {
            $this->baseURL = WWW_ROOT . '/' . $this->page->module()->invocation()->rootInvocation()->invocationPath();
        }
        else
        {
            $this->baseURL = WWW_ROOT . '/' . $this->page->module()->invocation()->modulePath() . '/' . $this->page->pageName();
        }
    }

    function render($blockContent = NULL)
    {
        if (!$this->paginator) throw( new Exception("No paginator assigned.") );

        $sortIndicator = NULL;
        $linkKey = "+{$this->value}";
        $sortKeys = $this->paginator->sortKeys();
        // if the paginator is currently using our sort key, or the reverse of our sort key, then we need to show a "toggle sort dir" link. Otherwise, just show the toggle ascending link.
        if (in_array("+{$this->value}", $sortKeys))
        {
            $sortIndicator = "+";
            $linkKey = "-{$this->value}";
        }
        else if (in_array("-{$this->value}", $sortKeys))
        {
            $sortIndicator = "-";
            $linkKey = "+{$this->value}";
        }

        $sortOptions = $this->paginator->sortOptions();

        if ($this->paginator->mode() == WFPaginator::MODE_URL)
        {
            $output = '<a href="' . $this->baseURL  . '/' . $this->paginator->paginatorState(NULL, NULL, array($linkKey)) . '">' . $sortOptions[$linkKey] . $sortIndicator . '</a>';
            if ($sortIndicator)
            {
                return "<b>$output</b>";
            }
            return $output;
        }
        else
        {
            $output = '<a href="#" onClick="' . $this->paginator->jsForState($this->paginator->paginatorState(NULL, NULL, array($linkKey))) . '">' . $sortOptions[$linkKey] . $sortIndicator . '</a>';
            if ($sortIndicator)
            {
                return "<b>$output</b>";
            }
            return $output;
        }
    }

    function canPushValueBinding() { return false; }
}

?>