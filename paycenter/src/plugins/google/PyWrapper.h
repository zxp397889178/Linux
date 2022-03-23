/*
 * =====================================================================================
 *
 *       Filename:  PyWrapper.h
 *
 *    Description:  C++ 调用Python 脚本定义
 *
 *        Version:  1.0
 *        Created:  2020年10月27日 16时21分46秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string>
#include <vector>
#include "BaseCode.h"

// https://blog.csdn.net/vampirem/article/details/12948955
/* strType支持以下类型
 *     s -- 字符串
 *     c -- 单个字符
 *     z -- 字符串，同 s
 *     b -- 整型，同 i
 *     h -- 短整型，同 i
 *     i -- 整型
 *     l -- 长整型
 *     d -- double型
 *     f -- 浮点型，同d
 * 暂不支持O& -对象, []-列表, {}-字典, ()-元组 转换
 */
struct PyParam
{
    std::string     strType;  
    std::string     value;
    PyParam(const std::string &_type, const std::string &_val)
        : strType(_type), value(_val)
    {
    }
};
class CPyWrapper
{
public:
    CPyWrapper();
    ~CPyWrapper();

    bool Init(const CMyString &pyHome);

    int LoadScript(const std::string &script);

    int CallFunc(const std::string &func);
    int CallFunc(const std::string &func, std::vector<PyParam> &vecParam, PyObject* &result);

    int getResult(PyObject *pResult, int &val);
    int getResult(PyObject *pResult, std::string &val);

private:
    PyObject*   m_pModule;
    PyObject*   m_pDict;

    bool        m_bInited;
};



