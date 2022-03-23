


# hash 计算方法 (API签名规则)
func gen_http_request_sign(sourceMap map[string]interface{}, bizKey string) string {
    orderedString := orderParam(sourceMap, bizKey)
    md5Ctx := md5.New()
    md5Ctx.Write([]byte(orderedString))
    signString := md5Ctx.Sum(nil)
    return strings.ToUpper(hex.EncodeToString(signString))
}
func orderParam(source map[string]interface{}, bizKey string) string {
    var tempArr []string
    i := 0
    for k, v := range source {
        tempArr = append(tempArr, fmt.Sprintf("%v=%v", k, v))
        i++
    }
    sort.Strings(tempArr)
    temString := ""
    for n, v := range tempArr {
        if n+1 < len(tempArr) {
            temString = temString + v + "&"
        } else {
            temString = temString + v + bizKey
        }
    }
    return temString
}




# 支付页面param参数生成方法
func gen_param(params map[string]interface{}, secret_key string) string {
    sign := gen_http_request_sign(params, secret_key)
    params["hash"] = sign
    paramsStr, _ := json.Marshal(params)
    param := base64.URLEncoding.EncodeToString([]byte(paramsStr))
    return param
}



# mch_url & 通知callback param参数解析方法
func parse_url_param(param string) map[string]string{
    base64.URLEncoding.DecodeString(param)
    decodeBytes, err := base64.StdEncoding.DecodeString(encodeString)
    if err != nil {
        //process err
        return nil
    }
    params:=map[string]string{}
    for _,v :=range strings.Split(string(decodeBytes),"&") {
        kv:=strings.Split(v,"=")
        params[kv[0]]=kv[1]
    }
    return params
}



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




