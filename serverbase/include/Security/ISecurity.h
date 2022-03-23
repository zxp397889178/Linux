#ifndef _GFX_SECURITY_ISECURITY_H_
#define _GFX_SECURITY_ISECURITY_H_

#include "BaseCode.h"
#include "BaseCode/mydef.h"

enum ENCRYPT_MODE
{
    FOX,
    ECB,   //Electronic Code Block, 电子密码本
    CBC,   //Cipher Block Chaining, 加密块链接
    CFB,   //Cipher Feedback,       64位加密反馈
    OFB,   //Output Feedback,       64位输出反馈

    EDE,   //采用了加密-解密-加密的方式,第一个密钥和最后一个密钥是相同的
    EDE3  //采用了加密-解密-加密的方式,三个密钥都不相同
};

enum PaddingMode
{
    padPKCS5,
    padPKCS7,
    padZERO,    // 填充0
    padNONE,    // 不做任何填充
};


#endif  //   _GFX_SECURITY_ISECURITY_H_

