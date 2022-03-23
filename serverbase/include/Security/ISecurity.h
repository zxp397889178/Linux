#ifndef _GFX_SECURITY_ISECURITY_H_
#define _GFX_SECURITY_ISECURITY_H_

#include "BaseCode.h"
#include "BaseCode/mydef.h"

enum ENCRYPT_MODE
{
    FOX,
    ECB,   //Electronic Code Block, �������뱾
    CBC,   //Cipher Block Chaining, ���ܿ�����
    CFB,   //Cipher Feedback,       64λ���ܷ���
    OFB,   //Output Feedback,       64λ�������

    EDE,   //�����˼���-����-���ܵķ�ʽ,��һ����Կ�����һ����Կ����ͬ��
    EDE3  //�����˼���-����-���ܵķ�ʽ,������Կ������ͬ
};

enum PaddingMode
{
    padPKCS5,
    padPKCS7,
    padZERO,    // ���0
    padNONE,    // �����κ����
};


#endif  //   _GFX_SECURITY_ISECURITY_H_

