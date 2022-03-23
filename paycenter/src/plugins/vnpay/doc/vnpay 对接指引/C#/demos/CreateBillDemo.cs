using vntaskdemo.utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace vntaskdemo.demos
{
    class CreateBillDemo
    {
        // 测试环境测试账号
        private static readonly string MCH_CODE = "S820191106095842000004";  // 商户号
        private static readonly string PLAT_PUBLIC_KEY = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCcEUIvQ/5L+SWbHOeR8VFeZvLbUk7V7OeEAQlQwIVLSZMTef3KtIOKKAsUYPf/aAcKRzZZXECODsPQiDPcdZvM/rFkgrFWkR7lPjTj5SiPxGaiK2Z2sne7A8aDF7fV/D7lfmEwNdZ7FWKVEB84/81BHnlGUwb5HpRTISG+boSO6wIDAQAB";  // 平台公钥
        private static readonly string MCH_PRIVATE_KEY = "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAMigm7rtWnTeNagwFzGTH+Uw1ypbiy7VhIoFJbgSYSSHdCaXWdT/l2+2fQlEYgAETVZ/IXB29MCnrf3O0dwRFXiipIbsm5zyqSLiS6cKXe8zN1/PlQWUbEt5wyWm0GADB/4bV6eu6gA7sGXmjQqrzfKZkcie3aK7+7ry1NFxTI51AgMBAAECgYEAklqXQAVlt1tiSQ7L3J425jp6u6uMHN4lkYmvuleuBbfKQ1yla3wIPLYjAF+iHeEFidOz0Rb19JRdmIkHDkJoJg2W27LvO6RdUwzgRnsuA3OuNz97w08B3aXXbPmB91nTFjKSlUsbh3IQWP71noxW+iKn844EW5hC5Gvn4L3quAECQQDrz1r0VKIbBSBB2aLuz1XyD/TBT2aRhvOKf0XtTRiQhNjtJxoQmvNbbhvUwj8an7HyCuihAbdbrC2ymFFyOSDZAkEA2c4Yudi48C6COrroi1cWgPlEFeGJXP/V1k5loZ1n2vstYAGPhAB4wUnFBLDvMqaBzWJt7MRkikzT8KnBQCDY/QJAMXpNlEyZwt+deBOpO0BnwSWV7mWxmMFfImU4D+WITPKC7aexsWBBxaJh1o93XCe715RwYTAR//stj3akSDoyaQJAa4FKuxC51/Nc3Fg9R+modeiTLqmv/3NXvPvdNjRXesLX1rduex0wfVdII9ShfEKrdxDKaT9W0NyzD+r6NAkCkQJBAMAnTgPYf0f7rh417GdoP7R7Nwi8KBKwPHlmfRukibOQlKt9xjqpsKJwglLn8pcnXbYbswXIdqzuBvIGAe3OWNg=";  // 商户私钥
        private static readonly string payUrl = "https://openapi.vnpay.cc/gateway/pay";
        // 接口详细信息可查看接口文档，该接口文档地址：https://merchant.vnpay.cc/interface/#request-payment-code
        public static void requestPayment()
        {
            long currenttimemillis = (long)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalMilliseconds;
            string merchantCode = MCH_CODE;                                             // 商户号
            string method = "ACB";                                                      // 支付方式，参数列表见：https://merchant.vnpay.cc/interface/#payment-method
            string bankCode = "";
            string orderNum = "T" + currenttimemillis.ToString();                       // 商户订单号
            string payMoney = "20000";                                                  // 订单金额
            string productDetail = "Test Pay";                                          // 订单详情
            string name = "neo";                                                        // 客户名称
            string email = "test@email.com";                                            // 客户邮箱，需要符合规范 xx@xx.xx
            string notifyUrl = "https://www.callback.com";                              // 回调地址，即客户支付成功后通知地址
            string redirectUrl = "https://www.redirectUrl.com";                         // 重定向地址，即客户支付成功后页面跳转
            string dateTime = "20191107105500";                                         // 时间戳 格式 yyyyMMddHHmmss
            string phone = "082113079955";                                              // 手机号
            Dictionary<string, object> VarPost = new Dictionary<string, object>(); ;
            VarPost.Add("merchantCode", merchantCode);//用a标识的参数，并用POST传值方式传给网页 ­
            VarPost.Add("method", method);
            VarPost.Add("orderNum", orderNum);
            VarPost.Add("payMoney", payMoney);
            VarPost.Add("productDetail", productDetail);
            VarPost.Add("name", name);
            VarPost.Add("email", email);
            VarPost.Add("notifyUrl", notifyUrl);
            VarPost.Add("dateTime", dateTime);
            VarPost.Add("phone", phone);
            VarPost.Add("expiryPeriod", "4320");// 分钟，不传默认4320即72小时
            List<string> paramNameList = new List<string>();
            foreach (string key in VarPost.Keys)
            {
                paramNameList.Add(key);
            }
            paramNameList.Sort();// 排序key
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i < paramNameList.Count; i++)
            {
                string key = paramNameList[i];
                stringBuilder.Append(VarPost[key]);  // 拼接参数
            }
            string keyStr = stringBuilder.ToString();  // 得到待加密的字符串
            int len = keyStr.Length;
            string signedStr = "";
            try
            {
                RSAForJava rsa = new RSAForJava();
                signedStr = rsa.EncryptByPrivateKey(keyStr, MCH_PRIVATE_KEY);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            VarPost.Add("sign", signedStr);

            WebClient w = new WebClient();
            w.Headers[HttpRequestHeader.ContentType] = "application/json";
            string jsonStr = JsonNewtonsoft.SerializeDictionaryToJsonString(VarPost);
            Console.WriteLine("postJson:" + jsonStr);     //发送字符串
            string sRemoteInfo = w.UploadString(payUrl, "POST", jsonStr);
            Console.WriteLine("return:" + sRemoteInfo);     //接收字符串
            //string sRemoteInfo = System.Text.Encoding.UTF8.GetString(byRemoteInfo);
            Dictionary<string, object> dict = JsonNewtonsoft.DeserializeJsonToDict(sRemoteInfo);
            bool pass = false;
            try
            {
                RSAForJava rsa = new RSAForJava();
                pass = rsa.verifySign(dict, PLAT_PUBLIC_KEY);  // 签名验证
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            Console.WriteLine("verifysign:"+pass);
        }

    }
}
