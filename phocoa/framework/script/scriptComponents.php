<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */

function createModule($modName, $pageName) {

	$moduleTemplate = '<' . '?' . 'php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
' . _moduleBit($modName, $pageName);

	if ($pageName !== NULL)
		$moduleTemplate .= '
' . _pageForModule($modName, $pageName);

	$moduleTemplate .= '
?' . '>';

	// check and make dir
	if (!file_exists("./$modName")) mkdir('./' . $modName);

	$modFilePath = "{$modName}/{$modName}.php";
	if (!file_exists($modFilePath))	{
		print "Writing $modFilePath\n";
		file_put_contents($modFilePath, $moduleTemplate);
	} else {
		print "Skipping module $modName because it already exists.\n";
	}

	print "Done building module $modName!\n";

} // createModule


function _moduleBit($modName, $pageName) {

	 $moduleTemplate = '
class module_' . $modName . ' extends WFModule {

	function sharedInstancesDidLoad() {



	} // sharedInstancesDidLoad


	// this function should throw an exception if the user is not permitted to edit (add/edit/delete) in the current context
	function verifyEditingPermission($oPage) {

		// example
		// $oAauthInfo = WFAuthorizationManager::sharedAuthorizationManager()->authorizationInfo();

		// if ($oAauthInfo->userid() != $oPage->sharedOutlet(\'sharedEntityId\')->selection()->getUserId()) throw(new Exception(\'You don\'t have permission to edit\'));

	} // verifyEditingPermission
';

	if (NULL === $pageName) $pageName = 'defaultPage';

	$moduleTemplate .= "

	function defaultPage() { return '{$pageName}'; } // defaultPage
";

	$moduleTemplate .= '
} // module_' . $modName . '
';

	return $moduleTemplate;

} // _moduleBit


function _pageForModule($modName, $pageName) {

	return '

class module_' . $modName . '_' . $pageName . ' {

	public function parameterList() {

		return array();

	} // parameterList


	function parametersDidLoad($oPage, $aParams) {

		//$oPage->sharedOutlet(\'paginator\')->readPaginatorStateFromParams($aParams);

	} // parametersDidLoad


	function noAction($oPage, $aParams) {

		//$this->search($oPage, $aParams);

	} // noAction


	function setupSkin($oPage, $aParams, $oSkin) {

		//$oSkin->addHeadString(\'<link rel="stylesheet" type="text/css" href="\' . $oSkin->getSkinDirShared() . \'/form.css" />\');

		$oSkin->setTitle(\'' . $modName . ' : ' . $pageName . '\');

	} // setupSkin

} // module_' . $modName . '_' . $pageName . '
';

} // _pageForModule


function createPage($pageName) {

		$configFile = "---";

		$templateFile = "{* vim: set expandtab tabstop=4 shiftwidth=4 syntax=smarty: *}
Smarty HTML Goes Here.
";

	if (!file_exists($pageName . '.tpl')) {
		print "Writing {$pageName}.tpl\n";
		file_put_contents($pageName . '.tpl', $templateFile);
	} else {
		print "Skipping .tpl file because it already exists.\n";
	}

	if (!file_exists($pageName . '.yaml')) {
		print "Writing {$pageName}.yaml\n";
		file_put_contents($pageName . '.yaml', $configFile);
	} else {
		print "Skipping .yaml file because it already exists.\n";
	}

	print "Done!\n";
}
?>
