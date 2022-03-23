/*
 * =====================================================================================
 *
 *       Filename:  DbAdapter.cpp
 *
 *    Description:  common function
 *
 *        Version:  1.0
 *        Created:  04/21/2015 09:47:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#include "zconf.h"
#include "zlib.h"
#include "BaseCode.h"
#include <assert.h>

int CompressData(byte_t *&pDest, size_t &cbDest, const byte_t *pSource, size_t cbSource)
{
    //[buff_size][origin_size][pack_data]
    //4           4

    size_t cbMaxBuff = sizeof(size_t) * 2 + cbSource + 1;
    pDest = (byte_t*)calloc(sizeof(byte_t), cbMaxBuff);
    memset(pDest, 0, cbMaxBuff);
    size_t *buff_size = (size_t*)&pDest[0];
    size_t *origin_size = (size_t*)&pDest[sizeof(size_t)];
    Bytef *pBuff = (Bytef *)&pDest[sizeof(size_t) * 2];

    *origin_size = cbSource;
    uLongf cbBuffer = cbSource;
    int ret = compress(pBuff, &cbBuffer, (const Bytef*)pSource, cbSource);
    DB_ASSERT(ret == Z_OK);
    if (ret!=Z_OK)
    {
		free(pDest);
        cbDest = 0;
        return -1;
    }

    *buff_size = cbBuffer + sizeof(size_t) * 2;
    cbDest = *buff_size;
    return (int)*buff_size;
}

int UncompressData(byte_t *&pDest, size_t &cbDest, const byte_t *pSource, size_t cbSource)
{
    //[buff_size][origin_size][pack_data]
    //4           4

    size_t buff_size = *(size_t*)pSource;
    size_t origin_size = *(size_t*)&pSource[sizeof(size_t)];
    Bytef *pSourceBuff = (Bytef*)&pSource[sizeof(size_t)*2];
    DB_ASSERT(buff_size==cbSource);
    pDest = (byte_t*)calloc(sizeof(byte_t), origin_size+1);
    memset(pDest, 0, origin_size+1);

    uLongf cbDestBuff = origin_size;
    uLongf cbSourceBuff = buff_size-sizeof(size_t)*2;
    int ret = uncompress((Bytef*)pDest, &cbDestBuff, pSourceBuff, cbSourceBuff);
    DB_ASSERT(ret==Z_OK);
    if (ret != Z_OK)
    {
        free(pDest);
        cbDest = 0;
        return -1;
    }
    cbDest = origin_size;
    return cbDestBuff;
}

