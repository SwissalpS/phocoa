<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
/**
 * @package UI
 * @subpackage Template
 * @copyright Copyright (cc) 2009 Luke aka SwissalpS. No Rights Reserved.
 * @version 20091218_030114
 * @author Luke aka SwissalpS <Luke@SwissalpS.ws>
 */

/**
 *  Smarty plugin to include yui css files, local or from yahoo.
 *
 *  Smarty Params:
 *  hosted - 'local' -> localhosting; 'yahoo' -> yahoo.com hosting
 *
 *  @param array The params from smarty tag.
 *  @param object WFSmarty object of the current tpl.
 *  @return string The rendered HTML <link> tags.
 */
function smarty_function_WFSkinCSSYAHOOBase($params, &$smarty) {

    // validate input
    $sValue = (isset($params['hosted']) and $params['hosted'])
    		? strtolower($params['hosted'])
    		: 'local';

    // tell yui loader to set correct base path
    switch ($sValue) {
		case 'yahoo' :
    		WFYAHOO_yuiloader::sharedYuiLoader()->setBaseToYUIHosted();
    	break;

 		case 'local' :
    	default :
    		WFYAHOO_yuiloader::sharedYuiLoader()->setBaseToLocal();
    } // switch local or yahoo hosting

	// fetch the base path
	$sBase = WFYAHOO_yuiloader::sharedYuiLoader()->base();

	$sPrefix = '<link rel="stylesheet" type="text/css" href="';

	// return the two link tags
    return $sPrefix . $sBase . 'reset-fonts-grids/reset-fonts-grids.css" />
    ' . $sPrefix . $sBase . 'base/base-min.css" />';

} // smarty_function_WFSkinCSSYAHOOBase
?>
