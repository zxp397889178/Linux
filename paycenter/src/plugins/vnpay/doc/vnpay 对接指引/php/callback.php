<?php

$res = $_POST;
$platSign = $res['platSign'];
unset($res['platSign']);
$public_key = 'MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDFJ/AmUV4Z8udG8aOBUt/kEwc/DbxF5Gtfw6Y00NHQ4Pz2X2x9IxjUZxn2dnFxmrmhqKNlfwXOqyejhBzi0pSHyGoI4XP9IEfZGO6YkSb9DCY1ZxX8fDl2G+tPCbWYTVO4JutFmzTWgk1Uhhu6L9dlOMUHvZf3/6czA/a9C7azXwIDAQAB';
$decryptSign = public_key_decrypt($platSign, $public_key);

$params = $res;
ksort($params);
$params_str = '';
foreach ($params as $key => $val) {
    $params_str = $params_str . $val;
}

if($params_str == $decryptSign) {
    if($res['code'] == '00') {
        echo 'success';
    }
    else {
        echo 'fail';
    }
}
else {
    echo 'fail';
}

function public_key_decrypt($data, $public_key)
{
    $public_key = '-----BEGIN PUBLIC KEY-----'."\n".$public_key."\n".'-----END PUBLIC KEY-----';
    $data = base64_decode($data);
    $pu_key =  openssl_pkey_get_public($public_key);
    $crypto = '';
    foreach (str_split($data, 128) as $chunk) {
        openssl_public_decrypt($chunk, $decryptData, $pu_key);
        $crypto .= $decryptData;
    }

    return $crypto;
}