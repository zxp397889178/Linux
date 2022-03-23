/*
 * =====================================================================================
 *
 *       Filename:  Compress.h
 *
 *    Description:  调用zlib进行压缩、解压缩
 *
 *        Version:  1.0
 *        Created:  01/27/2020 05:39:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mydef.h"
#include <errno.h>


int32_t z_compress(byte_t* pData, uint32_t cbData, byte_t *pOut, uint32_t &cbOut); 
int32_t z_decompress(byte_t *pData, uint32_t cbData,  byte_t *pOut, uint32_t &cbOut);


int32_t gz_compress(byte_t *pData, uint32_t cbData, byte_t *pOut, uint32_t &cbOut);
int32_t gz_decompress(byte_t *pData, uint32_t cbData, byte_t *pOut, uint32_t &cbOut);

int32_t httpgz_decompress(byte_t *pData, uint32_t cbData, byte_t *pOut, uint32_t cbOut);


namespace gzip {
enum error_define
{
    err_OK = 0,
    err_Success = err_OK,

    err_LastError = -2000,
    err_GZipCompressDataError,
    err_GZipCompressDataInvalid,
    err_GZipCompressEndFailure,
    err_GZipCompressFailure,
    err_GZipCompressInitFailure,
    err_GZipDecompressDataError,
    err_GZipDecompressEndFailure,
    err_GZipDecompressFailure,
    err_GZipDecompressInitFailure,
};

int32_t compress(byte_t* data, uint32_t ndata, byte_t* zdata, uint32_t* nzdata);

int32_t decompress(byte_t* zdata, uint32_t nzdata, byte_t* data, uint32_t* ndata);


};

