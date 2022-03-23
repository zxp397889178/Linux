/*
 * =====================================================================================
 *
 *       Filename:  protoSerializeEx.inl
 *
 *    Description:  包含文件，用于定义扩展的模板信息
 *
 *        Version:  1.0
 *        Created:  2019年09月20日 15时21分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * 参考URL：http://my.oschina.net/shewa/blog/206124
 * 必须显式的实例化类型，否则会链接出错
 * =====================================================================================
 */

/*
template class TSerializor<uint16_t>;
template class TSerializor<uint32_t>;
template class TSerializor<uint64_t>;
template class TSerializor<CMyString>;
*/

#pragma once

template<> inline
int32_t CSerializor::serialize<float>(float data)
{
    if ((m_offset + sizeof(float))> m_cbData)
    {
        return protoBufferOverflow;
    }
    char *pc = (char*)m_pData + m_offset;
    *(float*)pc = data;
    m_offset += sizeof(float);
    return protoSucceed;
}

template<> inline
int32_t CSerializor::serialize<double>(double data)
{
    if ((m_offset + sizeof(double))> m_cbData)
    {
        return protoBufferOverflow;
    }
    char *pc = (char*)m_pData + m_offset;
    *(double*)pc = data;
    m_offset += sizeof(double);
    return protoSucceed;
}

template<> inline
int32_t CSerializor::serialize<CMyString>(CMyString data)
{
    uint16_t len = (uint16_t)data.size();
    if ((m_offset + sizeof(len) + len )>m_cbData)
    {
        return protoBufferOverflow;
    }

    char *pc = (char*)m_pData + m_offset;
    *(uint16_t*)pc = CByteOrder::Cast(len);
    pc += sizeof(len);
    m_offset += sizeof(len);
    if (len > 0)
    {
        memcpy(pc, data.c_str(), len);
        m_offset += len;
    }
    return protoSucceed;
}

template<> inline
int32_t CSerializor::serialize<char*>(char *data)
{
    uint16_t len = (data==NULL) ? 0 : strlen(data);
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
        memcpy(pc, data, len);
        m_offset += len;
    }
    return protoSucceed;
}

template<> inline
int32_t CSerializor::serialize_buffer<FILE>(FILE *fp, size_t cbData)
{
    uint32_t pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    uint32_t last = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    cbData = last - pos;

    uint16_t len = (uint16_t)cbData;
    if ((m_offset + sizeof(len) + len)>m_cbData)
    {
        return protoBufferOverflow;
    }

    char *pc = (char*)m_pData + m_offset;
    *(uint16_t*)pc = CByteOrder::Cast(len);
    pc += sizeof(len);
    m_offset += sizeof(len);

    if (len > 0)
    {
        fread(pc, 1, len, fp);
        m_offset += len;
    }

    return protoSucceed;
}

template<> inline
int32_t CSerializor::deserialize<float>(float &data)
{
    if ((m_offset + sizeof(float))>m_cbData)
    {
        return protoBufferUnderflow;
    }

    char *pc = (char*)m_pData + m_offset;
    data = *(float*)pc;
    m_offset += sizeof(float);
    return protoSucceed;
}

template<> inline
int32_t CSerializor::deserialize<double>(double &data)
{
    if ((m_offset + sizeof(double))>m_cbData)
    {
        return protoBufferUnderflow;
    }

    char *pc = (char*)m_pData + m_offset;
    data = *(double*)pc;
    m_offset += sizeof(double);
    return protoSucceed;
}

template<> inline
int32_t CSerializor::deserialize<CMyString>(CMyString &data)
{
    if ((m_offset + sizeof(uint16_t))> m_cbData)
    {
        return protoBufferUnderflow;
    }

    char *pc = (char*)m_pData + m_offset;
    uint16_t len = *(uint16_t*)pc;
    len = CByteOrder::Cast(len);
    if ((m_offset + sizeof(len) + len)>m_cbData)
    {
        return protoBufferUnderflow;
    }
    pc += sizeof(len);
    m_offset += sizeof(len);
    if (len > 0)
    {
        data.assign(pc, len);
        m_offset += len;
    }
    return protoSucceed;
}

template<> inline
int32_t CSerializor::deserialize_buffer<FILE>(FILE *fp, size_t &cbData)
{
   if ((m_offset + sizeof(uint16_t))>m_cbData)
    {
        return protoBufferUnderflow;
    }

    char *pc = (char*)m_pData + m_offset;
    uint16_t len = *(uint16_t*)pc;
    len = CByteOrder::Cast(len);
    if ((m_offset + sizeof(len) + len) > m_cbData)
    {
        return protoBufferUnderflow;
    }
    pc += sizeof(len);
    m_offset += sizeof(len);
    if (len > 0)
    {
        fwrite(pc, 1, len, fp);
        fflush(fp);
        m_offset += len;
    }
    cbData = len;
    return protoSucceed;
}

template<> inline
uint16_t CSerializor::get_vector_length<uint32_t>(std::vector<uint32_t> &vec)
{
    return (uint16_t)(vec.size()*sizeof(uint32_t) + sizeof(uint32_t));
}

template<> inline
uint16_t CSerializor::get_vector_length<CMyString>(std::vector<CMyString> &vec)
{
    uint16_t len(0);
    std::vector<CMyString>::iterator it(vec.begin());
    for(; it != vec.end(); ++it)
    {
        len += sizeof(uint16_t);
        len += it->length();
    }
    return len + sizeof(len);
}


