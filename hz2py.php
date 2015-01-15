<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('hz2py')) {
	dl('hz2py.' . PHP_SHLIB_SUFFIX);
}
$module = 'hz2py';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach ($functions as $func)
{
    echo $func."$br\n";
}

$src    = '一个重庆人,走在银行门前的人行道上.';
//$result;
//$polyphone = array();
$ret = hz2py_jp($src, $result, $polyphone);

var_dump($result);
var_dump($polyphone);
var_dump($ret);

$ret = hz2py_qp($src, $result, $polyphone);

var_dump($result);
var_dump($polyphone);
var_dump($ret);

