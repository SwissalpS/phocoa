<?php

class WFYaml
{
    public static function loadFile($file)
    {
        if (function_exists('yaml_parse_file')) {

            // returns array() if no data, NULL if error.
            $a = yaml_parse_file($file);

            // documented to return NULL but sometimes returns FALSE.
            if (null === $a || false === $a) {

                throw new WFException("Error processing YAML file: {$file}");

            } // if error parsing file

            return $a;

        } else if (function_exists('syck_load')) {

            // php-lib-c version, much faster!
            // ******* NOTE: if using libsyck with PHP, you should install from pear/pecl (http://trac.symfony-project.com/wiki/InstallingSyck)
            // ******* NOTE: as it escalates YAML syntax errors to PHP Exceptions.
            // ******* NOTE: without this, if your YAML has a syntax error, you will be really confused when trying to debug it b/c syck_load will just return NULL.
            $yaml = NULL;
            $yamlfile = file_get_contents($file);
            if (strlen($yamlfile) != 0) {
                $yaml = syck_load($yamlfile);
            }
            if (null === $yaml) {
                $yaml = array();
            }

            return $yaml;

        } elseif (class_exists('Symfony\\Component\\Yaml\\Yaml')) {

            $aYaml = null;
            $sYaml = file_get_contents($file);

            if (strlen($sYaml)) {

                $aYaml = \Symfony\Component\Yaml\Yaml::parse($sYaml);

            }

            if (empty($aYaml)) {

                $aYaml = array();

            }

            return $aYaml;

        } else {

            // php version
            return Horde_Yaml::loadFile($file);

        } // if which do we use...

    } // loadFile


    /**
     * NOTE: libsyck extension doesn't have a 'string' loader, so we have to write a tmp file. Kinda slow... in any case though shouldn't really use YAML strings
     * for anything but testing stuff anyway
     *
     * @param
     * @return
     * @throws
     */
    public static function loadString($string)
    {
        if (function_exists('yaml_parse')) {

            // returns array() if no data, NULL if error.
            $a = yaml_parse($string);

            // documented to return NULL but sometimes returns FALSE.
            if (null === $a || false === $a) {

                throw new WFException("Error processing YAML string.");

            } // if error parsing

            return $a;

        } else if (function_exists('syck_load')) {

            // extension version
            $file = tempnam("/tmp", 'syck_yaml_tmp_');
            file_put_contents($file, $string);

            return self::loadFile($file);

        } elseif (class_exists('Symfony\\Component\\Yaml\\Yaml')) {

            return \Symfony\Component\Yaml\Yaml::parse($string);

        } else {

            // php version
            return Horde_Yaml::load($string);

        } // if which do we use...

    } // loadString


    /**
     * @deprecated Use loadFile()
     */
    public static function load($file)
    {

        return self::loadFile($file);

    }

    /**
     *  Given a php structure, returns a valid YAML string representation.
     *
     *  @param mixed PHP data
     *  @return string YAML equivalent.
     */
    public static function dump($phpData)
    {
        if (function_exists('yaml_emit')) {

            return yaml_emit($phpData);

        } else if (function_exists('syck_dump')) {

            // php-lib-c version, much faster!
            return syck_dump($phpData);

        } elseif (class_exists('Symfony\\Component\\Yaml\\Yaml')) {

            return \Symfony\Component\Yaml\Yaml::dump($phpData);

        } else {

            // php version
            return Horde_Yaml::dump($phpData);

        } // if which do we use...

    } // dump

} // WFYaml

?>
