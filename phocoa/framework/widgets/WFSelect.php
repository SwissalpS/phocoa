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
 * A Select widget for our framework.
 *
 * Used to select either a single, or multiple, values.
 *
 * <b>PHOCOA Builder Setup:</b>
 *
 * <b>Required:</b><br>
 * - {@link WFWidget::$value value} or {@link WFSelect::$values values}, depending on {@link WFSelect::$multiple multiple}.
 * 
 * <b>Optional:</b><br>
 * - {@link WFSelect::$multiple multiple}
 * - {@link WFSelect::$contentValues contentValues}
 * - {@link WFSelect::$contentLabels contentLabels}
 * - {@link WFSelect::$labelFormatter labelFormatter}
 * - {@link WFSelect::setOptions() options}
 * - {@link WFSelect::$visibleItems visibleItems}
 */
class WFSelect extends WFWidget
{
    /**
     * @var array The selected values of the select list, IF multiple is enabled.
     */
    protected $values;
    /**
     * @var boolean Allow multiple selection?
     */
    protected $multiple;
    /**
     * @var array The items to allow user to select. These are the "values" that are used if the user selects the item. There should be an equal number of contentValues and contentLabels.
     */
    protected $contentValues;
    /**
     * @var array The items to allow user to select. These are the "labels" that are used if the user selects the item. There should be an equal number of contentValues and contentLabels.
     */
    protected $contentLabels;
    /**
     * @var integer The number of items to show, if MULTIPLE is enabled. Default is 5.
     */
    protected $visibleItems;
    /**
     * @var string CSS width data for the popup. Default is EMPTY. Useful to constrain width of the popup menu. Ex: 80px will yield width: 80px;.
     */
    protected $width;
    /**
     * @var object WFFormatter labelFormatter A formatter for the "label" portion of the data.
     */
    protected $labelFormatter;

    /**
      * Constructor.
      */
    function __construct($id, $page)
    {
        parent::__construct($id, $page);
        $this->values = array();
        $this->multiple = false;
        $this->visibleItems = 5;
        $this->contentValues = array();
        $this->contentLabels = array();
        $this->width = NULL;
        $this->labelFormatter = NULL;
    }

    function setupExposedBindings()
    {
        $myBindings = parent::setupExposedBindings();
        $myBindings[] = new WFBindingSetup('value', 'The selected value for non-multiple select boxes.');
        $myBindings[] = new WFBindingSetup('values', 'The selected values for multiple select boxes.');
        $myBindings[] = new WFBindingSetup('contentValues', 'List of the VALUES of each item in the select box.',
                array(
                    'InsertsNullPlaceholder' => false,
                    'NullPlaceholder' => ''
                    )
                );
        $myBindings[] = new WFBindingSetup('contentLabels', 'List of the LABELS of each item in the select box.',
                array(
                    'InsertsNullPlaceholder' => false,
                    'NullPlaceholder' => 'Select...'
                    )
                );
        return $myBindings;
    }

    function processBindingOptions($boundProperty, $options, &$boundValue)
    {
        parent::processBindingOptions($boundProperty, $options, $boundValue);

        switch ($boundProperty) {
            case 'contentValues':
                if ($options["InsertsNullPlaceholder"]) {
                    $defaultValue = $options["NullPlaceholder"];
                    $boundValue = array_merge(array($defaultValue), $boundValue);
                }
                break;
            case 'contentLabels':
                if ($options["InsertsNullPlaceholder"]) {
                    $defaultLabel = $options["NullPlaceholder"];
                    $boundValue = array_merge(array($defaultLabel), $boundValue);
                }
                break;
        }
    }

    function setLabelFormatter($f)
    {
        if (!($f instanceof WFFormatter)) throw( new Exception("labelFormatter must be a WFFormatter subclass.") );
        $this->labelFormatter = $f;
    }
    function labelFormatter()
    {
        return $this->labelFormatter;
    }

    function setFormatter($f)
    {
        throw( new Exception("Formatters are not supported on WFSelect at this time. Are you looking for labelFormatter?") );
    }

    function setVisibleItems($numItems)
    {
        $this->assertMultiple(true);
        $this->visibleItems = $numItems;
    }
    function visibleItems()
    {
        return $this->visibleItems;
    }

