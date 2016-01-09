<?php

use Propel\Runtime\ActiveQuery\Criteria;
use SwissalpS\PHOCOA\Localization\Bla as SssSBla;
use SwissalpS\PHOCOA\Pagination\WFPagedPropelQuery;

// Created by PHOCOA WFModelCodeGen on {{php}}echo date('r');{{/php}}

class module_{{$moduleName}} extends WFModule {

	function defaultPage() { return 'list'; } // defaultPage


	// this function should throw an exception if the user is not permitted to edit (add/edit/delete) in the current context
	function verifyEditingPermission($oPage) {

		// example
		// $oAauthInfo = WFAuthorizationManager::sharedAuthorizationManager()->authorizationInfo();

		// if ($oAauthInfo->userid() != $oPage->sharedOutlet('{{$sharedEntityId}}')->selection()->getUserId()) throw(new Exception('You don't have permission to edit ' . {{$entityName}}));

	} // verifyEditingPermission

} // module_{{$moduleName}}



class module_{{$moduleName}}_list {

	function parameterList() {

		return array('paginatorState');

	} // parameterList


	function parametersDidLoad($oPage, $aParams) {

		$oPage->sharedOutlet('paginator')->readPaginatorStateFromParams($aParams);

	} // parametersDidLoad


	function noAction($oPage, $aParams) {

		$this->search($oPage, $aParams);

	} // noAction


	function search($oPage, $aParams) {

		$sQuery = trim($oPage->outlet('query')->value());

		$oC = {{$entityNameFull}}Query::create();

		if (strlen($sQuery)) {

			$sQuerySubStr = '%' . str_replace(' ', '%', $sQuery) . '%';

			$oC->filterBy{{$columnNameSearch}}($sQuerySubStr);

		} // if got query string

		$oPaginator = $oPage->sharedOutlet('paginator');
		$oSharedEntity = $oPage->sharedOutlet('{{$sharedEntityId}}');
		$oPagedQuery = new WFPagedPropelQuery($oC);

		$oPaginator->setDataDelegate($oPagedQuery);
		$oSharedEntity->setContent($oPaginator->currentItems());

	} // search


	function clear($oPage, $aParams) {

		$oPage->outlet('query')->setValue(NULL);

		$this->search($oPage, $aParams);

	} // clear


	function setupSkin($oPage, $aParams, $oSkin) {

		//$oSkin->addHeadString('<link rel="stylesheet" type="text/css" href="' . $oSkin->getSkinDirShared() . '/form.css" />');

		$oSkin->setTitle(SssSBla::cleanForTitle(WFLocalizedString('{{$entityName}}List')));

		//$oSkin->setTemplateType(WFSkin::SKIN_WRAPPER_TYPE_RAW);

	} // setupSkin

} // module_{{$moduleName}}_list



class module_{{$moduleName}}_edit {

	function parameterList() {

		return array('{{$sharedEntityPrimaryKeyProperty}}');

	} // parameterList


	function parametersDidLoad($oPage, $aParams) {

		$oSharedEntity = $oPage->sharedOutlet('{{$sharedEntityId}}');

		if (null === $oSharedEntity->selection()) {

			if (isset($aParams['{{$sharedEntityPrimaryKeyProperty}}'])) {

				$oC = {{$entityNameFull}}Query::create();

				$oEntity = $oC->findPk($aParams['{{$sharedEntityPrimaryKeyProperty}}']);

				$oSharedEntity->setContent(array($oEntity));

				$oPage->module()->verifyEditingPermission($oPage);

			} else {

				// prepare content for new
				$oSharedEntity->setContent(array(new {{$entityNameFull}}()));

			} // if edit or new

		} // if nothing selected

	} // parametersDidLoad


	function save($oPage) {

		try {

			$oPage->sharedOutlet('{{$sharedEntityId}}')->selection()->save();

			$oPage->outlet('statusMessage')->setValue("{{$entityName}} saved successfully.");

		} catch (Exception $e) {

			$oPage->addError(new WFError($e->getMessage()));

		} // try save catch

	} // save


	function deleteObj($oPage) {

		$oPage->module()->verifyEditingPermission($oPage);

		$oPage->module()->setupResponsePage('confirmDelete');

	} // deleteObj


	function setupSkin($oPage, $aParams, $oSkin) {

		//$oSkin->addHeadString('<link rel="stylesheet" type="text/css" href="' . $oSkin->getSkinDirShared() . '/form.css" />');

		$oSharedEntity = $oPage->sharedOutlet('{{$sharedEntityId}}');
		if ($oSharedEntity->selection()->isNew()) {

			$sTitle = SssSBla::cleanForTitle(WFLocalizedString('{{$entityName}}New'));

		} else {

			$sTitle = SssSBla::cleanForTitle(WFLocalizedString('{{$entityName}}Edit'))
					. ':' . $oSharedEntity->selection()->valueForKeyPath('{{$descriptiveColumnName}}');

		} // if is new or edit

		$oSkin->setTitle($sTitle);

	} // setupSkin

} // module_{{$moduleName}}_edit



class module_{{$moduleName}}_confirmDelete {

	function parameterList() {

		return array('{{$sharedEntityPrimaryKeyProperty}}');

	} // parameterList


	function parametersDidLoad($oPage, $aParams) {

		$oSharedEntity = $oPage->sharedOutlet('{{$sharedEntityId}}');

		// if we're a redirected action, then the {{$entityName}} object is already loaded. If there is no object loaded, try to load it from the object ID passed in the params.
		if (null === $oSharedEntity->selection()) {

			$oC = {{$entityNameFull}}Query::create();

			$objectToDelete = $oC->findPk($aParams['{{$sharedEntityPrimaryKeyProperty}}']);

			if (!$objectToDelete) {

				throw(new Exception("Could not load {{$entityName}} object to delete."));

			} // if nothing found

			$oSharedEntity->setContent(array($objectToDelete));

		} // if not yet loaded

		if (null === $oSharedEntity->selection()) {

			throw(new Exception("Could not load {{$entityName}} object to delete."));

		} // if still none loaded

	} // parametersDidLoad


	function cancel($oPage) {

		$oPage->module()->setupResponsePage('edit');

	} // cancel


	function deleteObj($oPage) {

		$oSharedEntity = $oPage->sharedOutlet('{{$sharedEntityId}}');

		$oPage->module()->verifyEditingPermission($oPage);

		$myObj = $oSharedEntity->selection();

		$myObj->delete();

		$oSharedEntity->removeObject($myObj);

		$oPage->module()->setupResponsePage('deleteSuccess');

	} // deleteObj

} // class module_{{$moduleName}}_confirmDelete



class module_{{$moduleName}}_detail {

	function parameterList() {

		return array('{{$sharedEntityPrimaryKeyProperty}}');

	} // parameterList


	function parametersDidLoad($oPage, $aParams) {

		$oC = {{$entityNameFull}}Query::create();

		$oEntity = $oC->findPk($aParams['{{$sharedEntityPrimaryKeyProperty}}']);

		$oPage->sharedOutlet('{{$sharedEntityId}}')->setContent(array($oEntity));

	} // parametersDidLoad

} // module_{{$moduleName}}_detail
