/*
* =====================================================================================
*
*       Filename:  CloseClientError.h
*
*    Description:  �رտͻ��˴�����Ϣ��
*
*        Version:  1.0
*        Created:  2015��5��18�� 9ʱ0��0��
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
		ACCOUNT_HAS_BEEN_BINDED = 1001,		//�˺��Ѿ���
		WRONG_ACCOUNT_PASSWD = 1002,		//�˺��������
		ACCOUNT_NOT_FOUND = 1003,			//�˺�û�ҵ�
		OTHER_PALCE_LOGIN = 1004,			//��ص�½
		VERSION_MISMATCH = 1005,			//�汾�Ų�ƥ��
		ILLEGAL_SOURCE = 1006,				//�Ƿ���Դ
		ILLEGAL_PROTOCOL = 1007,			//�Ƿ�Э��
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


