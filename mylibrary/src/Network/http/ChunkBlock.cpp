/*
 * =====================================================================================
 *
 *       Filename:  ChunkBlock.cpp
 *
 *    Description:  chunk 块的解析
 *
 *        Version:  1.0
 *        Created:  2020年02月15日 18时39分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "ChunkBlock.h"
#include <ctype.h>

extern void dumptext(const char*text, int32_t offset, int32_t txtLen, CMyString &strText);

CChunkBlock::CChunkBlock()
    : m_chunkFinished(false)
    , m_chunkSize(0)
    , m_chunkHeaderSize(0)
    , m_chunkDataSize(0)
    , m_pData(NULL)
{
}

CChunkBlock::~CChunkBlock()
{

}

bool CChunkBlock::Decode(const char *pbuff, size_t cbBuff)
{
    LogTrace("CB> ========== new decoder, length:%lu ==========", cbBuff);
    CMyString dumpShow("[");
    dumptext(pbuff, 0, 16, dumpShow);
    dumpShow.Append("]");
    LogTrace("CB> %s", dumpShow.c_str());

    m_strBuffer.assign(pbuff, cbBuff);
    this->Reset();

    if (NULL == pbuff || cbBuff<2)
    {
        return false;
    }

    // 去掉首部的\r\n
    uint8_t c1(0);
    uint8_t c2(0);
    int32_t offset(0);
    size_t idx(0);
    do {
        c1 = pbuff[idx];
        if (c1== 0x0D)
        {
            c2 = pbuff[idx+1];
            if (c2 == 0x0A)
            {
                idx += 2;
                continue;
            }
            offset = idx;
            break;
        }
        offset=idx;
        break;
    }while(idx+1<cbBuff);
/*    
    for(size_t i(0); i<cbBuff-1; ++i)
    {
        c1 = pbuff[i];
        if (c1==0x0D)
        {
            c2 = pbuff[i+1];
            if (c2==0x0A)
            {
                // 一个完整的\r\n, 继续清除
                continue;
            }
            offset = i;
            break;
        }
        offset = i;
        break;
    }
*/    
    // 获得位置 offset
    LogTrace("CB>offset:%d", offset);
    
    // 解析 chunk 的长度
    int32_t pos2(-1);
    for(size_t i(offset); i<cbBuff-1; ++i)
    {
        c1 = pbuff[i];
        if (c1 == 0x0D)
        {
            c2 = pbuff[i+1];
            if (c2 == 0x0A)
            {
                //找到结束符，中止
                pos2 = i;
                break;
            }
        }
    }
    if ((pos2<=0) || (pos2==offset))
    {
        // 数据未完整，继续收数据
        return false;
    }

    m_chunkHeaderSize = (pos2 - offset);
    char szLine[256] = "";
    strncpy(szLine, (char*)&pbuff[offset], m_chunkHeaderSize);
    LogTrace("CB>chunk block size line:[%s]", szLine);
    //解析数据头
    for(size_t i(0); i<strlen(szLine); ++i)
    {
        c1 = szLine[i];
        if (isalnum(c1) ==0 )
        {
            szLine[i] = '\0';
            break;
        }
    }

    // 获得chunk 大小
    char *endl;
    m_chunkDataSize = strtoul(szLine, &endl, 16);
    if (0 == m_chunkDataSize)
    {
        m_chunkFinished = true;
        // 2: \r\n
        // 3: 0\r\n
        m_chunkSize = offset + m_chunkHeaderSize + 2 + 3;
        LogWarn("CB>is the last chunk.");
        return true;
    }

    LogTrace("CB>chunk header:%d, data:%d", m_chunkHeaderSize, m_chunkDataSize);
    // 判断chunk 是否完整
    m_chunkSize = offset + m_chunkHeaderSize + 2 + m_chunkDataSize;
    if (cbBuff >= (uint32_t)m_chunkSize)
    {
        // 2: \r\n
        // 不含数据块的\r\n, 延到下一个循环处理
        int32_t pos = offset + m_chunkHeaderSize + 2;
        m_pData = (char*)&pbuff[pos];
        LogTrace("CB>chunk block size:%d", m_chunkSize);
        return true;
    }

    // 数据不完整，继续接收
    return false;
}

void CChunkBlock::Reset(void)
{
    m_chunkFinished = false;
    m_chunkSize = 0;
    m_chunkHeaderSize = 0;
    m_chunkDataSize = 0;
    m_pData = NULL; 
    m_strBuffer.clear();
}

void dumptext(const char*text, int32_t offset, int32_t txtLen, CMyString &strText)
{
    for(int32_t i(offset); i<(offset + txtLen); ++i)
    {
        uint8_t c = text[i];
        strText.Append("%02x ", c);
    }
}

void CChunkBlock::dumpIndicator(void)
{
    int32_t offset = m_chunkSize - (m_chunkHeaderSize+2) - m_chunkDataSize;
    const char *pc = m_strBuffer.c_str();

    CMyString strText("chunk indicator:[%d bytes] [", offset);
    dumptext(pc, 0, offset, strText);
    strText.Append("]");
    LogDebug(strText.c_str());
}

void CChunkBlock::dumpHeader(void)
{
    int32_t offset = m_chunkSize - (m_chunkHeaderSize+2) - m_chunkDataSize;
    int32_t hdrLen = m_chunkHeaderSize+2;

    const char *pc = m_strBuffer.c_str();
    CMyString strText("chunk header: [%d bytes] [", hdrLen);
    dumptext(pc, offset, hdrLen, strText);
    strText.Append("]");
    LogDebug(strText.c_str());
}

void CChunkBlock::dump(void)
{
    const char *pc = m_strBuffer.c_str();
    CMyString strText("dump of block: [%d bytes] [", m_strBuffer.size());
    dumptext(pc, 0, m_strBuffer.size(), strText);
    strText.Append("]");
    LogDebug(strText.c_str());
}


