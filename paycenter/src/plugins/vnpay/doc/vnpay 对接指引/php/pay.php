<?php
	// 平台公钥，从密钥配置中获取
	// platform public key, from Secret key config
	$platPublicKey = 'MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiaKBgQCcEUIvQ/5L+SWbHOeR8VFeZvLbUk7V7OeEAQlQwIVLSZMTef3KtsOKKAsUYPf/aAcKRzZZXECODsPQiDPcdZvM/rFkgrFWkR7lPjTj5SiPxGaiK2Z2sne7A8aDF7fV/D7lfmEwNdZ7FWKVEB84/81BHnlGUwb5HpRTISG+boSO6wIDAQAB';
    // 商户私钥，商户自己生成
    // mchchant private key
    $mchPrivateKey = 'MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAMigm7rtWnTeNagwFzGTH+Uw1ypbiy7VhIoFJbgSYSSHdCaXWdT/l2+2fQlEYgAETVZ/IXB29MCnrf3O0dwRFXiipIbsm5zyqSLiS6cKXe8zN1/PlQWUbEt5wyWm0GADB/4bV6eu6gA7sGXmjQqrzfKZkcie3aK7+7ry1NFxTI51AgMBAAECgYEAklqXQAVlt1tiSQ7L3J425jp6u6uMHN4lkYmvuleuBbfKQ1yla3wIPLYjAF+iHeEFidOz0Rb19JRdmIkHDkJoJg2W27LvO6RdUwzgRnsuA3OuNz97w08B3aXXbPmB91nTFjKSlUsbh3IQWP71noxW+iKn844EW5hC5Gvn4L3quAECQQDrz1r0VKIbBSBB2aLuz1XyD/TBT2aRhvOKf0XtTRiQhNjtJxoQmvNbbhvUwj8an7HyCuihAbdbrC2ymFFyOSDZAkEA2c4Yudi48C6COrroi1cWgPlEFeGJXP/V1k5loZ1n2vstYAGPhAB4wUnFBLDvMqaBzWJt7MRkiazT8KnBQCDY/QJAMXpNlEyZwt+deBOpO0BnwSWV7mWxmMFfImU4D+WITPbC7aexsWBBxaJh1o93XCe715RwYTAR//stj3akSDoyaQJAa4FKuxC51/Nc3Fg9R+modeiTLqmv/3NXvPvdNjRXesLX1rduex0wfVdII9ShfEKrdxDKaT9W0NyzD+r6NAkCkQJBAMAnTgPYf0f7rh417GdoP7R7Nwi8KBKwPHlmfRukibOQlKt9xjqpsKJwglLn8pcnXbYbswXIdqzuBvIGAe3OWNg=';
    // 商户ID，从商户信息中获取
    // merchent ID from vntask, from User info
    $merchantCode = 'YOUR_MERCHANT_CODE_HERE';
    // 支付金额 pay money
    $payMoney = '20000';
    // 支付方式 参考： https://merchant.vnpay.cc/interface/#payment-method
    // Payment method of your choice refer: https://merchant.vnpay.cc/interface/#payment-method
    $method = 'ACB';
    // 商户订单号
    // Merchant system unique order number
    $orderNum = 'T'.date("YmdHis",time());
    // 描述
    // The virtual account description
    $productDetail = 'Test Pay';
    $dateTime = date("YmdHis",time());
    // 邮箱
    // Customer's email address
    $email = 'test@test.com';
    // 手机号码
    // Customer's mobile number
    $phone = '082112345678';
    // 在付款确认页面显示的转账对象
    // Display name on bank confirmation display
    $name = 'Neo';
    // 回调地址
    // url for callback
    $notifyUrl = 'http://example.com/callback';
    // 重定向地址
    // url for redirect
    $redirectUrl = 'http://example.com/redirect';
    // 订单过期时间 Order expiration time
    // 非必填
    $expiryPeriod = '1000';

    $params = array(
        'merchantCode' => $merchantCode,
        'payMoney' => $payMoney,
        'method' => $method,
        'orderNum' => $orderNum,
        'productDetail' => $productDetail,
        'name' => $name,
        'email' => $email,
        'phone' => $phone,
        'notifyUrl' => $notifyUrl,
        'redirectUrl' => $redirectUrl,
        'dateTime' => $dateTime,
        'expiryPeriod' => $expiryPeriod
    );

    ksort($params);
    $params_str = '';
    foreach ($params as $key => $val) {
        $params_str = $params_str . $val;
    }


    $sign = pivate_key_encrypt($params_str, $mchPrivateKey);

    $params['sign'] = $sign;

    $params_string = json_encode($params);
    $url = 'https://openapi.vnpay.cc/gateway/pay';
    $ch = curl_init();

    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
    curl_setopt($ch, CURLOPT_POSTFIELDS, $params_string);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_HTTPHEADER, array(
        'Content-Type: application/json',
        'Content-Length: ' . strlen($params_string))
    );
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);

    //execute post
    $request = curl_exec($ch);
    $httpCode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

    if($httpCode == 200)
    {
        $result = json_decode($request, true);
        echo "platRespCode :". $result['platRespCode'] . "\n";
        echo "platRespMessage :". $result['platRespMessage'] . "\n";
        echo "platOrderNum :". $result['platOrderNum'] . "\n";
        echo "orderNum :". $result['orderNum'] . "\n";
        echo "method :". $result['method'] . "\n";
        echo "name :". $result['name'] . "\n";
        echo "email :". $result['email'] . "\n";
        echo "payData :". $result['payData'] . "\n";
        echo "payMoney :". $result['payMoney'] . "\n";
        echo "payFee :". $result['payFee'] . "\n";
        echo "productDetail :". $result['productDetail'] . "\n";
        echo "platSign :". $result['platSign'] . "\n";

        $decryptStr = public_key_decrypt($result['platSign'], $platPublicKey);
        echo "decryptStr :". $decryptStr . "\n";
    }
    else {
        echo $httpCode;
    }

	function pivate_key_encrypt($data, $pivate_key)
	{
        $pivate_key = '-----BEGIN PRIVATE KEY-----'."\n".$pivate_key."\n".'-----END PRIVATE KEY-----';
        $pi_key = openssl_pkey_get_private($pivate_key);
        $crypto = '';
        foreach (str_split($data, 117) as $chunk) {
            openssl_private_encrypt($chunk, $encryptData, $pi_key);
            $crypto .= $encryptData;
        }

        return base64_encode($crypto);
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
