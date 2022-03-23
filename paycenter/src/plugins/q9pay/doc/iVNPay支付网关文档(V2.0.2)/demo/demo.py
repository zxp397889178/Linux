


# hash 计算方法 (API签名规则)
def gen_http_request_sign(req_params, secret_key):
      # req_params (kv 字典)
      keys = req_params.keys()
      keys.remove("hash")
      s_keys = sorted(keys)
      prestr_list = []
      for key in s_keys:
          value = req_params.get(key)
          if value is None:
              value = ""
          s = str(key) + "=" + str(value)
          prestr_list.append(s)
      prestr = "&".join(prestr_list)
      prestr += secret_key
      sign = gen_md5(prestr)
      sign = sign.upper()
      return sign




# 支付页面param参数生成方法
def gen_param(req_params,secret_key):
      signstr = gen_http_request_sign(req_params,secret_key)
      req_params['hash'] = signstr
      param_str=json.dumps(req_params)
      param=base64.urlsafe_b64encode(param_str)
      return param



# mch_url & 通知callback param参数解析方法
def parse_url_param(param):
    params_str = base64.urlsafe_b64decode(param)
    params={}
    for  key_value  in  params_str.split( '&' ):
        kv = key_value.split('=')
        params[kv[0]] = kv[1] 
    return params



# 聚合页面param 生成示例
req_params={
"mch_id":'100000',
"mch_uid":"xxxxxx",
"mch_order_id":'02202008101010000020081034811088',
"equipment_type": 1,
"expected_mount":50000,
"mch_url":"http://xxxxx",
"show_types":"3"
}
secret_key = 'N111110dQfPLTxc'
param = gen_param(req_params,secret_key)
url = endpoint + "/public/comm/index.html?param=" + param



# 支付订单查询接口
请求举例:
{"mch_id":"000100",
"mch_order_id":"0220200328000100f66c126e70ce11eab7f1595321913560",
"hash":"EF962FBF690F1DDB1D62BE61B6633AC9"
}

返回举例:
{
    "data": {
        "mch_id": "000100",
        "mch_order_id": "0220200328000100f66c126e70ce11eab7f1595321913560",
        "pay_type": 4,
        "amount": 10000,
        "svr_transaction_id": "V220200721010001003556434114268020992",
        "status": 1,
        "attach":"xxxx"
    },
    "ret": 0
}



# 代付订单创建
请求举例:
{
    "pay_type":13,
    "account":"收款人账号",
    "hash":"xxxxxxxxxxxxxxxxxx",
    "bank_name":"收款银行",
    "payee_name":"收款人姓名",
    "amount":10000,
    "mch_id":"100000",
    "notify_url":"https://www.baidu.com",
    "mch_order_id":"xxxxxxxxxxxxxxx",
    "attach":"funx"
}

返回举例:
{
    "data": {
        "mch_order_id": "0220200328000100f66c126e70ce11eab7f1595321913560",
        "svr_transaction_id": "V220200721010001003556434114268020992",
        "status": 0
    },
    "ret": 0
}



# 代付订单查询
请求举例:
{
    "mch_id":"100",
    "hash":"099881BB4FA17280F0764735FCE32164",
    "mch_order_id":"02202008140001009d4eed9ede2411eaa786784f439badb1"
}

返回举例:
{
    "data": {
        "account":"xxx",
        "amount":10000,
        "pay_type":13,
        "mch_id":"100000",
        "mch_order_id": "02202008140001009d4eed9ede2411eaa786784f439badb1",
        "svr_transaction_id": "V220200721010001003556434114268020992",
        "status": 0
    },
    "ret": 0
}




