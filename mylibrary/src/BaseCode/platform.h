/*
 * =====================================================================================
 *
 *       Filename:  platform.h
 *
 *    Description: 定义平台的宏 
 *
 *        Version:  1.0
 *        Created:  2015年07月30日 16时01分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_BASELIB_PLATFORM_H_
#define _QW_BASELIB_PLATFORM_H_

// http://gobismoon.blog.163.com/blog/static/52442802201202714332680/
// 各平台宏定义
#if defined(__APPLE__) && defined(__GNUC__)
#	define OS_MACX
#	define OS_MAC
#elif defined(__MACOSX__)
#	define OS_MACX
#	define OS_MAC
#elif defined(macintosh)
#	define OS_MAC9
#elif defined(__CYGWIN__)
#	define OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#	define OS_MSDOS
#elif defined(__OS2__)
#	if defined(__EMX__)
#		define OS_OS2EMX
#	else
#		define OS_OS2
#	endif
#elif !defined(SAG_COM) && (defined(WIN64)||defined(_WIN64)||defined(__WIN64__))
#	define OS_WIN64
#	define OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#	define OS_WIN32
#elif defined(__sun)||defined(sun)
#	define OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#	define OS_HPUX 
#elif defined(__ultrix) || defined(ultrix) 
#	define OS_ULTRIX 
#elif defined(sinix) 
#	define OS_RELIANT 
#elif defined(__linux__) || defined(__linux) 
#	define OS_LINUX 
#elif defined(__FreeBSD__) 
#	define OS_FREEBSD 
#	define OS_BSD4 
#elif defined(__NetBSD__) 
#	define OS_NETBSD 
#	define OS_BSD4 
#elif defined(__OpenBSD__) 
#	define OS_OPENBSD 
#	define OS_BSD4 
#elif defined(__bsdi__) 
#	define OS_BSDI 
#	define OS_BSD4 
#elif defined(__sgi) 
#	define OS_IRIX 
#elif defined(__osf__) 
#	define OS_OSF 
#elif defined(_AIX) 
#	define OS_AIX 
#elif defined(__Lynx__) 
#	define OS_LYNX 
#elif defined(__GNU_HURD__) 
#	define OS_HURD 
#elif defined(__DGUX__) 
#	define OS_DGUX 
#elif defined(__QNXNTO__) 
#	define OS_QNX6 
#elif defined(__QNX__) 
#	define OS_QNX 
#elif defined(_SEQUENT_) 
#	define OS_DYNIX 
#elif defined(_SCO_DS) /*  SCO OpenServer 5 + GCC */ 
#	define OS_SCO 
#elif defined(__USLC__) /*  all SCO platforms + UDK or OUDK */ 
#	define OS_UNIXWARE 
#	define OS_UNIXWARE7 
#elif defined(__svr4__) && defined(i386) /*  Open UNIX 8 + GCC */ 
#	define OS_UNIXWARE 
#	define OS_UNIXWARE7 
#else 
#	error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com" 
#endif 


#endif	// _QW_BASELIB_PLATFORM_H_
