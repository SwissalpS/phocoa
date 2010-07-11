<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */

function createModule($modName, $pageName)
{

    $moduleTemplate = "<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */

class module_{$modName} extends WFModule
{
    function sharedInstancesDidLoad()
    {
    }
";

    if ($pageName === NULL)
    {
        $moduleTemplate .= "
    function defaultPage()
    {
        return 'defaultPage';
    }
";
    }
    else
    {
        $moduleTemplate .= "
    function defaultPage()
    {
        return '{$pageName}';
    }
";
    }
    $moduleTemplate .= "
}
";
    
    if ($pageName !== NULL)
    {
        $moduleTemplate .= "

class module_{$modName}_{$pageName}
{
    public function parameterList()
    {
        return array();
    }
}
";
    }
    $moduleTemplate .= "
?>";

	// 20100510_023949 SwissalpS added checking in which dir we are but decided to not use scripts/phocoa phing thingy for this simple stuff
	/*
	$sCurrDir = getcwd();
	if ('modules' == basename($sCurrDir)) {
		// good
	} else {
		print 'not in modules directory, searching upward' . chr(10);
		$sPossibleModuleDir = $sCurr . DIRECTORY_SEPARATOR . 'modules';
		if (isdir($sPossibleModuleDir)) {
			print 'changing to a possible dir: ' . $sPossibleModuleDir . chr(10);
			chdir($sPossibleModuleDir);

		} else {
			print 'not upward, well maybe closer to root' . chr(10) . 'checking if APP_ROOT is defined' . chr(10);
			if (defined('APP_ROOT')) {
				$sPossibleModuleDir = APP_ROOT . DIRECTORY_SEPARATOR . 'modules';
				if (isdir($sPossibleModuleDir)) {
					if (false === strstr($sCurrDir, APP_ROOT)) {
						print 'APP_ROOT is defined but we are *not* in its subtree. Bailing.' . chr(10); // TODO: ask if ok or ask for path...but we don't know... cli/cgi/eval.. call
						return;

					} else {
						print 'APP_ROOT is defined and we are in its subtree -> changing to dir: ' . $sPossibleModuleDir . chr(10);
						chdir($sPossibleModuleDir);

					} // if in subtree of APP_ROOT

				} else {
					print 'APP_ROOT is defined but no modules dir. Bailing.' . chr(10);
					return;

				} // if modules dir exists in APP_ROOT

			} else {
				// APP_ROOT is not defined -> search toward root
				print 'APP_ROOT is *not* defined';
				$mPos = strstr($sCurrDir, DIRECTORY_SEPARATOR . 'modules' . DIRECTORY_SEPARATOR);
				if (false === $mPos) {
					print ' and we are not in a subdir of /modules/. Bailing' . chr(10);
					return;

				} else {
				} // if 'modules' in path somewhere

			} // if got APP_ROOT -> ##PHOCOA_APP_DIR##

		} // if upwards
	} // if ok or need to change
	*/
    // check and make dir
    if (!file_exists("./$modName"))
    {
        mkdir('./' . $modName);
    }
    $modFilePath = "{$modName}/{$modName}.php";
    if (!file_exists($modFilePath))
    {
        print "Writing $modFilePath\n";
        file_put_contents($modFilePath, $moduleTemplate);
    }
    else
    {
        print "Skipping module $modName because it already exists.\n";
    }

    print "Done building module $modName!\n";
}

function createPage($pageName)
{
    $includeHints = false;
    if ($includeHints)
    {
        $configFile = "<?php
        /* vim: set expandtab tabstop=4 shiftwidth=4 syntax=php: */
        \$__config = array(
                    'instance_or_controller_id' => array(
                            'properties' => array(
                                'propName' => 'propValue'
                                ),
                            'bindings' => array(
                                'propName' => array(
                                    'instanceID' => 'controller instance id',       // or can be '#module#' to bind to the current module
                                    'controllerKey' => 'the controller key to use',
                                    'modelKeyPath' => 'the modelKeyPath to use',
                                    'options' => array(
                                        'bindingOption1' => 'bindingOptionValue'
                                        )
                                    )
                                )
                        )
                );

        ?>";

        $instancesFile = "<?php
        /* vim: set expandtab tabstop=4 shiftwidth=4 syntax=php: */

        \$__instances = array(
                'widgetID' => array(
                    'class' => 'WFWidget subclass',
                    'children' => array()
                    ),
                'controllerID' => array('class' => 'WFObjectController subclass')
                );

        ?>";

        $templateFile = "
{* vim: set expandtab tabstop=4 shiftwidth=4 syntax=smarty: *}
HTML Goes Here.
";
    }
    else
    {
        $configFile = "";

        $templateFile = "{* vim: set expandtab tabstop=4 shiftwidth=4 syntax=smarty: *}
HTML Goes Here.
";
    }

    if (!file_exists($pageName . '.tpl'))
    {
        print "Writing {$pageName}.tpl\n";
        file_put_contents($pageName . '.tpl', $templateFile);
    }
    else
    {
        print "Skipping .tpl file because it already exists.\n";
    }

    if (!file_exists($pageName . '.yaml'))
    {
        print "Writing {$pageName}.yaml\n";
        file_put_contents($pageName . '.yaml', $configFile);
    }
    else
    {
        print "Skipping .yaml file because it already exists.\n";
    }

    print "Done!\n";
}
?>
