/*
 * =====================================================================================
 *
 *       Filename:  TypeInfo.h
 *
 *    Description:  对 typeid的一些封装处理
 *
 *        Version:  1.0
 *        Created:  2021年07月28日 14时26分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "MyString.h"
#include <stdio.h>
#include <stdlib.h>
#include <typeinfo>

template<typename T>
bool isChar(T &val)
{
    return typeid(T) == typeid(char);
}
template<typename T>
bool isByte(T &val)
{
    return typeid(T) == typeid(byte_t);
}
template<typename T>
bool isShort(T &val)
{
    return typeid(T) == typeid(int16_t);
}
template<typename T>
bool isUShort(T &val)
{
    return typeid(T) == typeid(uint16_t);
}
template<typename T>
bool isInt32(T &val)
{
    return typeid(T) == typeid(int32_t);
}
template<typename T>
bool isUInt32(T &val)
{
    return typeid(T) == typeid(uint32_t);
}
template<typename T>
bool isInt64(T &val)
{
    return typeid(T) == typeid(int64_t);
}
template<typename T>
bool isUInt64(T &val)
{
    return typeid(T) == typeid(uint64_t);
}
template<typename T>
bool isFloat(T &val)
{
    return typeid(T) == typeid(float);
}
template<typename T>
bool isDouble(T &val)
{
    return typeid(T) == typeid(double);
}
template<typename T>
bool isBoolean(T &val)
{
    return typeid(T) == typeid(bool);
}
template<typename T>
bool isPChar(T &val)
{
    return typeid(T) == typeid(char*);
}
template<typename T>
bool isPChar_C(T &val)
{
    return typeid(T) == typeid(const char*);
}
template<typename T>
bool isMyString(T &val)
{
    return typeid(T) == typeid(CMyString);
}
template<typename T>
bool isPVoid(T &val)
{
    return typeid(T) == typeid(void*);
}
template<typename T>
bool isPVoid_C(T &val)
{
    return typeid(T) == typeid(const void*);
}
template<typename T>
bool isPByte(T &val)
{
    return typeid(T) == typeid(byte_t*);
}
template<typename T>
bool isPByte_C(T &val)
{
    return typeid(T) == typeid(const byte_t*);
}


