#ifndef _MYLIB_BASECODE_SYSINFO_FUNCTION_H_
#define _MYLIB_BASECODE_SYSINFO_FUNCTION_H_

#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdint.h>
/*
 * 输入：
 * pid --- 进程ID
 * 输出：
 * szcpu --- 使用 CPU资源百分比
 * szmem --- 使用虚拟内存量 (Kbytes)
 * 函数说明：
 * 	获取进程pid为输入pid使用的CPU百分比和内存量
 * */
bool GetCpuAndMem(pid_t pid, char *szcpu, char *szmem);

bool GetCpuAndMem(pid_t pid,uint32_t& szcpu, uint32_t& szmem);

/*
 * 输入：
 * pids --- 进程id列表
 * 输出：
 * szcpu --- 使用 CPU资源百分比总和
 * szmem --- 使用虚拟内存量 (Kbytes)总和
 * 函数说明：
 * 	获取列表的进程使用CPU百分比和内存量的总和
 * */
bool GetCpuAndMem(std::vector<pid_t>& pids,char *szcpu, char *szmem);

/*
 * 输出：
 * szcpu --- 当前进程使用 CPU资源百分比
 * szmem --- 当前进程使用虚拟内存量 (Kbytes)
 * 函数说明：
 * 	获取当前进程使用的CPU百分比和内存量
 * */
bool GetCpuAndMem(char *szcpu, char *szmem);

/*
 * 输出：
 * szcpu --- 使用 CPU资源百分比总和
 * szmem --- 使用虚拟内存量 (Kbytes)总和
 * 函数说明：
 * 	获取和当前进程一样进程名的进程使用的CPU百分比和内存量总和
 * */
bool GetTotalCpuAndMem(char *szcpu, char *szmem);
/*
 * 输入：
 * pid --- 进程id
 * 输出：
 * execparam --- 进程的命令行参数
 * 函数说明：
 * 	获取进程的命令行参数
 * */
bool GetCurExecParam(pid_t pid,char* execparam);
/*
 * 输入：
 * execparam --- 进程的命令行参数
 * 输出：
 * szcpu --- 使用 CPU资源百分比总和
 * szmem --- 使用虚拟内存量 (Kbytes)总和
 * 函数说明：
 * 	获取进程的命令行参数一样的进程使用CPU百分比和内存量的总和
 * */
bool GetCpuAndMem(char* execparam,char *szcpu, char *szmem);

#endif // _MYLIB_BASECODE_SYSINFO_FUNCTION_H_
