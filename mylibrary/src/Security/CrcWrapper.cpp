/*
 * =====================================================================================
 *
 *       Filename:  CrcWrapper.cpp
 *
 *    Description:  crc32 / crc64
 *
 *        Version:  1.0
 *        Created:  2019年08月30日 15时14分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "CrcWrapper.h"
#include "crc/crc32.h"
#include "crc/crc64.h"
#include <iostream>
#include <fstream>

#define CRC_BUFSIZE		65536	// 64KB

uint32_t CCrcWrapper::Encode32(uint8_t *input, size_t cbInput)
{
	uint32_t crc;
	crc32::Init_crc(crc);
	crc32::Calc_crc(crc, input, cbInput);
	crc32::Fin_crc(crc);

	return crc;
}

uint32_t CCrcWrapper::Encode64(uint8_t *input, size_t cbInput)
{
	uint64_t crc64;
	crc64::Init_crc(crc64);
	crc64::Calc_crc(crc64, input, cbInput);
	crc64::Fin_crc(crc64);

	return crc64;
}

int32_t CCrcWrapper::Encode32File(const CMyString &filename, uint32_t &cipher)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (NULL == fp)
    {
        return -1;
    }

    int32_t ret(0);
    crc32::Init_crc(cipher);
    while(!feof(fp))
    {
        uint8_t buff[CRC_BUFSIZE] = {0,};
        size_t size = fread(buff, 1, CRC_BUFSIZE, fp);
        if (0 == size)
        {
            ret = -2;
            break;
        }
        crc32::Calc_crc(cipher, buff, size);
    }
    crc32::Fin_crc(cipher);
    fclose(fp);

    return ret;
}

int32_t CCrcWrapper::Encode64File(const CMyString &filename, uint64_t &cipher)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (NULL == fp)
    {
        return -1;
    }

    int32_t ret(0);
    crc64::Init_crc(cipher);
    while(!feof(fp))
    {
        uint8_t buff[CRC_BUFSIZE] = {0,};
        size_t size = fread(buff, 1, CRC_BUFSIZE, fp);
        if (0 == size)
        {
            ret = -2;
            break;
        }
        crc64::Calc_crc(cipher, buff, size);
    }
    crc64::Fin_crc(cipher);
    fclose(fp);

    return ret;
}

