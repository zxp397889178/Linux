/*
 * =====================================================================================
 *
 *       Filename:  Process.h
 *
 *    Description:  进程处理函数
 *
 *        Version:  1.0
 *        Created:  2021年07月07日 14时00分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"


bool getPath(const CMyString &full_name, CMyString &path_name);
bool getFileName(const CMyString &full_name, CMyString &file_name);
/* 通过进程ID获得进程的名字
 * 返回：false 表示无此进程id 或出错
 */
bool getProcessNameByPid(pid_t pid, CMyString &name);
bool getProcessInfoByPid(pid_t pid, CMyString &path, CMyString &name);

/* 通过进程名，获得兄弟进程的id列表，不含父进程和自身
 * 返回值：<0  出错
 *         >=0 兄弟进程的数量
 */
int32_t getSiblingProcessPid(const CMyString &name, std::vector<int32_t> &vecSibling);
/* 通过进程名，获得子进程的PID列表
 * 返回值: <0  出错
 *         >=0 子百程的数量
 */
int32_t getChildProcessPid(const CMyString &name, std::vector<int32_t> &vecChild);


/* 向兄弟进程发送消息
 * 参数: [IN]   redis       数据缓存
 *       [IN]   keyFormat   redis存储的键值格式,必须包含pid，如 test_process_msg_%d
 *       [IN]   cmd         命令，自定义
 *       [IN]   data        详细命令参数
 *       [OUT]  strNote     操作结果描述，返回值<0时为错误说明
 * 返回值：>=0 发送到的兄弟进程数量
 *         <0  错误
 */

int32_t BroadcastCmdToSibling(CMyRedis &redis, const CMyString &keyFormat, 
        int32_t cmd, Json::Value &data, CMyString &strNote);


