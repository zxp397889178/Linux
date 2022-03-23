/*
 * =====================================================================================
 *
 *       Filename:  ChunkBlock.h
 *
 *    Description:  解析一个chunk block
 *
 *        Version:  1.0
 *        Created:  2020年02月15日 18时38分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"

class CChunkBlock
{
public:
    CChunkBlock();
    ~CChunkBlock();

    bool Decode(const char *pbuff, size_t cbBuff);

    int32_t getChunkSize(void) { return m_chunkSize; }
    int32_t getDataSize(void)  { return m_chunkDataSize; }
    char*   getDataBlock(void) { return m_pData; }

    bool IsFinished(void) { return m_chunkFinished; }

    int32_t getSize(void) { return m_chunkSize; }

    void dumpIndicator(void);
    void dumpHeader(void);
    void dump(void);

private:
    void Reset(void);

private:
    bool        m_chunkFinished;    // 是否最后一个chunk

    int32_t     m_chunkSize;        // 含前导\r\n+[header]\r\n[data], 不含[data]尾随的\r\n
    int32_t     m_chunkHeaderSize;  // [90\r\n]的长度,不含\r\n
    int32_t     m_chunkDataSize;    // [......\r\n]的长度，不含\r\n

    char*       m_pData;            // 数据块指针
    
    CMyString   m_strBuffer;
};
