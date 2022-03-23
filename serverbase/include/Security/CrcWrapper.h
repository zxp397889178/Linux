/*
 * =====================================================================================
 *
 *       Filename:  CrcWrapper.h
 *
 *    Description:  crc32 / crc64
 *
 *        Version:  1.0
 *        Created:  2019年08月30日 15时11分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef __SECURITY_LIBRARY_CRCWRAPPER_H__
#define __SECURITY_LIBRARY_CRCWRAPPER_H__

#include "ISecurity.h"
class CCrcWrapper
{
public:
    static uint32_t Encode32(uint8_t *input, size_t cbInput);
    static uint32_t Encode64(uint8_t *input, size_t cbInput);

    static int32_t Encode32File(const CMyString &filename, uint32_t &cipher);
    static int32_t Encode64File(const CMyString &filename, uint64_t &cipher);
};

#endif // __SECURITY_LIBRARY_CRCWRAPPER_H__
