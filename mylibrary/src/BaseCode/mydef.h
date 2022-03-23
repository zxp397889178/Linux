/*
 * =====================================================================================
 *
 *       Filename:  type_def.h
 *
 *    Description:  common type defined
 *
 *        Version:  1.0
 *        Created:  2014年11月26日 17时11分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#pragma once
#ifndef _MYLIB_BASECODE_TYPE_DEF_H_
#define _MYLIB_BASECODE_TYPE_DEF_H_

#include <stdint.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>

#include "platform.h"
#include "color.h"

// typedef char					int8_t;
// typedef short				int16_t;
// typedef int					int32_t;
// typedef long long			int64_t;
// typedef unsigned char		uint8_t;
// typedef unsigned short		uint16_t;
// typedef unsigned int			uint32_t;
// typedef unsigned long int 		uint64_t;

typedef uint8_t					byte_t;


#ifndef todo
#	define __STR2__(x)	#x
#	define __STR1__(x)	__STR2__(x)
#	define __LOC__		__FILE__ "(" __STR1__ ( __LINE__ )") : Warning Msg: "
#	define __LOC2__		__FILE__ "(" __STR1__ ( __LINE__ )") : <TODO> "
#	define todo(str)	message ( __LOC2__ str)
#endif

#ifndef SAFE_DELETE
#	define SAFE_DELETE(ptr)			if (ptr!=NULL) { delete ptr; ptr=NULL; }
#	define SAFE_DELETE_ARRAY(ptr)	if (ptr!=NULL) { delete[] ptr; ptr=NULL; }
#	define SAFE_FREE(ptr)			if (ptr!=NULL) { free(ptr); ptr=NULL; }
#	define SAFE_RELEASE(ptr)		if (ptr!=NULL) { ptr->release(); ptr=NULL; }
#endif

#ifndef HAS_PROPERTY
#	define HAS_PROPERTY(prop, val)		(((prop) & (val)) == (prop))
#endif
#ifndef SET_PROPERTY
#	define SET_PROPERTY(prop, val)		((val)|=(prop))
#endif
#ifndef SETBIT
#	define SETBIT(val, b)				((val) |= (1<<(b)))
#endif
#ifndef CLEARBIT
#	define CLEARBIT(val, b)				((val) &= (~(1<<(b))) )
#endif
#ifndef ISBITSET
#	define ISBITSET(val, b)				(( (val)&(1<<(b))) != 0)
#endif
#ifndef UNUSED
#	ifdef __GNUC__
#		define UNUSED(arg)
#	else
#		define UNUSED(arg)					(arg)
#	endif
#endif

#ifndef ASSERT
#	define ASSERT(val)					
#endif

#ifndef DB_ASSERT
#	define DB_ASSERT(val)
#endif

//计算按 align 对齐后的 len 的值
#define BUF_ALIGN(len, align)		( ((len)+(align)-1) & ~ ((align)-1) )

// 一些指数值定义
#define E0		1
#define E1		10
#define E2		100
#define E3		1000
#define E4		10000
#define E5		100000
#define E6		1000000
#define E7		10000000
#define E8		100000000
#define E9		1000000000
#define E10		10000000000
#define E11		100000000000
#define E12		1000000000000

#if defined(__GNUC__) // not clang (gcc comes later since clang emulates gcc) 
#   if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#       define DEPRECATED(message) __attribute__((deprecated(message)))
#   elif (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#       define DEPRECATED(message) __attribute__((__deprecated__))
#   endif
#elif defined(_MSC_VER) // MSVC (after clang because clang on Windows emulates )
#   define DEPRECATED(message) __declspec(deprecated(message))
#endif

#if !defined(DEPRECATED)
#   define DEPRECATED(message)
#endif // if !defined(JSONCPP_DEPRECATED)

#endif //_MYLIB_BASECODE_TYPE_DEF_H_



