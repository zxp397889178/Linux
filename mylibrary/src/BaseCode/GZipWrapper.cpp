/*
 * =====================================================================================
 *
 *       Filename:  GzipWrapper.cpp
 *
 *    Description:  gzip压缩的封装
 *
 *        Version:  1.0
 *        Created:  2020年07月11日 19时24分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include <zlib.h>
#include "Compress.h"

namespace gzip {

int32_t compress(byte_t* data, uint32_t ndata, byte_t* zdata, uint32_t* nzdata)
{
    z_stream c_stream;
    int32_t err(0);

    if (!data || ndata<=0)
    {
        return err_GZipCompressDataInvalid;
    }

    c_stream.zalloc = NULL;
    c_stream.zfree = NULL;
    c_stream.opaque = NULL;
    //只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
    if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) 
    {
        return err_GZipCompressInitFailure;
    }
    c_stream.next_in = data;
    c_stream.avail_in = ndata;
    c_stream.next_out = zdata;
    c_stream.avail_out = *nzdata;
    while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata)
    {
        if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK)
        {
            return err_GZipCompressDataError;
        }
    }
    if(c_stream.avail_in != 0)
    {
        return c_stream.avail_in;
    }
    
    for(;;) 
    {
        if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) 
        {
            break;
        }
        if(err != Z_OK)
        {
            return err_GZipCompressFailure;
        }
    }

    if(deflateEnd(&c_stream) != Z_OK) 
    {
        return err_GZipCompressEndFailure;
    }

    *nzdata = c_stream.total_out;

    return err_OK;
}

int32_t decompress(byte_t* zdata, uint32_t nzdata, byte_t* data, uint32_t* ndata)
{
    int32_t err(0);

    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = 
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };

    d_stream.zalloc = NULL;
    d_stream.zfree = NULL;
    d_stream.opaque = NULL;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;

    //只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
    if(inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK)
    {
        return err_GZipDecompressInitFailure;
    } 

    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
    while(d_stream.total_out < *ndata && d_stream.total_in < nzdata) 
    {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END)
        {
            break;
        }

        if(err != Z_OK) 
        {
            if(err == Z_DATA_ERROR) 
            {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
                {
                    return err_GZipDecompressFailure;
                }
            }
            else
            {
                return err_GZipDecompressDataError;
            }
        }
    }

    if(inflateEnd(&d_stream) != Z_OK)
    {
        return err_GZipDecompressEndFailure;
    }

    *ndata = d_stream.total_out;

    return err_OK;
}

};  // namespace gzip



