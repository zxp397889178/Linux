/*
 * =====================================================================================
 *
 *       Filename:  DaemonMgt.h
 *
 *    Description:  守护进程管理类
 *
 *        Version:  1.0
 *        Created:  2015年4月13日 9时0分0秒
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