/////////////////////////////////////////////////////////////////////
//


template<> inline
void CSerializorEx::serialize<float>(float data)
{
    m_buffer.Write((const void*)&data, sizeof(float));
}

template<> inline
void CSerializorEx::serialize<double>(double data)
{
    m_buffer.Write((const void*)&data, sizeof(double));
}

template<> inline
void CSerializorEx::serialize<CMyString>(CMyString data)
{
    uint16_t len = (uint16_t)data.size();
    uint16_t len2 = CByteOrder::Cast(len);
    m_buffer.Write((const void*)&len2, sizeof(len2));
    m_buffer.Write((const void*)data.c_str(), len);
}

template<> inline
void CSerializorEx::serialize<CMyString&>(CMyString& data)
{
    uint16_t len = (uint16_t)data.size();
    uint16_t len2 = CByteOrder::Cast(len);
    m_buffer.Write((const void*)&len2, sizeof(len2));
    m_buffer.Write((const void*)data.c_str(), len);
}

template<> inline
void CSerializorEx::serialize<const CMyString&>(const CMyString& data)
{
    uint16_t len = (uint16_t)data.size();
    uint16_t len2 = CByteOrder::Cast(len);
    m_buffer.Write((const void*)&len2, sizeof(len2));
    m_buffer.Write((const void*)data.c_str(), len);
}

template<> inline
void CSerializorEx::serialize<char*>(char *data)
{
    uint16_t len = (NULL==data) ? 0 : strlen(data);
    uint16_t len2 = CByteOrder::Cast(len);
    m_buffer.Write((const void*)&len2, sizeof(len2));
    if (NULL != data)
    {
        m_buffer.Write((const void*)data, len);
    }
}

template<> inline
void CSerializorEx::serialize<const char*>(const char *data)
{
    uint16_t len = (NULL==data) ? 0 : strlen(data);
    uint16_t len2 = CByteOrder::Cast(len);
    m_buffer.Write((const void*)&len2, sizeof(len2));
    if (NULL != data)
    {
        m_buffer.Write((const void*)data, len);
    }
}

template<> inline
void CSerializorEx::serialize_buffer<FILE>(FILE *fp, size_t cbData)
{
    uint32_t pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    uint32_t last = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    cbData = last - pos;

    uint16_t len = (uint16_t)cbData;
    uint16_t len2 = CByteOrder::Cast(len);
    m_buffer.Write((const void*)&len2, sizeof(len2));
    if (len > 0)
    {
        CTempBuffer tmp(len);
        fread(tmp.data(), 1, len, fp);
        m_buffer.Write((const void*)tmp.data(), len);
    }
}

template<> inline
int32_t CSerializorEx::deserialize<float>(float &data)
{
    if (sizeof(float) > m_buffer.size())
    {
        return protoBufferUnderflow;
    }
    m_buffer.Read((void*)&data, sizeof(float));
    return protoSucceed;
}

template<> inline
int32_t CSerializorEx::deserialize<double>(double &data)
{
    if (sizeof(double) > m_buffer.size())
    {
        return protoBufferUnderflow;
    }
    m_buffer.Read((void*)&data, sizeof(double));
    return protoSucceed;
}

template<> inline
int32_t CSerializorEx::deserialize<CMyString>(CMyString &data)
{
    if (sizeof(uint16_t) > m_buffer.size())
    {
        return protoBufferUnderflow;
    }

    uint16_t len(0);
    CByteStreamGuard guard(m_buffer);
    if(!m_buffer.Read((void*)&len, sizeof(len)))
    {
        return protoBufferUnderflow;
    }
    len = CByteOrder::Cast(len);

    CMyString temp;
    if (!m_buffer.Read(temp, len))
    {
        return protoBufferUnderflow;
    }
    guard.skip(len + sizeof(len));
    data = temp;
    return protoSucceed;
}

template<> inline
int32_t CSerializorEx::deserialize_buffer<FILE>(FILE *fp, size_t &cbData)
{
   if (sizeof(uint16_t)>m_buffer.size())
    {
        return protoBufferUnderflow;
    }

    uint16_t len(0);
    CByteStreamGuard guard(m_buffer);
    deserialize(len);
    if (len > 0)
    {
        CTempBuffer tmpbuf(len);
        if (!m_buffer.Read(tmpbuf.data(), len))
        {
            return protoBufferUnderflow;
        }
        fwrite(tmpbuf.data(), 1, len, fp);
        fflush(fp);

    }
    cbData = len;
    guard.skip(len+sizeof(len));
    return protoSucceed;
}

template<> inline
uint16_t CSerializorEx::get_vector_length<uint32_t>(std::vector<uint32_t> &vec)
{
    return (uint16_t)(vec.size()*sizeof(uint32_t) + sizeof(uint32_t));
}

template<> inline
uint16_t CSerializorEx::get_vector_length<CMyString>(std::vector<CMyString> &vec)
{
    uint16_t len(0);
    std::vector<CMyString>::iterator it(vec.begin());
    for(; it != vec.end(); ++it)
    {
        len += sizeof(uint16_t);
        len += it->length();
    }
    return len + sizeof(len);
}

