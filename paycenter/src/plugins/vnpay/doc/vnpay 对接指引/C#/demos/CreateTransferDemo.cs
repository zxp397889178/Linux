using vntaskdemo.utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace vntaskdemo.demos
{
    class CreateTransferDemo
    {
        // 测试环境测试账号
        private static readonly string MCH_CODE = "S820191106095842000004";  // 商户号
        private static readonly string PLAT_PUBLIC_KEY = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCcEUIvQ/5L+SWbHOeR8VFeZvLbUk7V7OeEAQlQwIVLSZMTef3KtIOKKAsUYPf/aAcKRzZZXECODsPQiDPcdZvM/rFkgrFWkR7lPjTj5SiPxGaiK2Z2sne7A8aDF7fV/D7lfmEwNdZ7FWKVEB84/81BHnlGUwb5HpRTISG+boSO6wIDAQAB";  // 平台公钥
        private static readonly string MCH_PRIVATE_KEY = "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAMigm7rtWnTeNagwFzGTH+Uw1ypbiy7VhIoFJbgSYSSHdCaXWdT/l2+2fQlEYgAETVZ/IXB29MCnrf3O0dwRFXiipIbsm5zyqSLiS6cKXe8zN1/PlQWUbEt5wyWm0GADB/4bV6eu6gA7sGXmjQqrzfKZkcie3aK7+7ry1NFxTI51AgMBAAECgYEAklqXQAVlt1tiSQ7L3J425jp6u6uMHN4lkYmvuleuBbfKQ1yla3wIPLYjAF+iHeEFidOz0Rb19JRdmIkHDkJoJg2W27LvO6RdUwzgRnsuA3OuNz97w08B3aXXbPmB91nTFjKSlUsbh3IQWP71noxW+iKn844EW5hC5Gvn4L3quAECQQDrz1r0VKIbBSBB2aLuz1XyD/TBT2aRhvOKf0XtTRiQhNjtJxoQmvNbbhvUwj8an7HyCuihAbdbrC2ymFFyOSDZAkEA2c4Yudi48C6COrroi1cWgPlEFeGJXP/V1k5loZ1n2vstYAGPhAB4wUnFBLDvMqaBzWJt7MRkikzT8KnBQCDY/QJAMXpNlEyZwt+deBOpO0BnwSWV7mWxmMFfImU4D+WITPKC7aexsWBBxaJh1o93XCe715RwYTAR//stj3akSDoyaQJAa4FKuxC51/Nc3Fg9R+modeiTLqmv/3NXvPvdNjRXesLX1rduex0wfVdII9ShfEKrdxDKaT9W0NyzD+r6NAkCkQJBAMAnTgPYf0f7rh417GdoP7R7Nwi8KBKwPHlmfRukibOQlKt9xjqpsKJwglLn8pcnXbYbswXIdqzuBvIGAe3OWNg=";  // 商户私钥
        private static readonly string payUrl = "https://openapi.vnpay.cc/gateway/cash";
        // 接口详细信息可查看接口文档，该接口文档地址：https://merchant.vnpay.cc/interface/disbursement.html#Disbursement
        public static void requestTransfer()
        {
            long currenttimemillis = (long)(DateTime.UtcNow - new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc)).TotalMilliseconds;
            string merchantCode = MCH_CODE;                                             // 商户号
            string orderNum = "CASH" + currenttimemillis.ToString();                    // 商户订单号
            string money = "20000";                                                     // 订单金额
            string feeType = "1";                                                      		 // 手续费收取方式 0：从提现金额中扣除 1：手续费另计
            string dateTime = "20191107105500";                                         // 时间戳 格式 yyyyMMddHHmmss
            string name = "CHANDRA DERMAWAN";                                           // 收款账号名称
            string number = "3500035098";                                               // 收款账号，注意收款姓名与收款卡号需要对应一致，不然可能会导致转账失败，建议对接账号信息校验接口获取账号名称，具体可参照 CheckBankAccountDemo.java
            string bankCode = "BCA";                                                    // 银行编号，BCA指BCA银行，参数列表见：https://merchant.vnpay.cc/interface/disbursement.html#ListBank
            string description = "TestCash";                                            // 订单描述
            string notifyUrl = "https://www.baidu.com";                                 // 回调地址
            Dictionary<string, object> VarPost = new Dictionary<string, object>();
            VarPost.Add("merchantCode", merchantCode);//用a标识的参数，并用POST传值方式传给网页 ­
            VarPost.Add("orderNum", orderNum);
            VarPost.Add("money", money);
            VarPost.Add("feeType", feeType);
            VarPost.Add("dateTime", dateTime);
            VarPost.Add("name", name);
            VarPost.Add("number", number);
            VarPost.Add("bankCode", bankCode);
            VarPost.Add("description", description);
            VarPost.Add("notifyUrl", notifyUrl);
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
            String keyStr = stringBuilder.ToString();  // 得到待加密的字符串
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
            Console.WriteLine("verifysign:"+ pass);
        }
    }
}
