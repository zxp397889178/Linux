/*
 * =====================================================================================
 *
 *       Filename:  DaemonMgt.cpp
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

#include "DaemonMgt.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
//

const int MAX_PATH = 256;

void signal_handle(int sig)
{
	exit (0);
}

DaemonMgt::DaemonMgt()
{
	
}

DaemonMgt::~DaemonMgt()
{
	
}

void DaemonMgt::start(const char *i_program_name)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) 
	{
		printf("fork1 failed\n"); 
		exit (-1);
	}

	if (pid > 0) 
	{
		// �������ڴ˴��˳�
		exit (0);
	}

	setsid();

	pid = fork();
	if (pid < 0) 
	{
		printf("fork2 failed\n"); 
		exit (-1);
	}

	if (pid > 0) 
	{
		exit (0);
	}

	signal(SIGTERM, signal_handle);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, signal_handle);

	umask(0);	//�����ļ�������ģ 

	//����.pid�ļ�������pid��Ϣ
	char current_path[MAX_PATH] = "";
	if (getcwd(current_path, MAX_PATH) == NULL)
	{
		printf("get current path failed\n"); 
		exit (-1);
	}
	string seperator = "/";
	string file_postfix = ".pid";	//�ļ���׺
	string file_name = current_path + seperator + i_program_name + file_postfix;
	printf("pid store file =[%s]\n",file_name.c_str());

	ofstream fout;
	fout.open(file_name.c_str(), ios::out);
	if (!fout)
	{
		cout << "���ļ�ʧ�ܣ�" << endl;
		exit (-1);
	}
	pid = getpid();
	fout << pid << endl;
	fout.close();
	
}

void DaemonMgt::stop(const char *i_program_name)
{
	char current_path[MAX_PATH] = "";
	if (getcwd(current_path, MAX_PATH) == NULL)
	{
		printf("get current path failed\n"); 
		exit (-1);
	}
	string seperator = "/";
	string file_postfix = ".pid";	//�ļ���׺
	string file_name = current_path + seperator + i_program_name + file_postfix;
	printf("pid store file =[%s]\n",file_name.c_str());	

	ifstream fin(file_name.c_str(),ios::in);
	if (!fin)
	{
		cout << "���ļ�ʧ�ܣ�" << endl;
		exit (-1);
	}
	pid_t pid = 0;
	char buf[32] = "";
	fin.read(buf,32);
	pid = atoi(buf);
	if ( kill(pid, SIGQUIT) == 0 ) 
	{
		printf ("Program %s(pid=%d) is killed.\n", i_program_name, pid);
	} 
	else 
	{
		printf ("Program %s(pid=%d) kill failure.\n", i_program_name, pid);
		exit(-1);
	}
}

