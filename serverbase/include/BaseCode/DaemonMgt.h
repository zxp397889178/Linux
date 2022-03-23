/*
 * =====================================================================================
 *
 *       Filename:  DaemonMgt.h
 *
 *    Description:  �ػ����̹�����
 *
 *        Version:  1.0
 *        Created:  2015��4��13�� 9ʱ0��0��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wuwenchi
 *   Organization:  quwan
 *
 * =====================================================================================
 */

#ifndef _DaemonMgt_H_
#define _DaemonMgt_H_

class DaemonMgt
{
public:
	DaemonMgt();
	~DaemonMgt();

public:
	static void start(const char *i_program_name);

	static void stop(const char *i_program_name);
};

#endif
