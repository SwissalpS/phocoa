<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
/**
 * @package framework-base
 * @subpackage Error
 * @copyright Copyright (c) 2005 Alan Pinstein. All Rights Reserved.
 * @version $Id: kvcoding.php,v 1.3 2004/12/12 02:44:09 alanpinstein Exp $
 * @author Alan Pinstein <apinstein@mac.com>                        
 */

/** 
 * A generic error class.
 */
class WFError extends WFObject
{
    protected $errorMessage;
    protected $errorCode;

    function __construct($errorMessage = NULL, $errorCode = NULL)
    {
        parent::__construct();
        $this->errorCode = $errorCode;
        $this->errorMessage = $errorMessage;
    }

    function setErrorMessage($msg)
    {
        $this->errorMessage = $msg;
    }
    function errorMessage()
    {
        return $this->errorMessage;
    }

    function setErrorCode($code)
    {
        $this->errorCode = $code;
    }
    function errorCode()
    {
        return $this->errorCode;
    }
    function __toString()
    {
        return "Error #{$this->errorCode}: {$this->errorMessage}";
    }
    
}

/**
 * WFErrorArray class can be used in lieu of array() for passing into KVC functions as the $errors parameter.
 *
 * WFErrorArray knows how to handle the multi-level error structure used by {@link WFKeyValueCoding::validateObject()}.
 *
 * Using WFErrorArray instead of a standard error will allow you to use the $errors array as an object and interrogate it
 * for things like particular error codes, all errors for a certain property, etc.
 */
class WFErrorArray extends WFArray
{
    public function __construct($array = array(), $flags = 0, $iterator_class = "ArrayIterator")
    {
        parent::__construct($array, $flags, $iterator_class);
    }

    public function hasErrorWithCode($code)
    {
        foreach ($this->allErrors() as $e)
        {
            if ($e->errorCode() == $code) return true;
        }
        return false;
    }

    public function allErrors()
    {
        $flattenedErrors = array();
        foreach ($this as $k => $v) {
            if (gettype($k) == 'integer')
            {
                $flattenedErrors[] = $v;
            }
            else
            {
                $flattenedErrors = array_merge($flattenedErrors, $v);
            }
        }
        return $flattenedErrors;
    }

    /**
     * Get the errors that are not mapped to specific properties.
     *
     * @return array An array of WFError objects.
     */
    public function generalErrors()
    {
        $general = array();
        foreach ($this as $k => $v) {
            if (gettype($k) == 'integer')
            {
                $general[] = $v;
            }
        }
        return $general;
    }

    /**
     * Get all errors for the given key.
     *
     * @return array An array of all WFError objects.
     */
    public function errorsForKey($key)
    {
        if (isset($this[$key]))
        {
            return $this[$key];
        }
        return array();
    }

    /**
     * Get all error codes for the given key.
     *
     * @return array An array all codes for the WFError objects for the given key.
     */
    public function errorCodesForKey($key)
    {
        $codes = array();
        foreach ($this->errorsForKey($key) as $e) {
            $codes[] = $e->errorCode();
        }
        return $codes;
    }

    public function __toString()
    {
        $str = "";
        foreach ($this->generalErrors() as $e) {
            $str .= $e->errorCode() . ' - ' . $e->errorMessage() . "\n";
        }
        foreach ($this as $k => $v) {
            if (gettype($k) == 'integer') continue;
            $str .= "Errors for key: {$k}\n";
            $keyErrs = new WFErrorArray($v);
            $str .= $keyErrs;
        }
        return $str;
    }
}

/**
 * A special WFException subclass meant for carrying multiple WFError objects.
 *
 * WFPage automatically catches WFErrorsException's thrown from action methods and displays the errors.
 *
 * WFErrorsException knows how to handle the multi-level error structure used by {@link WFKeyValueCoding::validateObject()}.
 * @see WFErrorArray
 */
class WFErrorsException extends WFException
{
    protected $errors;

    function __construct($errors)
    {
        if (!is_array($errors)) throw( new WFException("WFErrorsException requires an array of WFError objects.") );
        if (count($errors) === 0) throw( new WFException("WFErrorsException must contain errors!") );

        if (!($errors instanceof WFErrorArray))
        {
            $errors = new WFErrorArray($errors);
        }
        $this->errors = $errors;

        $message = join(',', $this->errors->valueForKeyPath('allErrors.errorMessage'));
        parent::__construct($message);
    }

    /**
     * Get all errors in the format prescribed by {@link WFKeyValueCoding::validateObject()}
     *
     * @return array
     */
    public function errors()
    {
        return $this->errors;
    }

    /**
     * Get the errors that are not mapped to specific properties.
     *
     * @return array An array of WFError objects.
     * @deprecated Use WFErrorsException->errors()->generalErrors()
     */
    public function generalErrors()
    {
        return $this->errors->generalErrors();
    }

    /**
     * Get all errors in the current exception.
     *
     * @return array An array of all WFError objects.
     * @deprecated Use WFErrorsException->errors()->allErrors()
     */
    public function allErrors()
    {
        return $this->errors->allErrors();
    }

    /**
     * Get all errors for the given key.
     *
     * @return array An array of all WFError objects.
     * @deprecated Use WFErrorsException->errors()->errorsForKey($key)
     */
    public function errorsForKey($key)
    {
        return $this->errors->errorsForKey($key);
    }

    /**
     * Get all error codes for the given key.
     *
     * @return array An array all codes for the WFError objects for the given key.
     * @deprecated Use WFErrorsException->errors()->errorCodesForKey($key)
     */
    public function errorCodesForKey($key)
    {
        return $this->errors->errorCodesForKey($key);
    }

    /**
     * Inform a widget of all errors for the given key.
     *
     * Optionally [and by default], prune the errors that have been propagated from the current list. Since the caller will typically re-throw this exception to be caught by the WFPage,
     * the auto-pruning prevents errors from appearing twice, as the WFPage will automatically detect and report all errors as well (although not linked to widgets).
     *
     * @param string The key which generated the errors
     * @param object WFWidget The widget that the errors should be reported to.
     * @param bolean Prune errors for this key from the exception object.
     */
    public function propagateErrorsForKeyToWidget($key, $widget, $prune = true)
    {
         foreach ($this->errorsForKey($key) as $keyErr) {
             $widget->addError($keyErr);
         }
         if ($prune && isset($this->errors[$key]))
         {
             unset($this->errors[$key]);
         }
    }

    public function __toString()
    {
        return "WFErrorsException with errors: " . $this->errors;
    }
}
