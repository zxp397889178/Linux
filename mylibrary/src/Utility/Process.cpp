/*
 * =====================================================================================
 *
 *       Filename:  Process.cpp
 *
 *    Description:  进程相关信息获得
 *
 *        Version:  1.0
 *        Created:  2021年07月07日 10时51分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "BaseCode.h"
#include "MyDB.h"

bool getPath(const CMyString &full_name, CMyString &path_name)
{
    CMyString strCmd = "dirname " + full_name;    
    FILE *cmd_pipe(NULL);
    bool result(true);
    char buffer[1024] = "";
    try{
        do {
            cmd_pipe = popen(strCmd.c_str(), "r");
            if(NULL==cmd_pipe)
            {
                result = false;
                break;
            }
            if (NULL == fgets(buffer, sizeof(buffer), cmd_pipe))
            {
                result = false;
                break;
            }
        }while(false);
    }
    catch(std::exception &ex)
    {
        result = false;
    }
    catch(...)
    {
        result = false;
    }
    if (NULL!= cmd_pipe)
    {
        pclose(cmd_pipe);
        cmd_pipe = NULL;
    }
    if (result)
    {
        path_name = buffer;
        path_name.trim_endl();
    }
    return result;
}

bool getFileName(const CMyString &full_name, CMyString &file_name)
{
    CMyString strCmd = "basename " + full_name;    
    FILE *cmd_pipe(NULL);
    bool result(true);
    char buffer[1024] = "";
    try{
        do {
            cmd_pipe = popen(strCmd.c_str(), "r");
            if(NULL==cmd_pipe)
            {
                result = false;
                break;
            }
            if (NULL == fgets(buffer, sizeof(buffer), cmd_pipe))
            {
                result = false;
                break;
            }
        }while(false);
    }
    catch(std::exception &ex)
    {
        result = false;
    }
    catch(...)
    {
        result = false;
    }
    if (NULL!= cmd_pipe)
    {
        pclose(cmd_pipe);
        cmd_pipe = NULL;
    }
    if (result)
    {
        file_name = buffer;
        file_name.trim_endl();
    }
    return result;
}

bool getProcessNameByPid(pid_t pid, CMyString &name)
{
    //CMyString strCmd("grep -i Name /proc/%d/status | awk -F ' ' '{print $$2}' ");
    CMyString strCmd("grep -i Name /proc/%d/status | awk -F ' ' '{print $2}' ", pid);
    FILE *cmd_pipe(NULL);
    bool result(true);
    char buffer[1024] = "";

    try
    {
        do {
            cmd_pipe = popen(strCmd.c_str(), "r");
            if (NULL == cmd_pipe)
            {
                result = false;
                break;
            }

            if (NULL ==fgets(buffer, sizeof(buffer), cmd_pipe))
            {
                result = false;
                break;
            }
            result = true;
            name = buffer;
            name.trim_endl();
            name.trim();
        }while(0);
    }
    catch(std::exception &ex)
    {
        result = false;
    }
    catch(...)
    {
        result = false;
    }
    if (NULL!=cmd_pipe)
    {
        pclose(cmd_pipe);
        cmd_pipe = NULL;
    }

    return result;
}

bool getProcessInfoByPid(pid_t pid, CMyString &path, CMyString &name)
{
    CMyString strCmd("readlink /proc/%d/exe ", pid);
    FILE *cmd_pipe(NULL);
    bool result(true);
    char buffer[1024] = "";

    try
    {
        do {
            cmd_pipe = popen(strCmd.c_str(), "r");
            if (NULL == cmd_pipe)
            {
                result = false;
                break;
            }

            if (NULL ==fgets(buffer, sizeof(buffer), cmd_pipe))
            {
                result = false;
                break;
            }
            result = true;
        }while(0);
    }
    catch(std::exception &ex)
    {
        result = false;
    }
    catch(...)
    {
        result = false;
    }
    if (NULL!=cmd_pipe)
    {
        pclose(cmd_pipe);
        cmd_pipe = NULL;
    }

    // 对buffer 进行解析，解出文件名和路径
    if (!result)
    {
        return false;
    }
    getPath(buffer, path);
    getFileName(buffer, name);
    return true;

}

int32_t getSiblingProcessPid(const CMyString &name, std::vector<int32_t> &vecSibling)
{
    CMyString strCmd("pgrep %s", name.c_str());
    FILE *cmd_pipe(NULL);
    char buffer[1024] = "";
    char *pc(NULL);
    int32_t result(0);
    pid_t pid = getpid();
    pid_t ppid = getppid();

    try
    {
        do {
            cmd_pipe = popen(strCmd.c_str(), "r");
            if (NULL == cmd_pipe)
            {
                result = -1;
                break;
            }

            do {
                pc = fgets(buffer, sizeof(buffer), cmd_pipe);
                if (NULL == pc)
                {
                    break;
                }
                pid_t pid1 = strtoul(buffer, NULL, 10);
                if (pid1==0 || pid1>=INT_MAX)
                {
                    result = -3;
                    break;
                }

                if (pid1 == ppid || pid1 == pid)
                {
                    continue;
                }
                vecSibling.push_back(pid1);
            }while(true);
        }while(0);
    }
    catch(std::exception &ex)
    {
        result = -5;
    }
    catch(...)
    {
        result = -7;
    }
    if (NULL!=cmd_pipe)
    {
        pclose(cmd_pipe);
        cmd_pipe = NULL;
    }
    if (result == 0)
    {
        result = (int32_t)vecSibling.size();
    }

    return result;
}

int32_t getChildProcessPid(const CMyString &name, std::vector<int32_t> &vecChild)
{
    CMyString strCmd("pgrep %s", name.c_str());
    FILE *cmd_pipe(NULL);
    char buffer[1024] = "";
    char *pc(NULL);
    int32_t result(0);
    pid_t pid = getpid();

    try
    {
        do {
            cmd_pipe = popen(strCmd.c_str(), "r");
            if (NULL == cmd_pipe)
            {
                result = -1;
                break;
            }

            do {
                pc = fgets(buffer, sizeof(buffer), cmd_pipe);
                if (NULL == pc)
                {
                    break;
                }
                pid_t pid1 = strtoul(buffer, NULL, 10);
                if (pid1==0 || pid1>=INT_MAX)
                {
                    result = -3;
                    break;
                }

                if (pid1 == pid)
                {
                    // 忽略自身的pid
                    continue;
                }
                vecChild.push_back(pid1);
            }while(true);
        }while(0);
    }
    catch(std::exception &ex)
    {
        result = -5;
    }
    catch(...)
    {
        result = -7;
    }
    if (NULL!=cmd_pipe)
    {
        pclose(cmd_pipe);
        cmd_pipe = NULL;
    }
    if (result == 0)
    {
        result = (int32_t)vecChild.size();
    }

    return result;
}

int32_t BroadcastCmdToSibling(CMyRedis &redis, const CMyString &keyFormat, 
        int32_t cmd, Json::Value &data, CMyString &strNote)
{
    // 获得父进程ID，父进程下的子进程ID
    pid_t ppid = getppid();
    pid_t pid = getpid();
    LogDebug("RELOAD>process id:%d, parent process id:%d", pid, ppid);
    CMyString proc_name;
    if (!getProcessNameByPid(pid, proc_name))
    {
        strNote.Format("get process_name by pid failure, pid:%d", pid);
        return -1;
    }
    LogDebug("RELOAD>process name: [%s]", proc_name.c_str());
    std::vector<int32_t> sibling_pid;
    int32_t cnt = getSiblingProcessPid(proc_name, sibling_pid);
    if (cnt<0)
    {
        strNote.Format("get sibling process failure, pid:%d, name:%s", pid, proc_name.c_str());
        return -3;
    }
    else if(cnt==0)
    {
        strNote.Format("not any sibling process, pid:%d, name:%s", pid, proc_name.c_str());
        return 0;
    }

    cnt = 0;
    Json::Value req_data;
    req_data["timestamp"] = (Json::Value::Int64)time(NULL);
    req_data["caller"] = pid;
    req_data["command"] = cmd;
    req_data["data"] = data;
    CJsonWrapper wr;
    wr.Load(req_data);
    CMyString strData = wr.toString();
    for(std::vector<int32_t>::iterator it(sibling_pid.begin());
            it != sibling_pid.end(); ++it) 
    {
        // 逐个进程发送消息,采用queue的格式
        pid_t pid1 = *it;
        CMyString strKey(keyFormat.c_str(), pid1); 
        int32_t ret = redis.LPush(strKey, strData);
        if (ret==1)
        {
            strNote.Format("RELOAD>send request to process %d succeed.", pid1);
            ++cnt;
        }
        else
        {
            strNote.Format("RELOAD>send request to process %d failure.", pid1);
        }
    }

    return cnt;
}

