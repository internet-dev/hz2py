# hz2py
汉字转拼音的 php 扩展,支持转全拼/简拼,多音字

# 例子

```php
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
```

输出结果

```json
string(18) "ygzqr,zzyhmqdrhds."
array(2) {
  ["重"]=>
  array(2) {
    [0]=>
    string(1) "z"
    [1]=>
    string(1) "c"
  }
  ["行"]=>
  array(2) {
    [0]=>
    string(1) "h"
    [1]=>
    string(1) "x"
  }
}
int(0)
string(55) "yigezhongqingren,zouzaiyinhangmenqiandirenhangdaoshang."
array(3) {
  ["重"]=>
  array(2) {
    [0]=>
    string(5) "zhong"
    [1]=>
    string(5) "chong"
  }
  ["行"]=>
  array(2) {
    [0]=>
    string(4) "hang"
    [1]=>
    string(4) "xing"
  }
  ["的"]=>
  array(3) {
    [0]=>
    string(2) "di"
    [1]=>
    string(2) "di"
    [2]=>
    string(2) "de"
  }
}
int(0)
```
