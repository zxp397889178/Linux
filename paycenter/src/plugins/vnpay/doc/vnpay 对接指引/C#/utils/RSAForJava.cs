using Org.BouncyCastle.Asn1;
using Org.BouncyCastle.Asn1.Pkcs;
using Org.BouncyCastle.Asn1.X509;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Encodings;
using Org.BouncyCastle.Crypto.Engines;
using Org.BouncyCastle.Crypto.Generators;
using Org.BouncyCastle.Crypto.Parameters;
using Org.BouncyCastle.Pkcs;
using Org.BouncyCastle.Security;
using Org.BouncyCastle.X509;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace vntaskdemo.utils
{
    public class RSAForJava
    {

        public RSAForJava()
        {

        }
        /// <summary>
        /// KEY 结构体
        /// </summary>
        public struct RSAKEY
        {
            /// <summary>
            /// 公钥
            /// </summary>
            public string PublicKey
            {
                get;
                set;
            }
            /// <summary>
            /// 私钥
            /// </summary>
            public string PrivateKey
            {
                get;
                set;
            }
        }
        public RSAKEY GetKey()
        {
            //RSA密钥对的构造器
            RsaKeyPairGenerator keyGenerator = new RsaKeyPairGenerator();

            //RSA密钥构造器的参数
            RsaKeyGenerationParameters param = new RsaKeyGenerationParameters(
                Org.BouncyCastle.Math.BigInteger.ValueOf(3),
                new Org.BouncyCastle.Security.SecureRandom(),
                1024,   //密钥长度
                25);
            //用参数初始化密钥构造器
            keyGenerator.Init(param);
            //产生密钥对
            AsymmetricCipherKeyPair keyPair = keyGenerator.GenerateKeyPair();
            //获取公钥和密钥
            AsymmetricKeyParameter publicKey = keyPair.Public;
            AsymmetricKeyParameter privateKey = keyPair.Private;

            SubjectPublicKeyInfo subjectPublicKeyInfo = SubjectPublicKeyInfoFactory.CreateSubjectPublicKeyInfo(publicKey);
            PrivateKeyInfo privateKeyInfo = PrivateKeyInfoFactory.CreatePrivateKeyInfo(privateKey);


            Asn1Object asn1ObjectPublic = subjectPublicKeyInfo.ToAsn1Object();

            byte[] publicInfoByte = asn1ObjectPublic.GetEncoded("UTF-8");
            Asn1Object asn1ObjectPrivate = privateKeyInfo.ToAsn1Object();
            byte[] privateInfoByte = asn1ObjectPrivate.GetEncoded("UTF-8");

            RSAKEY item = new RSAKEY()
            {
                PublicKey = Convert.ToBase64String(publicInfoByte),
                PrivateKey = Convert.ToBase64String(privateInfoByte)
            };
            return item;
        }
        private AsymmetricKeyParameter GetPublicKeyParameter(string s)
        {
            s = s.Replace("\r", "").Replace("\n", "").Replace(" ", "");
            byte[] publicInfoByte = Convert.FromBase64String(s);
            Asn1Object pubKeyObj = Asn1Object.FromByteArray(publicInfoByte);//这里也可以从流中读取，从本地导入
            AsymmetricKeyParameter pubKey = PublicKeyFactory.CreateKey(publicInfoByte);
            return pubKey;
        }
        private AsymmetricKeyParameter GetPrivateKeyParameter(string s)
        {
            s = s.Replace("\r", "").Replace("\n", "").Replace(" ", "");

            byte[] privateInfoByte = Convert.FromBase64String(s);
            // Asn1Object priKeyObj = Asn1Object.FromByteArray(privateInfoByte);//这里也可以从流中读取，从本地导入
            // PrivateKeyInfo privateKeyInfo = PrivateKeyInfoFactory.CreatePrivateKeyInfo(privateKey);
            AsymmetricKeyParameter priKey = PrivateKeyFactory.CreateKey(privateInfoByte);
            return priKey;
        }

        private static StreamReader GetStreamReader(string content)
        {
            byte[] bytes = Encoding.UTF8.GetBytes(content);
            var memory = new MemoryStream(bytes);
            var reader = new StreamReader(memory);

            return reader;
        }

        public string EncryptByPrivateKey(string s, string key)
        {
            //非对称加密算法，加解密用
            IAsymmetricBlockCipher engine = new Pkcs1Encoding(new RsaEngine());
            //加密
            try
            {
                engine.Init(true, GetPrivateKeyParameter(key));
                byte[] byteData = System.Text.Encoding.UTF8.GetBytes(s);

                byte[] result = new byte[] { };
                for (int i = 0, j = byteData.Length / 117 + (byteData.Length % 117 == 0 ? 0 : 1); i < j; i++)
                {
                    byte[] getData = byteData.Skip(i * 117).Take(117).ToArray();
                    byte[] ResultData = engine.ProcessBlock(getData, 0, getData.Length);
                    result = result.Concat(ResultData).ToArray();
                }

                //var ResultData = engine.ProcessBlock(byteData, 0, byteData.Length);
                return Convert.ToBase64String(result);

                //Console.WriteLine("密文（base64编码）:" + Convert.ToBase64String(testData) + Environment.NewLine);
            }
            catch (Exception ex)
            {
                return ex.Message;
            }
        }
        public string DecryptByPublicKey(string s, string key)
        {
            s = s.Replace("\r", "").Replace("\n", "").Replace(" ", "");
            //非对称加密算法，加解密用
            IAsymmetricBlockCipher engine = new Pkcs1Encoding(new RsaEngine());

            //解密

            try
            {
                engine.Init(false, GetPublicKeyParameter(key));
                byte[] byteData = Convert.FromBase64String(s);

                string all = "";

                for (int i = 0, j = byteData.Length / 128 + (byteData.Length % 128 == 0 ? 0 : 1); i < j; i++)
                {
                    byte[] getData = byteData.Skip(i * 128).Take(128).ToArray();
                    byte[] ResultData = engine.ProcessBlock(getData, 0, getData.Length);
                    all += Encoding.UTF8.GetString(ResultData);
                }
                return all;
                //var ResultData = engine.ProcessBlock(byteData, 0, byteData.Length);
                //return System.Text.Encoding.UTF8.GetString(ResultData);

            }
            catch (Exception ex)
            {
                return ex.Message;

            }
        }
        /// <summary>
        /// 验证签名
        /// </summary>
        /// <param name="?"></param>
        /// <returns></returns>
        public bool verifySign(Dictionary<string, object> dict, string publickey)
        {
            string platSign = dict["platSign"].ToString().Trim(); // 签名
            List<string> paramNameList = new List<string>();
            foreach (string key in dict.Keys)
            {
                if (!"platSign".Equals(key))
                {
                    paramNameList.Add(key);
                }
            }
            paramNameList.Sort();
            StringBuilder stringBuilder = new StringBuilder();
            for (int i = 0; i < paramNameList.Count; i++)
            {
                string name = paramNameList[i];
                stringBuilder.Append(dict[name]);
            }

            String decryptSign = "";
            try
            {
                decryptSign = DecryptByPublicKey(platSign, publickey);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }

            if (!stringBuilder.ToString().Equals(decryptSign, StringComparison.OrdinalIgnoreCase))
            {
                return false;
            }
            return true;
        }
    }
}