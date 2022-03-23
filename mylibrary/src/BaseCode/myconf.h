/*
 * =====================================================================================
 *
 *       Filename:  config.h
 *
 *    Description:  config of BaseCode library
 *
 *        Version:  1.0
 *        Created:  2014/11/10 21时25分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_CONFIG_H_
#define _MYLIB_BASECODE_CONFIG_H_

#include <exception>
#include <stdexcept>
#include "mydef.h"

//#define USE_EXCEPTION  // use try..except, in function, it may call throw
//define in Makefile


#ifdef USE_EXCEPTION
#	define THROW(_cls, str)		throw _cls(str)
#	define THROW_STD(_cls, str)	throw std::_cls(str)
#	define THROW_EX(_cls)		throw _cls
#else
#	define THROW(_cls, str)
#	define THROW_STD(_cls, str)
#	define THROW_EX(_cls)
#endif



#endif // _MYLIB_BASECODE_CONFIG_H_

