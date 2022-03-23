/*
 * =====================================================================================
 *
 *       Filename:  MsgBuffer.h
 *
 *    Description:  用于交换消息的缓存类
 *
 *        Version:  1.0
 *        Created:  2021年11月13日 16时39分31秒
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

class CMsgBuffer
{
 public:
     CMsgBuffer();
     CMsgBuffer(bool bReleaseManual, uint32_t len);
     CMsgBuffer(void* pData, uint32_t len);
     ~CMsgBuffer();

     void* Allocate(uint32_t len);
     void Release(void);
     void SetReleaseManual(bool val) { m_bReleaseManual = val; }

     char*    Str(void)    { return (char*)m_pBuffer; }
     byte_t*  Data(void)   { return (byte_t*)m_pBuffer; }
     void*    Buffer(void) { return m_pBuffer; }
     uint32_t Length(void) { return m_cbBuffer; }

 private:
     void*      m_pBuffer;
     uint32_t   m_cbBuffer;

     bool       m_bReleaseManual; // true时需要手工释放内存
};

