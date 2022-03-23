using vntaskdemo.utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace vntaskdemo.demos
{
    class CallbackDemo
    {
        private static readonly string PLAT_PUBLIC_KEY = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCcEUIvQ/5L+SWbHOeR8VFeZvLbUk7V7OeEAQlQwIVLSZMTef3KtIOKKAsUYPf/aAcKRzZZXECODsPQiDPcdZvM/rFkgrFWkR7lPjTj5SiPxGaiK2Z2sne7A8aDF7fV/D7lfmEwNdZ7FWKVEB84/81BHnlGUwb5HpRTISG+boSO6wIDAQAB";  // 平台公钥
        private static readonly string jsonstr = "{\"payData\":\"3865023459\",\"platOrderNum\":\"I1_1289033071470972928\",\"payMoney\":\"20000\",\"payFee\":\"3000\",\"method\":\"I1\",\"productDetail\":\"Test Pay\",\"name\":\"neo\",\"platSign\":\"INaY8jjdvaHdWKc4gXtTee9PwLl1157wC4+XN1tIQpHxnKtEjoGAD1cHok66eK+PzvPFUfmK/opvZhqf/MhOZEX6i/C9/iU4Esi9rfcN/l+DToG+rBPv8B4Ha908irHeosd3W4F1MnPCxxdjL+NH6aYyxX9cT5rZkJz5eUlr5Y8=\",\"orderNum\":\"T1596164382079\",\"platRespCode\":\"SUCCESS\",\"platRespMessage\":\"Request Transaction Success\",\"email\":\"test@email.com\"}";

        public static void requestCallback()
        {
            bool pass = false;
            try
            {
                Dictionary<string, object> dict = JsonNewtonsoft.DeserializeJsonToDict(jsonstr);

                RSAForJava rsa = new RSAForJava();
                pass = rsa.verifySign(dict, PLAT_PUBLIC_KEY);  // 签名验证
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
            Console.WriteLine("verifysign：" + pass.ToString());
        }

    }
}
