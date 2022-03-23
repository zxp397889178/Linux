/*
 * =====================================================================================
 *
 *       Filename:  PyWrapper.cpp
 *
 *    Description:  C++调用Python脚本
 *
 *        Version:  1.0
 *        Created:  2020年10月27日 16时22分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "PyWrapper.h"
#include <sstream>
#include "BaseCode.h"
#include "BaseCode/Misc.h"

double stod(std::string &str)
{
    double val(0);
    std::stringstream ss;
    ss << str;
    ss >> val;
    return val;
}

CPyWrapper::CPyWrapper()
    : m_pModule(NULL)
    , m_pDict(NULL)
    , m_bInited(false)
{
}

CPyWrapper::~CPyWrapper()
{
    Py_XDECREF(m_pDict);
    Py_XDECREF(m_pModule);
    Py_Finalize();
}

bool CPyWrapper::Init(const CMyString &pyHome)
{
    if (!Py_IsInitialized())
    {
        Py_SetPythonHome(L"/opt/python3");
        Py_Initialize();
        // 初始化调用失败
        //return false;
    }

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./py')");
    //PyRun_SimpleString("print(sys.path)");

    m_bInited = true;
    return true;
}

#define MAX_SIZE 256
int CPyWrapper::LoadScript(const std::string &script)
{
    if (!m_bInited)
    {
        // python 初始化失败
        return -1;
    }
    Py_XDECREF(m_pDict);
    Py_XDECREF(m_pModule);

    char current_absolute_path[MAX_SIZE];
    int cnt = readlink("/proc/self/exe", current_absolute_path, MAX_SIZE);
    if (cnt < 0 || cnt >= MAX_SIZE)
    {
        return -2;
    }
    for (int i=cnt; i>=0; --i)
    {
        if (current_absolute_path[i] == '/')
        {
            current_absolute_path[i+1] = '\0';
            break;
        }
    }
    LogWarn("current path is [%s]", current_absolute_path);
    CMyString str1("sys.path.append('%spy')", current_absolute_path);
    PyRun_SimpleString(str1.c_str());
    PyRun_SimpleString("print(sys.path)");

    m_pModule = PyImport_ImportModule(script.c_str());
    if (NULL == m_pModule)
    {
        return -3;
    }

    m_pDict = PyModule_GetDict(m_pModule);
    if (NULL == m_pDict)
    {
        return -5;
    }

    return 0;
}

int CPyWrapper::CallFunc(const std::string &func, std::vector<PyParam> &vecParam, PyObject* &result)
{
    if (!m_bInited || NULL == m_pModule || NULL == m_pDict)
    {
        LogErr("python env init fail.");
        return -1;
    }

    LogDebug("begin to call python function [%s], param count:%lu", func.c_str(), vecParam.size());
    result = NULL;
#if 0
    PyObject* pFunc = PyDict_GetItemString(m_pDict, func.c_str());
    if (NULL == pFunc || !PyCallable_Check(pFunc))
    {
        return -3;
    }
#else
    PyObject* pFunc = PyObject_GetAttrString(m_pModule, func.c_str());
    if (NULL==pFunc || !PyCallable_Check(pFunc))
    {
        LogErr("not found function %s in script.", func.c_str());
        return -3;
    }
#endif
    if (vecParam.empty())
    {
        result = PyObject_CallObject(pFunc, NULL);
        Py_DECREF(pFunc);
        return 0;
    }

    // 创建参数
    PyObject *pArgs = PyTuple_New(vecParam.size()); // 函数调用的参数传递均是以元组的形式
    int arg_no(0);
    for(std::vector<PyParam>::iterator it(vecParam.begin());
            it != vecParam.end(); ++it)
    {
        PyParam &param = *it;
        char c=param.strType[0];
        c = tolower(c);
        PyObject *pArgVal;
        if (c=='s' || c=='c' || c=='z')
        {
            pArgVal = Py_BuildValue(param.strType.c_str(), param.value.c_str());
            LogDebug("python param %d => str: [%s]", arg_no, param.value.c_str());
        }
        else if (c=='b' || c=='h' || c=='i')
        {
            char *endptr;
            pArgVal = Py_BuildValue(param.strType.c_str(), strtol(param.value.c_str(), &endptr, 10));
            LogDebug("python param %d => int: [%s]", arg_no, param.value.c_str());
        }
        else if (c=='l')
        {
            char *endptr;
            pArgVal = Py_BuildValue(param.strType.c_str(), strtoll(param.value.c_str(), &endptr, 10));
            LogDebug("python param %d => int64: [%s]", arg_no, param.value.c_str());
        }
        else if (c=='f' || c=='d')
        {
            pArgVal = Py_BuildValue(param.strType.c_str(), stod(param.value));
            LogDebug("python param %d => double: [%s]", arg_no, param.value.c_str());
        }
        else
        {
            LogErr("python param %d => type[%s], value[%s] INVALID", arg_no, param.strType.c_str(), param.value.c_str());
            // 参数不合法
            Py_DECREF(pArgs);
            Py_DECREF(pFunc);
            return -4;
        }
        if (pArgVal == NULL)
        {
            LogErr("call Py_BuildValue failure, arg no:%d", arg_no);
            // 参数转换失败
            Py_DECREF(pArgs);
            Py_DECREF(pFunc);
            return -5;
        }
        // 若失败时可试着用 PyTuple_SET_ITEM
        int ret = PyTuple_SetItem(pArgs, arg_no, pArgVal); 
        if (ret != 0)
        {
            LogErr("call PyTuple_SetItem failure, ret=%d", ret);
            // 参数设置失败
            Py_DECREF(pArgs);
            Py_DECREF(pFunc);
            return -7;
        }
        ++arg_no;
    }
#if 0
    result = PyEval_CallObject(pFunc, pArgs);
#else
    result = PyObject_CallObject(pFunc, pArgs);
#endif
    if (NULL == result)
    {
        LogErr("call python function [%s] failure.", func.c_str());
        return -9;
    }

    LogDebug("call python function %s succeed.", func.c_str());
    // printf("result=%p\n", result);
    return 0;
}

int CPyWrapper::CallFunc(const std::string &func)
{
    std::vector<PyParam> vecParam;
    PyObject *result = NULL;
    int ret = CallFunc(func, vecParam, result);
    if (ret < 0)
    {
        return ret;
    }
    
    if (NULL == result)
    {
        return 0;
    }

    int val(0);
    //i表示转换成int型变量。在这里，最需要注意的是：PyArg_Parse的最后一个参数，必须加上“&”符号
    ret = PyArg_Parse(result, "i", &val);
    if (ret != 0)
    {
        Py_DECREF(result);
        return -11;
    }
    Py_DECREF(result);
    return val;
}

int CPyWrapper::getResult(PyObject *pResult, int &val)
{
    if (NULL == pResult)
    {
        return -1;
    }
    int ret = PyArg_Parse(pResult, "i", &val);
    if (ret < 0)
    {
        Py_DECREF(pResult);
        return -3;
    }
    Py_DECREF(pResult);
    return 0;
}

int CPyWrapper::getResult(PyObject *pResult, std::string &val)
{
    if (NULL == pResult)
    {
        return -1;
    }
    char *str;
    int ret = PyArg_Parse(pResult, "s", &str);
    if (ret < 0)
    {
        Py_DECREF(pResult);
        return -3;
    }

    val = str;
    Py_DECREF(pResult);
    Py_DECREF(str);
    return 0;
}



