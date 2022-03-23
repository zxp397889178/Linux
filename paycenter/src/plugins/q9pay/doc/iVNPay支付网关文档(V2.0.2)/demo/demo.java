


# hash 计算方法 (API签名规则)
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import java.util.SortedMap;
import java.security.MessageDigest;
import java.util.Iterator;
import java.math.BigInteger;

public  String gen_http_request_sign(Map<String,Object> params, String secret)
{
    params.remove("hash");
    // 先将参数以其参数名的字典序升序进行排序
    Map<String, Object> sortedTree = new TreeMap<String, Object>(params);
    // 遍历排序后的字典，将所有参数按"key=value"格式拼接在一起
    StringBuilder basestring = new StringBuilder();

    int count = 0;
    Iterator<String> it = sortedTree.keySet().iterator();
    while(it.hasNext()){
        String key = it.next();
        String value = sortedTree.get(key).toString();
        if(count > 0){
            basestring.append("&");
        }
        basestring.append(key).append("=").append(value);
        count++;
    }
    basestring.append(secret);
    try {
        // 使用MD5对待签名串求签
        byte[] bytes = null;
        MessageDigest md5 = MessageDigest.getInstance("MD5");
        bytes = md5.digest(basestring.toString().getBytes("UTF-8"));
        //16是表示转换为16进制数
        String sign = new BigInteger(1, bytes).toString(16).toUpperCase();
        return sign;
    } catch (Exception e) {
        throw new RuntimeException(e);
    }

}






# 支付页面param参数生成方法
public  String gen_param(Map<String, Object> params,   String secret_key) {
    String signstr = gen_http_request_sign(params, secret_key);
    params.put("hash", signstr);
    String param_str = JSON.toJSONString(params);
    try {
        return  Base64.getEncoder().encodeToString(param_str.getBytes("utf-8"));
    }catch (Exception e) {
        throw new RuntimeException(e);
    }
}



# mch_url & 通知callback param参数解析方法
import java.util.Base64;
import java.util.HashMap;
import java.util.Map;

public Map<String, Object> parse_url_param(String param) {
    String paramstr = new  String(Base64.getDecoder().decode(param));
    Map<String, Object> map = new HashMap<String, Object>();
    String[] keyValues = paramstr.split("&");
    for (int i = 0; i < keyValues.length; i++) {
        String key = keyValues[i].substring(0, keyValues[i].indexOf("="));
        String value = keyValues[i].substring(keyValues[i].indexOf("=") + 1);
        map.put(key, value);
    }
    return map;
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




