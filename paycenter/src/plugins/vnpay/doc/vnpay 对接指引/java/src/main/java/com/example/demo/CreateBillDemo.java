package com.example.demo;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.util.EntityUtils;
import org.apache.tomcat.util.http.fileupload.IOUtils;
import org.apache.commons.codec.binary.Base64;


import javax.crypto.Cipher;
import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.interfaces.RSAPrivateKey;
import java.security.interfaces.RSAPublicKey;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;
import java.util.*;

public class CreateBillDemo {
    // 测试环境测试账号
    private static final String MCH_CODE = "S820201009141041000000";  // 商户号
    private static final String PLAT_PUBLIC_KEY = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDP+/O4+sXGNTK2YG3sBKUWhk2cqFMQBP8ZZjk8ftpGPFSRXeCC5b6/hDfDzRusD0eu1ycQS1nFLWsx+poLYQ9GDgh7WrJAwxVnHgzfTYEGgw98zPMsmx/zY2Pqgz6Oh48rUctGa141hAWAbw/mgA2D8dBD34gMa0XQsUsuqk2FrQIDAQAB";  // 平台公钥
    private static final String MCH_PRIVATE_KEY = "MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBANftCkRs0Ij5g75Lv3I32odxuLnnV/LRzX69ymBBNfBeMHoBC1u97bt+pa+qs9l5h+MnwXMhEDDas/vxIcGPiZhDwMfiNe9bq3CZ3h6aWz0ORCrD8cCNUrc6b3u0xC9GYKYeHLPsLddEb/mWjFQ0QaHCxDK0+ydFyMsygjBo0IaFAgMBAAECgYEAyTi7grytMZjlLyfEB5sJaYOPFKAFE+J00JHp0P20WIZOQsRKY3qB22LQ4P4BkL9y5TudSb8cKYsGg9lZDdrH64mq2a8kg5ZmAmIumzDvF3GOFTz3dVo88K83DqsmqhpL29sbEdYL/U/2NeQ8ZYtyKkz5nmzCbC91qG4HCakNRoECQQDuCtCZPQbRZad5o+16ClHVckDMzT9mDA2nr+UqbladUEXdjvxvxiBhWOPvXlJ5cfqAxM1GQLOw0wjZQNoSqpu1AkEA6Dcax6SJOhR0h4wIM3ZZO6FfF5MWQE6n+tHFmEVo7JMapnLJJRPKyga0S9WZAa0ZO2RXey6sWHM/tH4b4SwhkQJAPSXUDfkqPjIRwmKEx1vNR7pQwQtcZNdlzpSVlr+/Sivrto1bdU781GTkMIIXR5Eq21cysABA6IRdeol5O2BOHQJAHH+exr+Xo8wc3QG/g+lTXDEaxX7krTG6Bds2v9BM5zPKgoD11JoQOikGeYQWShFfVl2jKtf4iMZkHBeZ/lF3MQJBAN9lMkKcXZC8zwuP0OsljdFkACqhcJlIp3/NQyEViveCtJ8ABkkVygyaXflbL2+WOlOlfPYrib7bNGijeVu2rxc=";  // 商户私钥
    private static final String payUrl = "https://openapi.vnpay.cc/gateway/pay";
    //private static final String payUrl = "http://127.0.0.1:12090/gateway/pay";
    //接口详细信息可查看接口文档，该接口文档地址：https://merchant.vnpay.cc/interface/#request-payment-code
    public static void main(String[] args) throws UnsupportedEncodingException {

          //System.out.println("\\u0026\\u0026\\u0026\\u0026".replace("\\u0026","&"));

        ///pay/vnbank/202073020
        ///ku_vn_page/index.html?oid=202073031
        ///vn_direct/?orderId=202073036
        //https://momo.lf.thepashy.com/?oid=24065&c=2146861111
        //https://zalo.lf.thepashy.com/?oid=24066&c=1425948436

        //method ==> VA 银行扫码 VAD 银行直连 VAT 银行卡转卡 MOMO ZALO

        String merchantCode = MCH_CODE;                                             // 商户号
        String method = "ACB-D";                                                      // 支付方式，参数列表见：https://merchant.vnpay.cc/interface/#payment-method
        String bankCode= "ACB";
        String orderNum = "T" + System.currentTimeMillis();                         // 商户订单号
        String payMoney = "100000";                                                  // 订单金额
        String productDetail = "Test Pay";                                          // 订单详情
        String name = "neo";                                                        // 客户名称
        String email = "test@email.com";                                            // 客户邮箱，需要符合规范 xx@xx.xx
        String notifyUrl = "https://www.callback.com";                              // 回调地址，即客户支付成功后通知地址
        String redirectUrl = "https://www.redirect.com";                            // 重定向地址，即客户支付成功后页面跳转地址
        String dateTime = "20191107105500";                                         // 时间戳 格式 yyyyMMddHHmmss
        String phone = "082113079955";                                              // 手机号
        Map<String, String> createMap = new HashMap<>();
        createMap.put("merchantCode", merchantCode);
        createMap.put("method", method);
        createMap.put("orderNum", orderNum);
        createMap.put("payMoney", payMoney);
        createMap.put("productDetail", productDetail);
        createMap.put("name", name);
        createMap.put("email", email);
        createMap.put("notifyUrl", notifyUrl);
        createMap.put("redirectUrl", redirectUrl);
        createMap.put("dateTime", dateTime);
        createMap.put("phone", phone);
        createMap.put("expiryPeriod", "4320"); // 分钟，不传默认4320即72小时
        createMap.put("bankCode",bankCode);

        List<String> paramNameList = new ArrayList<>();
        for (String key : createMap.keySet()) {
            paramNameList.add(key);
        }
        Collections.sort(paramNameList);  // 排序key
        StringBuilder stringBuilder = new StringBuilder();
        for (int i = 0; i < paramNameList.size(); i++) {
            String key = paramNameList.get(i);
            stringBuilder.append(createMap.get(key));  // 拼接参数
        }
        String keyStr = stringBuilder.toString();  // 得到待加密的字符串
        System.out.println("keyStr:" + keyStr);
        String signedStr = "";
        try {
            signedStr = privateEncrypt(keyStr, getPrivateKey(MCH_PRIVATE_KEY));  // 私钥加密
        } catch (Exception e) {
            System.out.println(e);
        }
        createMap.put("sign", signedStr);

        String postStr = JSON.toJSONString(createMap);
        JSONObject postJson = JSONObject.parseObject(postStr);
        System.out.println("postJson:" + postJson);
        Long start = System.currentTimeMillis();
        JSONObject returnObj = doPost(payUrl, postJson);  // 发送 post json请求
        Long end = System.currentTimeMillis();
        System.out.println("耗时:" + (end-start));
        System.out.println("return:" + returnObj);

        boolean pass = false;
        try {
            pass = verifySign(returnObj, PLAT_PUBLIC_KEY);  // 签名验证
        } catch (Exception e) {
            System.out.println(e);
        }
        System.out.println(pass);
    }

