/*
 * =====================================================================================
 *
 *       Filename:  BaseCode.h
 *
 *    Description:  common define of BaseCode library
 *
 *        Version:  1.0
 *        Created:  2014年12月02日 17时25分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com(IGG)
 *
 * =====================================================================================
 */

#ifndef _MYLIB_BASECODE_BASECODE_H_
#define _MYLIB_BASECODE_BASECODE_H_

#include "BaseCode/mydef.h"
#include "BaseCode/myconf.h"
#include "BaseCode/ScopedBuffer.hpp"

#include "BaseCode/IException.h"
#include "BaseCode/ILock.h"
#include "BaseCode/IThreadEvent.h"

#include "BaseCode/StringFunc.h"
#include "BaseCode/TimeFunc.h"

#include "BaseCode/MyString.h"
#include "BaseCode/ByteStreamEx.h"
#include "BaseCode/ByteStream.h"
#include "BaseCode/MyDate.h"
#include "BaseCode/MyDateTime.h"
#include "BaseCode/MyTimeDelta.h"
#include "BaseCode/MyTimer.h"
#include "BaseCode/ShareMem.h"
#include "BaseCode/ShmDataQueue.h"
#include "BaseCode/MemFile.h"
#include "BaseCode/MyThread.h"
#include "BaseCode/ByteOrder.h"
#include "BaseCode/JsonWrapper.h"
// #include "BaseCode/JsonReader.h"
// #include "BaseCode/JsonWriter.h"

#ifndef LOG_WITH_SHMFILE
#	define LOG_WITH_SHMFILE
#endif
//#include "BaseCode/LogWrapper.h"
#include "BaseCode/iLog.h"
#include "BaseCode/LogShmWrapper.h"

#include "BaseCode/FileInfo.h"
#include "BaseCode/Misc.h"
#include "BaseCode/Codec.h"

#include "BaseCode/MyIni.h"
#include "BaseCode/XmlFile.h"

#include "BaseCode/ObjectPtrGuard.hpp"

#include "BaseCode/proc_stat.h"
#include "BaseCode/SysInfoFunc.h"

#include "BaseCode/Compress.h"
#include "BaseCode/StrSpliter.h"

#include "BaseCode/TypeInfo.h"

#endif  //_MYLIB_BASECODE_BASECODE_H_