    function multiple()
    {
        return $this->multiple;
    }

    function setMultiple($multiple)
    {
        if (!is_bool($multiple)) throw( new Exception("multiple must be boolean.") );
        $this->multiple = $multiple;
    }

    function setValue($val)
    {
        $this->assertMultiple(false);
        parent::setValue($val);
    }
    function value()
    {
        $this->assertMultiple(false);
        return parent::value();
    }
    function valueLabel()
    {
        if (!$this->value) return NULL;

        for ($i = 0; $i < count($this->contentValues); $i++) {
            if ($this->contentValues[$i] == $this->value)
            {
                return $this->contentLabels[$i];
            }
        }
        throw( new Exception("Couldn't find label for value {$this->value} for select id '{$this->id}'.") );
    }

    function setValues($valArray)
    {
        $this->assertMultiple(true);
        if (!is_array($valArray)) throw( new Exception("setValues requires an array.") );
        $this->values = $valArray;
    }

    function valueIsSelected($value)
    {
        if ($this->multiple())
        {
            if (in_array($value, $this->values))
            {
                return true;
            }
        }
        else
        {
            if ($value == $this->value)
            {
                return true;
            }
        }
        return false;
    }

    function addValue($val)
    {
        $this->assertMultiple(true);
        $this->values[] = $val;
    }

    function values()
    {
        $this->assertMultiple(true);
        return $this->values;
    }
    function valuesLabels()
    {
        $labels = array();
        for ($i = 0; $i < count($this->contentValues); $i++) {
            if ($this->contentValues[$i] == $this->value)
            {
                array_push($labels, $this->contentLabels[$i]);
            }
        }
        return $labels;
    }

    
    function contentValues()
    {
        return $this->contentValues;
    }
    
    /**
     * A convenience function to set both values and labels simultaneously from an associative array.
     * @param assoc_array A list of value => label.
     */
    function setOptions($opts)
    {
        $this->setContentValues(array_keys($opts));
        $this->setContentLabels(array_values($opts));
    }

    function setContentValues($values)
    {
        $this->contentValues = $values;
    }

    function contentLabels()
    {
        return $this->contentLabels;
    }
    
    function setContentLabels($labels)
    {
        $this->contentLabels = $labels;
    }

    function assertMultiple($multiple)
    {
        if ($multiple !== $this->multiple()) throw( new Exception("Attempt to retrive a multiple value from a non-multiple widget, or vice-versa.") );
    }

    function pushBindings()
    {
        // our only bindable property that should be propagated back is VALUE / VALUES.
        if ($this->multiple())
        {
            // use propagateValueToBinding() to call validator and propagate new value to binding.
            $cleanValue = $this->propagateValueToBinding('values', $this->values);
            // update UI to cleaned-up value
            $this->setValues($cleanValue);
        }
        else
        {
            parent::pushBindings();
        }
    }

    function restoreState()
    {
        //  must call super
        parent::restoreState();

        if ($this->multiple())
        {
            if (isset($_REQUEST[$this->name]))
            {
                $this->setValues($_REQUEST[$this->name]);
            }
        }
        else
        {
            if (isset($_REQUEST[$this->name]))
            {
                $this->setValue($_REQUEST[$this->name]);
            }
        }
    }

    function render($blockContent = NULL)
    {
        $multiple = $this->multiple() ? ' multiple size="' . $this->visibleItems() . '" ' : '';

        $output = '<select name="' . $this->name() . ($this->multiple() ? '[]' : '') . '" ' .
                    $multiple .
                    ($this->enabled() ? '' : ' disabled readonly ') .
                    ($this->width ? ' style="width: ' . $this->width . ';" ' : '') . 
                    '>';

        $values = $this->contentValues();
        $labels = $this->contentLabels();
        for ($i = 0; $i < count($values); $i++) {
            $value = $label = $values[$i];
            if (isset($labels[$i])) $label = $labels[$i];
            if ($this->labelFormatter())
            {
                $label = $this->labelFormatter->stringForValue($label);
            }
            $selected = $this->valueIsSelected($value) ? 'selected' : '';
            $output .= "\n<option value=\"{$value}\" {$selected} >$label</option>";
        }

        $output .= "\n</select>";

        return $output;
    }

    function canPushValueBinding() { return true; }

}

?>
