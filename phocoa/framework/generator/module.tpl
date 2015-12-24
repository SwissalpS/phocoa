<?php

use Propel\Runtime\ActiveQuery\Criteria;

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

		$oC = new Criteria();

		$sQuery = $oPage->outlet('query')->value();

		if (!empty($sQuery)) {

			$sQuerySubStr = '%' . str_replace(' ', '%', trim($sQuery)) . '%';

			$oC->add({{$entityName}}Query::{{$descriptiveColumnConstantName}}, $sQuerySubStr, Criteria::LIKE); // for pgsql ILIKE

		} // if got query string

		$oPage->sharedOutlet('paginator')->setDataDelegate(new WFPagedPropelQuery($oC, '{{$entityName}}Query'));
		$oPage->sharedOutlet('{{$sharedEntityId}}')->setContent($oPage->sharedOutlet('paginator')->currentItems());

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

		if ($oPage->sharedOutlet('{{$sharedEntityId}}')->selection() === NULL) {

			if ($aParams['{{$sharedEntityPrimaryKeyProperty}}']) {
				$oPage->sharedOutlet('{{$sharedEntityId}}')->setContent(array({{$entityName}}Query::retrieveByPK($aParams['{{$sharedEntityPrimaryKeyProperty}}'])));

				$oPage->module()->verifyEditingPermission($oPage);

			} else {

				// prepare content for new
				$oPage->sharedOutlet('{{$sharedEntityId}}')->setContent(array(new {{$entityName}}()));

			} //

		} //

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

		if ($oPage->sharedOutlet('{{$sharedEntityId}}')->selection()->isNew()) {

			$title = SssSBla::cleanForTitle(WFLocalizedString('{{$entityName}}New'));
		}
		else {

			$title = SssSBla::cleanForTitle(WFLocalizedString('{{$entityName}}Edit')) . ':' . $oPage->sharedOutlet('{{$sharedEntityId}}')->selection()->valueForKeyPath('{{$descriptiveColumnName}}');

		}

		$oSkin->setTitle($title);

	} // setupSkin

} // module_{{$moduleName}}_edit



class module_{{$moduleName}}_confirmDelete {

	function parameterList() {

		return array('{{$sharedEntityPrimaryKeyProperty}}');

	} // parameterList


	function parametersDidLoad($oPage, $aParams) {

		// if we're a redirected action, then the {{$entityName}} object is already loaded. If there is no object loaded, try to load it from the object ID passed in the params.
		if ($oPage->sharedOutlet('{{$sharedEntityId}}')->selection() === NULL) {

			$objectToDelete = {{$entityName}}Query::retrieveByPK($aParams['{{$sharedEntityPrimaryKeyProperty}}']);

			if (!$objectToDelete)
				throw(new Exception("Could not load {{$entityName}} object to delete."));

			$oPage->sharedOutlet('{{$sharedEntityId}}')->setContent(array($objectToDelete));

		} // if not yet loaded

		if ($oPage->sharedOutlet('{{$sharedEntityId}}')->selection() === NULL)
			throw(new Exception("Could not load {{$entityName}} object to delete."));

	} // parametersDidLoad


	function cancel($oPage) {

		$oPage->module()->setupResponsePage('edit');

	} // cancel


	function deleteObj($oPage) {

		$oPage->module()->verifyEditingPermission($oPage);

		$myObj = $oPage->sharedOutlet('{{$sharedEntityId}}')->selection();

		$myObj->delete();

		$oPage->sharedOutlet('{{$sharedEntityId}}')->removeObject($myObj);

		$oPage->module()->setupResponsePage('deleteSuccess');

	} // deleteObj

} // class module_{{$moduleName}}_confirmDelete



class module_{{$moduleName}}_detail {

	function parameterList() {

		return array('{{$sharedEntityPrimaryKeyProperty}}');

	} // parameterList


	function parametersDidLoad($oPage, $aParams) {

		$oEntity = {{$entityName}}Query::retrieveByPK($aParams['{{$sharedEntityPrimaryKeyProperty}}']);

		$oPage->sharedOutlet('{{$sharedEntityId}}')->setContent(array($oEntity));

	} // parametersDidLoad

} // module_{{$moduleName}}_detail
