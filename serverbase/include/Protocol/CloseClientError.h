/*
* =====================================================================================
*
*       Filename:  CloseClientError.h
*
*    Description:  关闭客户端错误信息类
*
*        Version:  1.0
*        Created:  2015年5月18日 9时0分0秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/
#ifndef _CloseClientError_H_
#define _CloseClientError_H_
#include <string.h>
#include <stdarg.h>
#include <exception>
using namespace std;

#define MAX_MSG_LEN 256

class CloseClientError : public std::exception
{
public:
	enum ErrorCode
	{
		BASE_CODE = 1000,
		ACCOUNT_HAS_BEEN_BINDED = 1001,		//账号已经绑定
		WRONG_ACCOUNT_PASSWD = 1002,		//账号密码错误
		ACCOUNT_NOT_FOUND = 1003,			//账号没找到
		OTHER_PALCE_LOGIN = 1004,			//异地登陆
		VERSION_MISMATCH = 1005,			//版本号不匹配
		ILLEGAL_SOURCE = 1006,				//非法来源
		ILLEGAL_PROTOCOL = 1007,			//非法协议
		MAX_CODE = 1008
	};
public:
	CloseClientError(int error_code, const char *err_msg = "unknow");

	virtual ~CloseClientError() throw(){}

	virtual const char *what() const throw()
	{
		return m_err_msg;
	}

public:
	int GetErrorCode()
	{
		return m_error_code;
	}
	const char *GetErrorMsg()
	{
		return m_err_msg;
	}
private:
	int m_error_code;
	char m_err_msg[MAX_MSG_LEN];
};



#define THROW_CLOSE_CLIENT(CODE) throw CloseClientError(CODE);

#endif


