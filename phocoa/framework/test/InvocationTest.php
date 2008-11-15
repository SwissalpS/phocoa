<?php

/*
 * @package test
 */
 

error_reporting(E_ALL);
require_once('/Users/alanpinstein/dev/sandbox/phocoadev/phocoadev/conf/webapp.conf');

class InvocationTest extends PHPUnit_Framework_TestCase
{
    // normal, allPages OFF
    function testInvocationNoModule()
    {
        $this->setExpectedException('WFRequestController_NotFoundException');
        WFModuleInvocation::quickModule('test/dne');
    }
    function testInvocationNoPage()
    {
        $this->setExpectedException('WFRequestController_NotFoundException');
        WFModuleInvocation::quickModule('test/invocation/dne');
    }
    function testInvocationModuleNameOnly()
    {
        $this->executeAndAssert('test/invocation', array('HTML::','display.tpl'));
    }
    function testInvocationModuleNameAndPage()
    {
        $this->executeAndAssert('test/invocation/display', array('HTML::','display.tpl'));
    }
    function testInvocationModuleNameAndAnotherPage()
    {
        $this->executeAndAssert('test/invocation/anotherPage', array('HTML::','anotherPage.tpl'));
    }
    function testInvocationModuleNameOnlyWith1Parameter()
    {
        // moduleNameOnly + parameters should 404 with allpages OFF
        $this->setExpectedException('WFRequestController_NotFoundException');
        WFModuleInvocation::quickModule('test/invocation/1');
    }
    function testInvocationWith1Parameter()
    {
        $this->executeAndAssert('test/invocation/display/1', array('HTML:1:','display.tpl'));
    }
    function testInvocationWith2Parameters()
    {
        $this->executeAndAssert('test/invocation/display/1/2', array('HTML:1:','display.tpl'));
    }
    function testInvocationAnotherPageWith1Parameter()
    {
        $this->executeAndAssert('test/invocation/anotherPage/1', array('HTML:1:','anotherPage.tpl'));
    }
    function testInvocationAnotherPageWith2Parameters()
    {
        $this->executeAndAssert('test/invocation/anotherPage/1/2', array('HTML:1:','anotherPage.tpl'));
    }
    function testInvocationModuleInModule()
    {
        $this->executeAndAssert('test/invocation/moduleInModule2', 'Module in module');
    }
    
    // normal, allPages ON
    function testInvocationAllPagesModuleNameOnly()
    {
        $this->executeAndAssert('test/invocationAllPages', array('HTML::','display.tpl'));
    }
    function testInvocationAllPagesModuleNameAndManifestedPage()
    {
        $this->executeAndAssert('test/invocationAllPages/display', array('HTML::','display.tpl'));
    }
    function testInvocationAllPagesModuleNameAndManifestedAnotherPage()
    {
        $this->executeAndAssert('test/invocationAllPages/anotherPage', array('HTML::','anotherPage.tpl'));
    }
    function testInvocationAllPagesNotManifestedPage()
    {
        // since the page is not manifested, it gets treated as defaultPage and then the "page" part gets treated as the 1st parameter to the defaultPage
        $this->executeAndAssert('test/invocationAllPages/pageNotManifested', array('HTML:pageNotManifested:','display.tpl'));
    }
    function testInvocationAllPagesModuleInModule()
    {
        $this->executeAndAssert('test/invocationAllPages/moduleInModule', 'Module in module');
    }

#   // REST + HTML
#   function testRestModuleNameOnly()
#   {
#       $this->executeAndAssert('test/restful', 'HTML::');
#   }
#   function testRestModuleAndPage()
#   {
#       $this->executeAndAssert('test/restful/display', 'HTML::');
#   }
#   function testRestModuleAndPageWith1Parameter()
#   {
#       $this->executeAndAssert('test/restful/display/1', 'HTML:1:');
#   }
#   function testRestModuleAndPageWith2Parameters()
#   {
#       $this->executeAndAssert('test/restful/display/1/2', 'HTML:1:');
#   }
#   function testRestModule404()
#   {
#       $this->setExpectedException('WFRequestController_NotFoundException');
#       WFModuleInvocation::quickModule('test/resfulDNE');
#   }
#   function testRestModulePage404()
#   {
#       $this->setExpectedException('WFRequestController_NotFoundException');
#       WFModuleInvocation::quickModule('test/resful/pageDNE');
#   }

#   // REST + XML
#   function testRestModuleNameOnlyXML()
#   {
#       $this->executeAndAssert('test/restful.xml', 'XML::');
#   }
#   function testRestModuleAndPageXML()
#   {
#       $this->executeAndAssert('test/restful/display.xml', 'XML::');
#   }
#   function testRestModuleAndPageWith1ParameterXML()
#   {
#       $this->executeAndAssert('test/restful/display/1.xml', 'XML:1:');
#   }
#   function testRestModuleAndPageWith2ParametersXML()
#   {
#       $this->executeAndAssert('test/restful/display/1/2.xml', 'XML:1:');
#   }

    private function executeAndAssert($invocationPath, $expectedResult)
    {
        if (!is_array($expectedResult)) $expectedResult = array($expectedResult);

        $result = NULL;
        try {
            $result = WFModuleInvocation::quickModule($invocationPath);
        } catch (Exception $e) {
            $this->fail('Invocation Path: "' . $invocationPath . '" threw an exception: ' . get_class($e) . ': ' . $e->getMessage());
        }
        foreach ($expectedResult as $r) {
            $this->assertContains($r, $result);
        }
    }
}
