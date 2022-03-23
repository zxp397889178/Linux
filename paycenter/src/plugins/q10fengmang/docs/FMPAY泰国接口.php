<?php
        $params['partner_id'] = '108660';
        // 支付类型：0001 固定值
        $params['pay_type'] = '0001';
        // 用户姓名；
        $params['bank_code'] = 'ThaiQR'; //ThaiQR，ThaiP2P
        // 接口版本: API协议版本，当前值：V1.0 (大写)
        $params['version'] = 'V1.0';
        // 商户订单号: 商家网站生成的订单号，由商户保证其唯一性，由字 母、数字、下划线组成，字符长度不超过32位.
        $params['order_no'] = intval(microtime(true) * 1000 * 1000);
        // 订单金额: 以元为单位，精确到小数点后两位.例如:12.00、14.01等
        $params['amount'] = '2000';
		// 是否返回json
        $params['attach'] = 'json';
        // 异步通知地址
        $params['notify_url'] = 'https://gateway.fmpay.org/payment/notify_url';
		
		$params['sign'] = generateSign($params, 'c772de8b4a85be8a2eac6ae500616e37');

		echo build_form($params, 'https://gateway.fmpay.org/payment/gateway');
        exit;
      function build_form ( $data , $gateway , $method = 'post' )
    {
        $sHtml = "<form id='pay_form' name='pay_form' action='" . $gateway . "' method='" . $method . "'>";
        foreach ( $data as $key => $val ) {
            $sHtml .= "<input type='hidden' name='" . $key . "' value='" . $val . "'/>";
        }
        $sHtml .= "</form>";
        $sHtml .= "<script>document.forms['pay_form'].submit();</script>";
        
        return $sHtml;
    }

		function generateSign($data, $signKey)
    {
        // 按照ASCII码升序排序
        ksort($data);

        $str = '';
        foreach ($data as $key => $value) {
            $value = trim($value);
            if ('sign' != $key && '' != $value) {
                $str .= $key . '=' . $value . '&';
            }
        }

        $str .= $signKey;

        return strtolower(md5($str));
    }

		?>

		