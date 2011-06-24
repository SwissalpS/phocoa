<?php

require_once(getenv('PHOCOA_PROJECT_CONF'));

if ($argc < 2) die("Usage: scaffold.php 'entity1 entity2 ...'\n");
$adapter = 'Propel';
$builder = 'WFModelCodeGen' . $adapter;
if (defined(ORM_MODEL_ADAPTER)) {

    $adapter = ORM_MODEL_ADAPTER;
    $builder = 'WFModelCodeGenPropel';

}
$configFile = APP_ROOT . '/propel-build/phocoa-generator-config.yaml';
if (!file_exists($configFile))
{
    $configFile = NULL;
}

$aEntities = array();
$aArgs = $argv;
array_shift($aArgs); // drop invocation

foreach ($aArgs as $sArgs) {
	$delim = ' ';
	if (false != strchr($sArgs, ',')) {
		$delim = ',';
	} // if got comma in arg

	$aEntities = array_merge($aEntities,
			array_map('trim', explode($delim, $sArgs)));

} // loop all args and collect into array

$model = WFModel::sharedModel();
$model->buildModel($adapter, $configFile, $aEntities);
print $model->__toString();
foreach ($model->entities() as $entity) {
    $codeGen = new $builder;
    try {
        $codeGen->generateModuleForEntity($entity);
    } catch (Exception $e) {
        print "Error generating scaffold for entity '{$entity}': " . $e->getMessage() . "\n";
    }
}
