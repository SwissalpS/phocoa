<?php
///////////////////////// ALPHA!!!! IN PROGRESS .!!!!!!!!!!!!!!!!!!
class WFModelBuilderPropel2 extends WFObject implements WFModelBuilder
{
    protected $builtEntities = array(); // prevent infinite loops!

    function setup() {
        if (is_readable(PROPEL2_RUNTIME_CONFIG)) {
            require_once(PROPEL2_RUNTIME_CONFIG);
        } else {
            throw new WFException('could not read Propel2 config file at ' . PROPEL2_RUNTIME_CONFIG);
        } // if got propel config at all
    } // setup

    /**
     * Get the propel metadata for the given entity.
     *
     * @param string The name of the entity, as it's PHP class name.
     * @return object TableMap The Propel TableMap for this entity.
     */
    function getEntityMetadata($sClassName) {
        // in Propel, the MapBuilder class is only set up for an entity when the Peer file is loaded...
        $sTableMapClassName = $sClassName::TABLE_MAP;

        $oTableMap = $sTableMapClassName::getTableMap();

        return $oTableMap;

    } // getEntityMetadata

    /**
     * Pass in a WFModelEntity object with a name filled out.
     *
     * @param object WFModelEntity An WFModelEntity with a name.
     * @throws object WFModelEntity
     */
    function buildEntityModel($oEntity) {
        if (!($oEntity instanceof WFModelEntity)) throw( new WFException("WFModelEntity required.") );
        $sName = $oEntity->valueForKey('name');

        if (isset($this->builtEntities[$sName])) return $this->builtEntities[$sName];

        // build a WFModelEntity structure from the Propel metadata....
        $oTableMap = $this->getEntityMetadata($sName);
        $aColumns = $oTableMap->getColumns();

        // set up properties
        foreach ($aColumns as $oColumn) {
            $oProperty = new WFModelEntityProperty;
            $sPropertyName = $oColumn->getPhpName();
            $sPropertyName[0] = strtolower($sPropertyName[0]);
            $oProperty->setValueForKey($sPropertyName, 'name');
            $oProperty->setValueForKey($oColumn->getDefaultValue(), 'defaultValue');
            // BOOLEAN|TINYINT|SMALLINT|INTEGER|BIGINT|DOUBLE|FLOAT|REAL|DECIMAL|CHAR|{VARCHAR}|LONGVARCHAR|DATE|TIME|TIMESTAMP|BLOB|CLOB
            switch (strtoupper($oColumn->getType())) {
                case 'TINYINT':
                case 'SMALLINT':
                case 'INTEGER':
                case 'BIGINT':
                case 'DOUBLE':
                case 'NUMERIC':
                case 'FLOAT':
                case 'REAL':
                case 'DECIMAL':
                    $sType = WFModelEntityProperty::TYPE_NUMBER;
                    break;
                case 'TIMESTAMP':
                case 'DATETIME':
                case 'DATE':
                    $sType = WFModelEntityProperty::TYPE_DATETIME;
                    break;
                case 'TEXT':
                case 'LONGVARCHAR':
                    $sType = WFModelEntityProperty::TYPE_TEXT;
                    break;
                case 'BOOLEAN':
                    $sType = WFModelEntityProperty::TYPE_BOOLEAN;
                    break;
                case 'CHAR':
                case 'VARCHAR':
                case 'STRING':
                    $sType = WFModelEntityProperty::TYPE_STRING;
                    break;
                default:
                    print "WARNING: Unknown property type for column "
                        . $oProperty->valueForKey('name')
                        . ": " . $oColumn->getType() . "\n";
                    $sType = WFModelEntityProperty::TYPE_STRING;
                    break;
            } // switch type
            if (!$oEntity->valueForKey('descriptiveColumnName')
                    && $sType === WFModelEntityProperty::TYPE_STRING) {
                $oEntity->setValueForKey(
                        $oProperty->valueForKey('name'),
                        'descriptiveColumnName');
            }
            if (!$oEntity->valueForKey('primaryKeyProperty')
                    && $oColumn->isPrimaryKey()) {
                $oEntity->setValueForKey(
                        $oProperty->valueForKey('name'),
                        'primaryKeyProperty');
            }
            $oProperty->setValueForKey($sType, 'type');
            $oEntity->addProperty($oProperty);
        } // loop all colums
        if (!$oEntity->valueForKey('descriptiveColumnName')) {
            $oEntity->setValueForKey(
                    $oEntity->valueForKey('primaryKeyProperty'),
                    'descriptiveColumnName');
        } // if no descriptive column name

        // set up relationships
        $oTableMap->getRelations();  // populate databaseMap with related columns
if ($aColumns != $oTableMap->getColumns()) {
var_dump(__METHOD__, 'got more columns than before');
}
        foreach ($aColumns as $oColumn) {
            if (!$oColumn->isForeignKey()) continue;

            //print "Processing {$tableMap->getPhpName()}.{$column->getPhpName()}\n";

            // get related entity
            $sRelatedEntityName = $oColumn->getRelatedTable()->getPhpName();
            $relatedEntityTableMap = $this->getEntityMetadata($sRelatedEntityName);
var_dump(get_class($relatedEntityTableMap), 'not yet coded relationship columns!!! edit ' . __FILE__ . '(' . __LINE__ . ')');
exit();
            $relatedEntity = WFModel::sharedModel()->getEntity($relatedEntityTableMap->getPhpName());
            if (!$relatedEntity)
            {
                //print "Building related WFModel entity {$relatedEntityTableMap->getPhpName()}\n";
                $relatedEntity = WFModel::sharedModel()->buildEntity($relatedEntityTableMap->getPhpName());
            }

            // configure relationship
            $relName = $relatedEntity->valueForKey('name');
            if (!$entity->getRelationship($relName))
            {
                // create relationship from this table to the other one
                $rel = new WFModelEntityRelationship;
                $rel->setToOne(true);   // if we are the fk column, it must be to-one (unless it's many-to-many)
                $rel->setValueForKey($relName, 'name'); // singular
                $rel->setValueForKey($column->isNotNull(), 'required');
                $entity->addRelationship($rel);
            }

            // create relationship in the other direction
            $invRelName = $tableMap->getPhpName();    // make plural as needed -
            if (!$relatedEntity->getRelationship($invRelName))
            {
                $invRel = new WFModelEntityRelationship;
                // configure relationship
                $inverseRelationshipIsToOne = false;
                // is this an "extension" table? TRUE if the relationship is on our PK; this makes the INVERSE relationship have an EXT relationship to this table
                if ($column->isPrimaryKey())
                {
                    $inverseRelationshipIsToOne = true;
                    $invRel->setValueForKey(true, 'isExtension');
                }
                $invRel->setToOne($inverseRelationshipIsToOne);
                $invRel->setValueForKey($invRelName, 'name');
                $relatedEntity->addRelationship($invRel);
            }
        } // loop all columns extracting relationships
        $this->builtEntities[$oEntity->valueForKey('name')] = $oEntity;
    } // buildEntityModel
} // WFModelBuilderPropel2