    /**
     * 验证签名
     * @param params
     * @return
     */
    public static boolean verifySign(JSONObject params, String publickey) {
        String platSign = params.getString("platSign"); // 签名
        System.out.println("platSign:" + platSign);
        List<String> paramNameList = new ArrayList<>();
        for (String key : params.keySet()) {
            if (!"platSign".equals(key)) {
                paramNameList.add(key);
            }
        }
        Collections.sort(paramNameList);
        StringBuilder stringBuilder = new StringBuilder();
        for (int i = 0; i < paramNameList.size(); i++) {
            String name = paramNameList.get(i);
            stringBuilder.append(params.getString(name));
        }
        System.out.println("keys:" + stringBuilder);

        String decryptSign = "";
        try {
            decryptSign = publicDecrypt(platSign, getPublicKey(publickey));
        } catch (Exception e){
            System.out.println(e.toString());
        }
        System.out.println("decryptSign:" + decryptSign);

        if (!stringBuilder.toString().equalsIgnoreCase(decryptSign)) {
            return false;
        }
        return true;
    }

    /**
     * 私钥加密
     * @param data
     * @param privateKey
     * @return
     */

    public static String privateEncrypt(String data, RSAPrivateKey privateKey){
        try{
            Cipher cipher = Cipher.getInstance("RSA");
            cipher.init(Cipher.ENCRYPT_MODE, privateKey);
            return Base64.encodeBase64String(rsaSplitCodec(cipher, Cipher.ENCRYPT_MODE, data.getBytes("UTF-8"), privateKey.getModulus().bitLength()));
        }catch(Exception e){
            throw new RuntimeException("加密字符串[" + data + "]时遇到异常", e);
        }
    }

