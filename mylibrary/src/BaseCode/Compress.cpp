/*
 * =====================================================================================
 *
 *       Filename:  Compress.cpp
 *
 *    Description:  调用zlib进行压缩、解压的算法
 *
 *        Version:  1.0
 *        Created:  01/27/2020 05:40:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Compress.h"
#include <zlib.h>

// compress data
int32_t z_compress(byte_t *pData, uint32_t cbData, byte_t *pOut, uint32_t &cbOut)
{
    z_stream c_stream;
    int err = 0;

    if (NULL == pData || 0==cbData)
    {
        return -1;
    }

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;
    if(deflateInit(&c_stream, Z_DEFAULT_COMPRESSION) != Z_OK) 
    {
        return -3;
    }
    c_stream.next_in  = pData;
    c_stream.avail_in  = cbData;
    c_stream.next_out = pOut;
    c_stream.avail_out  = cbOut;
    while (c_stream.avail_in != 0 && c_stream.total_out < cbOut) 
    {
        if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) 
        {
            return -5;
        }
    }
    if(c_stream.avail_in != 0)
    {
        return c_stream.avail_in;
    }

    for (;;) 
    {
        if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) 
            break;
        if(err != Z_OK)
        {
             return -7;
        }
    }
    if(deflateEnd(&c_stream) != Z_OK)
    {
        return -9;
    } 
    cbOut = c_stream.total_out;
    return 0;
}

// decompress data
int32_t z_decompress(byte_t *pData, uint32_t cbData,  byte_t *pOut, uint32_t &cbOut)
{
    int err = 0;
    z_stream d_stream; /* decompression stream */

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = pData;
    d_stream.avail_in = 0;
    d_stream.next_out = pOut;

    if(inflateInit(&d_stream) != Z_OK) 
    {
        return -1;
    }
    while (d_stream.total_out < cbOut && d_stream.total_in < cbData) 
    {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) 
            break;
        if(err != Z_OK) 
        {
            return -3;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) 
    {
        return -5;
    }
    cbOut = d_stream.total_out;
    return 0;
}

// Compress gzip data 
int32_t gz_compress(byte_t *pData, uint32_t cbData, byte_t *pOut, uint32_t &cbOut)
{
    z_stream c_stream;
    int err = 0;

    if (NULL == pData || 0==cbData)
    {
        return -1;
    }

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;
    if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 
                -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
    {
        return -3;
    }
    c_stream.next_in  = pData;
    c_stream.avail_in  = cbData;
    c_stream.next_out = pOut;
    c_stream.avail_out  = cbOut;
    while (c_stream.avail_in != 0 && c_stream.total_out < cbOut) 
    {
        if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK) 
        {
            return -5;
        }
    }
    if(c_stream.avail_in != 0) 
    {
        return c_stream.avail_in;
    }

    for (;;) 
    {
        if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) 
            break;
        if(err != Z_OK)
        {
            return -7;
        }
    }
    if(deflateEnd(&c_stream) != Z_OK)
    {
        return -9;
    }
    cbOut = c_stream.total_out;
    return 0;
}

// Uncompress gzip data
int32_t gz_decompress(byte_t *pData, uint32_t cbData, byte_t *pOut, uint32_t &cbOut)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = 
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = pData;
    d_stream.avail_in = 0;
    d_stream.next_out = pOut;
    if(inflateInit2(&d_stream, -MAX_WBITS) != Z_OK)
    {
        return -1;
    }
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while (d_stream.total_out < cbOut && d_stream.total_in < cbData) 
    {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) 
            break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (byte_t*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
                {
                    return -3;
                }
            }
            else 
            {
                return -5;
            }
        }
    }

    if(inflateEnd(&d_stream) != Z_OK) 
    {
        return -7;
    }
    cbOut = d_stream.total_out;
    return 0;
}

// HTTP gzip decompress 
int httpgz_decompress(byte_t *pData, uint32_t cbData, byte_t *pOut, uint32_t cbOut)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = 
    {       0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = pData;
    d_stream.avail_in = 0;
    d_stream.next_out = pOut;
    if(inflateInit2(&d_stream, 47) != Z_OK) 
    {
        return -1;
    }

    while (d_stream.total_out < cbOut && d_stream.total_in < cbData)
    {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) 
            break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (byte_t*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
                {
                    return -3;
                }
            }
            else
            {
                return -5;
            }
        }
    }
    if(inflateEnd(&d_stream) != Z_OK)
    {
        return -7;
    }
    cbOut = d_stream.total_out;
    return 0;
}



