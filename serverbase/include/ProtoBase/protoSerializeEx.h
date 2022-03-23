/*
 * ==================================================================
 *
 *       Filename:  ProtoSerializeEx.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2015 02:58:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =================================================================
 */

#ifndef _GFX_PROTO_SERIALIZE_FUNC_H_
#define _GFX_PROTO_SERIALIZE_FUNC_H_

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <vector>

#include "BaseCode.h"
#include "protoBase.h"

enum protoSerializeError
{
	protoSucceed 			= 0,
	protoBufferOverflow 	= -1,	// 内存上溢出
	protoBufferUnderflow 	= -2,	// 内存下溢出
	protoBufferEmpty 		= -3
};

// 序列化函数
class CSerializor
{
public:
    CSerializor(void *pData, size_t cbData);
    ~CSerializor();

    template<typename T>
    int32_t serialize(T data)
    {
        if ((m_offset + sizeof(T))> m_cbData)
        {
            return protoBufferOverflow;
        }

        char *pc = (char*)m_pData + m_offset;
        if (sizeof(T)==1 )
        {
            *(T*)pc = data;
        }
        else
        {
            *(T*)pc = CByteOrder::Cast(data);
        }
        m_offset += sizeof(T);
        return protoSucceed;
    }
    template<typename T>
    int32_t serialize_buffer(T* pData, size_t cbData)
    {
        uint16_t len = (uint16_t)cbData;
        if ((m_offset + sizeof(len) + len) > m_cbData)
        {
            return protoBufferOverflow;
        }

        char *pc = (char*)m_pData + m_offset;
        *(uint16_t*)pc = CByteOrder::Cast(len);
        pc += sizeof(len);
        m_offset += sizeof(len);
        if (len > 0)
        {
            memcpy(pc, pData, len);
            m_offset += len;
        }
        return protoSucceed;
    }
    template<typename T>
    int32_t serialize_vector(std::vector<T > &vec)
    {
        typedef typename std::vector<T >::iterator ITERATOR;
        
        if ((m_offset + sizeof(uint32_t) + get_vector_length(vec)) > m_cbData)
        {
            return protoBufferOverflow;
        }

        uint32_t cnt = vec.size();
        serialize(cnt);
        for(ITERATOR it(vec.begin()); it != vec.end(); ++it)
        {
            T &item = *it;
            serialize(item);
        }
        return protoSucceed;
    }

    template<typename T>
    int32_t deserialize(T &data)
    {
        if ((m_offset + sizeof(T))>m_cbData)
        {
            return protoBufferUnderflow;
        }

        char *pc = (char*)m_pData + m_offset;
        if (sizeof(T)==1)
        {
            data = *(T*)pc;
        }
        else
        {
            data = *(T*)pc;
            data = CByteOrder::Cast(data);
        }
        m_offset += sizeof(T);
        return protoSucceed;
    }
    template<typename T>
    int32_t deserialize_buffer(T *pData, size_t &cbData)
    {
        if ((m_offset + sizeof(uint16_t))>m_cbData)
        {
            return protoBufferUnderflow;
        }

        char *pc = (char*)m_pData + m_offset;
        uint16_t len = *(uint16_t*)pc;
        len = CByteOrder::Cast(len);
        len = std::min(len, (uint16_t)cbData); // 取最小值
        if ((m_offset + sizeof(len) + len) > m_cbData)
        {
            return protoBufferUnderflow;
        }
        pc += sizeof(len);
        m_offset += sizeof(len);
        if (len > 0)
        {
            memcpy(pData, pc, len);
            m_offset += len;
        }
        cbData = len;
        return protoSucceed;
    }
    template<typename T>
    int32_t deserialize_vector(std::vector<T > &vec)
    {
        typedef typename std::vector<T >::iterator ITERATOR;
        
        if ((m_offset + sizeof(uint32_t)) > m_cbData)
        {
            return protoBufferOverflow;
        }

        int32_t ret(0);
        uint32_t cnt(0);
        deserialize(cnt);
        for(uint32_t i(0); i<cnt; ++i)
        {
            T item;
            ret = deserialize(item);
            if (ret != protoSucceed) return ret;
            vec.push_back(item);
        }
        return protoSucceed;
    }

public:
    void* data(void) { return m_pData; }
    uint32_t capacity(void) { return (uint32_t)m_cbData; }
    uint32_t offset(void) { return m_offset; }

private:
    template<typename T>
    uint16_t get_vector_length(std::vector<T > &vec)
    {
        typedef typename std::vector<T >::iterator ITERATOR;

        uint16_t len(0);
        for(ITERATOR it(vec.begin()); it != vec.end(); ++it)
        {
            T &item = *it;
            len += item.getLength();
        }
        return len + sizeof(len);
    }

private:
    void*               m_pData;
    const size_t        m_cbData;
    uint32_t            m_offset;
};


class CSerializorEx
{
public:
    explicit CSerializorEx(CByteStream &buff);
    ~CSerializorEx();

