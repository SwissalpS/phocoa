<?php
/* * * *
 * * SssS_IncludeThesePathsToPHPIncludePath.inc
 * *
 * * no autoload support but is a great help when initiating it
 * *
 * * @version 20100501_004024 (CC) Luke JZ aka SwissalpS
 * * * */
if (!function_exists('SssS_IncludeThesePathsToPHPIncludePath')) {
	/**
	 * SssS_IncludeThesePathsToPHPIncludePath($mPaths, $bPrependWithoutChecking = false)
	 * makes sure the given paths are added to PHPs include_path
	 * uses ini_set('include_path', ...) and ini_get('include_path')
	 *
	 * @param mixed $mPaths array of string paths or 1 path string
	 * @param mixed $bPrependWithoutChecking bool when true just prepends the paths in the order they came to the current include path
	 *
	 * @return mixed null on failure uint of how many added
	 *
	 * @uses PHP - keep it that way
	 *
	 * @package __SssS__ - __package_desciption_here__
	 * @author Luke JZ aka SwissalpS <SwissalpS@LukeZimmerman.com>
	 * @license __license_url_here__
	 * @copyright 2007 Luke JZ aka SwissalpS
	 * @version 20100501_005024 (CC)2007 Luke JZ aka SwissalpS
	 */
	function SssS_IncludeThesePathsToPHPIncludePath($mPaths, $bPrependWithoutChecking = false) {
		// nonsense -> go home
		if (empty($mPaths)) return null;

		// only one path as string supplied...
		if ('string' == gettype($mPaths)) $mPaths = array($mPaths);

		// final type check
		if ('array' != gettype($mPaths)) return null;

		// get current include path
		$sIncludePath = ini_get('include_path');

		// add ':' if not yet
		if (':' !== substr($sIncludePath, -1)) $sIncludePath .= ':';

		// prepare new path string
		$sNewIncludePath = ($bPrependWithoutChecking) ? '' : $sIncludePath;

		// add any missing paths
		$iCount = 0;
		foreach($mPaths as $sPath) {
			// append ':' if not yet
			if (':' !== substr($sPath, -1)) $sPath .= ':';

			if ($bPrependWithoutChecking) {
				$sNewIncludePath .= $sPath;
				$iCount++;

			} else {
				// check existance
				if (false === strstr($sIncludePath, $sPath)) {
					$sNewIncludePath .= $sPath;
					$iCount++;

				} // if missing found
			} // if $bPrependWithoutChecking
		} // loop all paths

		$sNewIncludePath .= ($bPrependWithoutChecking) ? $sIncludePath : '';

		// set the new path
		if (false === ini_set('include_path', $sNewIncludePath)) return null;

		return $iCount;
	} // SssS_IncludeThesePathsToPHPIncludePath
} // if not yet defined function SssS_IncludeThesePathsToPHPIncludePath

// prepend no matter what
if (!function_exists('SssS_PrependThesePathsToPHPIncludePath')) {
	function SssS_PrependThesePathsToPHPIncludePath($mPaths) {
		return SssS_IncludeThesePathsToPHPIncludePath($mPaths, true);
	} // SssS_PrependThesePathsToPHPIncludePath
} // if not yet defined function SssS_PrependThesePathsToPHPIncludePath


/*/ example adds both paths to include path if not yet there
$sSwissalpSFrameworkPath = "/Volumes/UNIX/public/SssS";
$mRes = SssS_IncludeThesePathsToPHPIncludePath(array($sSwissalpSFrameworkPath,
													'/Volumes/UNIX'));


/* * * *\ SssS_IncludeThesePathsToPHPIncludePath.inc (CC) Luke JZ aka SwissalpS /* * * */
?>
