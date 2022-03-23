#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "SysInfoFunc.h"

using namespace std;

bool GetCpuAndMem(char *szcpu, char *szmem)
{
	return GetCpuAndMem(::getpid(),szcpu, szmem);
}

bool GetCpuAndMem(std::vector<pid_t>& pids,char *szcpu, char *szmem)
{
	uint32_t total_szcpu=0,total_szmem=0,tmp_szcpu=0,tmp_szmem=0;
	for(size_t i=0;i<pids.size();i++)
	{
		tmp_szcpu=0;
		tmp_szmem=0;
		if(GetCpuAndMem(pids[i],tmp_szcpu,tmp_szmem))
		{
			total_szcpu += tmp_szcpu;
			total_szmem += tmp_szmem;
		}
	}

	sprintf(szcpu,"%u",total_szcpu);
	strcat(szcpu,"\%");
	sprintf(szmem,"%uK",total_szmem);
	return true;
}

bool GetTotalCpuAndMem(char *szcpu, char *szmem)
{
	//获取当前进程的命令参数
	char execparam[100]={0};
	GetCurExecParam(::getpid(),execparam);
	return GetCpuAndMem(execparam,szcpu,szmem);
}

bool GetCpuAndMem(char* execparam,char *szcpu, char *szmem)
{
	char szCommand[100] = {0};	//存放命令
	char line[100] = {0};		//存放每行数据
	char *token = NULL;
	bool bFind = false;
	uint32_t total_szcpu=0,total_szmem=0,tmp_szcpu=0,tmp_szmem=0;
	sprintf(szCommand, "SYSTEM98= ps -e -o \"pcpu,vsz,args\"|grep \"%s[[:space:]]\"|grep -v \"grep\"", execparam);
	FILE *fp = popen(szCommand, "r");
	if(NULL == fp)
	{
		return false;
	}
	while(NULL != fgets(line, sizeof(line)-1, fp))
	{
		int nOffset = 0;

		// 去除多余空格
		for(size_t i=0;i<sizeof(line); ++i)
		{
			if(line[i] == ' ')
				nOffset++;
			else
				break;
		}

		// 查询是否满足
		if (NULL != (token = strtok((char*)line+nOffset, " \t\n")))
		{
			tmp_szcpu = atoi(token);
			token = strtok(NULL," \t\n");
			tmp_szmem = atoi(token);
			total_szcpu += tmp_szcpu;
			total_szmem += tmp_szmem;
			bFind = true;
		}
	}

	sprintf(szcpu,"%u",total_szcpu);
	strcat(szcpu,"\%");
	sprintf(szmem,"%uK",total_szmem);

	pclose(fp);
	return bFind;
}

bool GetCurExecParam(pid_t pid,char* execparam)
{
	char szCommand[100] = {0};	//存放命令
	char line[100] = {0};		//存放每行数据
	char *token = NULL;
	bool bFind = false;
	sprintf(szCommand, "SYSTEM98= ps -e -o \"pid,args\"|grep \"%d\"|grep -v \"grep\"", pid);
	FILE *fp = popen(szCommand, "r");
	if(NULL == fp)
	{
		//cout<<" szCommand = "<<szCommand<<endl;
		//cout<<" fp popen error"<<endl;
		return false;
	}
	while(NULL != fgets(line, sizeof(line)-1, fp))		//一般就是第一行,但是还是要去除vsz=pid有相同的存在
	{
		int nOffset = 0;

		// 去除多余空格
		for(size_t i=0;i<sizeof(line);++i)
		{
			if(line[i] == ' ')
				nOffset++;
			else
				break;
		}

		// 查询是否满足
		if (NULL != (token = strtok((char*)line+nOffset, " \t\n")))
		{
			pid_t tempPid = atoi(token);

			// 匹配到进程的时候
			if(tempPid == pid)
			{
				token = strtok(NULL," \t\n");
				strcpy(execparam, token);
				bFind = true;
				break;
			}
			else
				continue;
		}
	}
	pclose(fp);
	return bFind;
}

bool GetCpuAndMem(pid_t pid,char *szcpu, char *szmem)
{
	char szCommand[100] = {0};	//存放命令
	char line[100] = {0};		//存放每行数据
	char *token = NULL;
	bool bFind = false;
	sprintf(szCommand, "SYSTEM98= ps -e -o \"pid,pcpu,vsz,args\"|grep \"%d\"|grep -v \"grep\"", pid);
	FILE *fp = popen(szCommand, "r");
	if(NULL == fp)
	{
		//cout<<" szCommand = "<<szCommand<<endl;
		//cout<<" fp popen error"<<endl;
		return false;
	}
	while(NULL != fgets(line, sizeof(line)-1, fp))		//一般就是第一行,但是还是要去除vsz=pid有相同的存在
	{
		int nOffset = 0;

		// 去除多余空格
		for(size_t i=0;i<sizeof(line);++i)
		{
			if(line[i] == ' ')
				nOffset++;
			else
				break;
		}

		// 查询是否满足
		if (NULL != (token = strtok((char*)line+nOffset, " \t\n")))
		{
			pid_t tempPid = atoi(token);

			// 匹配到进程的时候
			if(tempPid == pid)
			{
				token = strtok(NULL," \t\n");
				strcpy(szcpu, token);
				strcat(szcpu,"\%");
				token = strtok(NULL," \t\n");
				strcpy(szmem, token);
				strcat(szmem, "K");
				bFind = true;
				//printf("szmem = %s, szcpu = %s\n", szmem,szcpu);
				break;
			}
			else
				continue;
		}
	}
	pclose(fp);
	return bFind;
}

bool GetCpuAndMem(pid_t pid,uint32_t& szcpu, uint32_t& szmem)
{
	char szCommand[100] = {0};	//存放命令
	char line[100] = {0};		//存放每行数据
	char *token = NULL;
	bool bFind = false;
	sprintf(szCommand, "SYSTEM98= ps -e -o \"pid,pcpu,vsz,args\"|grep \"%d\"|grep -v \"grep\"", pid);
	FILE *fp = popen(szCommand, "r");
	if(NULL == fp)
	{
		//cout<<" szCommand = "<<szCommand<<endl;
		//cout<<" fp popen error"<<endl;
		return false;
	}
	while(NULL != fgets(line, sizeof(line)-1, fp))		//一般就是第一行,但是还是要去除vsz=pid有相同的存在
	{
		int nOffset = 0;

		// 去除多余空格
		for(size_t i=0;i<sizeof(line);++i)
		{
			if(line[i] == ' ')
				nOffset++;
			else
				break;
		}

		// 查询是否满足
		if (NULL != (token = strtok((char*)line+nOffset, " \t\n")))
		{
			pid_t tempPid = atoi(token);

			// 匹配到进程的时候
			if(tempPid == pid)
			{
				token = strtok(NULL," \t\n");
				szcpu = atoi(token);
				token = strtok(NULL," \t\n");
				szmem = atoi(token);
				bFind = true;
				//printf("szmem = %s, szcpu = %s\n", szmem,szcpu);
				break;
			}
			else
				continue;
		}
	}
	pclose(fp);
	return bFind;
}