    /**
     * 公钥解密
     * @param data
     * @param publicKey
     * @return
     */

    public static String publicDecrypt(String data, RSAPublicKey publicKey){
        try{
            Cipher cipher = Cipher.getInstance("RSA");
            cipher.init(Cipher.DECRYPT_MODE, publicKey);
            return new String(rsaSplitCodec(cipher, Cipher.DECRYPT_MODE, Base64.decodeBase64(data), publicKey.getModulus().bitLength()), "UTF-8");
        }catch(Exception e){
            throw new RuntimeException("解密字符串[" + data + "]时遇到异常", e);
        }
    }

    /**
     * 得到私钥
     * @param privateKey 密钥字符串（经过base64编码）
     * @throws Exception
     */
    public static RSAPrivateKey getPrivateKey(String privateKey) throws NoSuchAlgorithmException, InvalidKeySpecException {
        //通过PKCS#8编码的Key指令获得私钥对象
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        PKCS8EncodedKeySpec pkcs8KeySpec = new PKCS8EncodedKeySpec(Base64.decodeBase64(privateKey));
        RSAPrivateKey key = (RSAPrivateKey) keyFactory.generatePrivate(pkcs8KeySpec);
        return key;
    }

    /**
     * 得到公钥
     * @param publicKey 密钥字符串（经过base64编码）
     * @throws Exception
     */
    public static RSAPublicKey getPublicKey(String publicKey) throws NoSuchAlgorithmException, InvalidKeySpecException {
        //通过X509编码的Key指令获得公钥对象
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        X509EncodedKeySpec x509KeySpec = new X509EncodedKeySpec(Base64.decodeBase64(publicKey));
        RSAPublicKey key = (RSAPublicKey) keyFactory.generatePublic(x509KeySpec);
        return key;
    }

    private static byte[] rsaSplitCodec(Cipher cipher, int opmode, byte[] datas, int keySize){
        int maxBlock = 0;
        if(opmode == Cipher.DECRYPT_MODE){
            maxBlock = keySize / 8;
        }else{
            maxBlock = keySize / 8 - 11;
        }
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        int offSet = 0;
        byte[] buff;
        int i = 0;
        try{
            while(datas.length > offSet){
                if(datas.length-offSet > maxBlock){
                    buff = cipher.doFinal(datas, offSet, maxBlock);
                }else{
                    buff = cipher.doFinal(datas, offSet, datas.length-offSet);
                }
                out.write(buff, 0, buff.length);
                i++;
                offSet = i * maxBlock;
            }
        }catch(Exception e){
            throw new RuntimeException("加解密阀值为["+maxBlock+"]的数据时发生异常", e);
        }
        byte[] resultDatas = out.toByteArray();
        IOUtils.closeQuietly(out);
        return resultDatas;
    }

    public static JSONObject doPost(String url,JSONObject json){
        HttpClient client = new DefaultHttpClient();
        HttpPost post = new HttpPost(url);
        post.setHeader("x-forwarded-for", "192.168.0.1");
        JSONObject response = null;
        try {
            StringEntity s = new StringEntity(json.toString());
            s.setContentEncoding("UTF-8");
            s.setContentType("application/json");//发送json数据需要设置contentType
            post.setEntity(s);
            HttpResponse res = client.execute(post);
            if(res.getStatusLine().getStatusCode() == HttpStatus.SC_OK){
                HttpEntity entity = res.getEntity();
                String result = EntityUtils.toString(res.getEntity());// 返回json格式：
                response = JSONObject.parseObject(result);
            }
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
        return response;
    }
}