    template<typename T>
    void serialize(T data)
    {
        if (sizeof(T)==1)
        {
            m_buffer.Write((const void*)&data, sizeof(T) );
        }
        else
        {
            T data2 = CByteOrder::Cast(data);
            m_buffer.Write((const void*)&data2, sizeof(T));
        }
    }
    template<typename T>
    void serialize_buffer(T* pData, size_t cbData)
    {
        uint16_t len=(uint16_t)cbData;
        if (NULL == pData)
        {
            len = 0;
            m_buffer.Write((const void*)&len, sizeof(len));
        }
        else
        {
            serialize(len);
            m_buffer.Write((const void*)pData, len);
        }
    }
    template<typename T>
    void serialize_vector(std::vector<T > &vec)
    {
        typedef typename std::vector<T >::iterator ITERATOR;

        uint32_t cnt = vec.size();
        serialize(cnt);
        for(ITERATOR it(vec.begin()); it!= vec.end(); ++it)
        {
            T &item = *it;
            serialize(item);
        }
    }

    template<typename T>
    int32_t deserialize(T &data)
    {
        if (sizeof(T) > m_buffer.size())
        {
            return protoBufferUnderflow;
        }
        m_buffer.Read((void*)&data, sizeof(T));
        if(sizeof(T)>1)
        {
            data = CByteOrder::Cast(data);
        }
        return protoSucceed;
    }
    template<typename T>
    int32_t deserialize_buffer(T *pData, size_t &cbData)
    {
        if (cbData==0)
        {
            return protoSucceed;
        }

        uint16_t len(0);
        CByteStreamGuard guard(m_buffer);
        deserialize(len);
        if (len > 0)
        {
            len = std::min(len, (uint16_t)cbData);
            if (!m_buffer.Read((void*)pData, len))
            {
                return protoBufferUnderflow;
            }
            guard.skip(len + sizeof(len));
        }
        return protoSucceed;
    }
    template<typename T>
    int32_t deserialize_vector(std::vector<T > &vec)
    {
        typedef typename std::vector<T >::iterator ITERATOR;

        if (sizeof(uint32_t) > m_buffer.size())
        {
            return protoBufferOverflow;
        }

        int32_t ret(0);
        uint32_t cnt(0);
        CByteStreamGuard guard(m_buffer);
        deserialize(cnt);
        for(uint32_t i(0); i<cnt; ++i)
        {
            T item;
            ret = deserialize(item);
            if (ret != protoSucceed) return ret;
            vec.push_back(item);
        }
        uint16_t len = get_vector_length(vec);
        guard.skip(len);

        return protoSucceed;
    }

private:
    template<typename T>
    uint16_t get_vector_length(std::vector<T > &vec)
    {
        typedef typename std::vector<T >::iterator ITERATOR;

        uint16_t len(0);
        for(ITERATOR it(vec.begin()); it != vec.end(); ++it)
        {
            T &item = *it;
            len += item.getLength();
        }
        return len + sizeof(len);
    }

private:
    CByteStream&    m_buffer;
};

#include "protoSerializeEx.inl"

/*
// 消息编码结束后做校验码生成
void GenerateCheckCode(void *pBuf);
// 收到消息后立即做校验码检查
bool IsCheckCodeValid(void *pBuf);
*/

//////////////////////////////////////////////////////////////////////////////
//常用宏定义

#define EMPTY_STRING(str)	 memset((void*)&str[0], 0, sizeof(str))
#define LENGTH_STR(str)	     sizeof(uint16_t)+strlen((str))

#define INIT_SERIALIZE_ENV   protoLen = this->getProtoLen();\
                             CSerializorEx packer(buff);\
                             packer.serialize(protoLen);\
                             packer.serialize(protoId);\
                             packer.serialize(checkCode);

#define INIT_DESERIALIZE_ENV CSerializorEx packer(buff);\
                             if (!packer.deserialize(protoLen)) return -1;\
                             if (!packer.deserialize(protoId)) return-2;\
                             if (!packer.deserialize(checkCode)) return -3;

#define _STR(T)              #T
#define STR(T)               _STR(T)

/*
// 反序列化协议，
// T      - 协议类名称
// v_buff - 传入的缓冲区
// v_len  - 传入缓冲区的长度
#define DESERIALIZE_REQ(T, v_buff, v_len)   T req; int32_t ret=req.Deserialize(v_buff, v_len); if (ret<0) { \
                                            LogFatal("deserialize %s failure, ret=%d, datalen %u", STR(T) , ret, v_len);\
                                            return -1001; }

// 序列化协议到buff
// T        - 协议的类型，事先定义，并赋值
// v_buff   - 传出的缓冲区类，CTempBuffer
// v_len    - 数据长度
// v_resp   - T 类型的变量
#define SERIALIZE_RESP(T, v_buff, v_len, v_resp)  v_len = v_resp.getProtoLen(); ret = v_resp.Serialize(v_buff.data(), v_len ); \
                                                  if (ret < 0) { \
                                                  LogFatal("serialize %s failure, ret=%d, protoLen=%u", STR(T), ret, v_len);\
                                                  return -1003; }
                                                            
*/

#endif // _GFX_PROTO_SERIALIZE_FUNC_H_

