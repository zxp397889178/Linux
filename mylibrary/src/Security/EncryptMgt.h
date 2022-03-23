/*
* =====================================================================================
*
*       Filename:  EncryptMgt.h
*
*    Description:  加密管理类
*
*        Version:  1.0
*        Created:  2015年7月2日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/
#ifndef _EncryptMgt_H_
#define _EncryptMgt_H_
#include <cstddef>

class EncryptMgt
{
public:
	static EncryptMgt *getInstance();
	
	EncryptMgt();
	~EncryptMgt();

public:
	/*初始化函数*/
	bool Init();

	/*加密函数*/
	bool Encrypt(unsigned char *buf, size_t nLen);

	/*解密函数*/
	bool Decrypt(unsigned char *buf, size_t nLen);

protected:
	bool internal_encrypt(unsigned char *buf, size_t nLen);
private:
	unsigned char m_key1[256];
	unsigned char m_key2[256];
private:
	static EncryptMgt *m_instance;

};

#define ENCRYPTMGT EncryptMgt::getInstance()


#endif

