/*
 * =====================================================================================
 *
 *       Filename:  ByteOrder.h
 *
 *    Description:  use for ByteOrder convert
 *
 *        Version:  1.0
 *        Created:  04/04/2015 11:05:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_BASECODE_BYTEORDER_H_
#define _GFX_BASECODE_BYTEORDER_H_

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

class CByteOrder
{
public:
	static uint16_t Cast(uint16_t val);
	static uint32_t Cast(uint32_t val);
	static uint64_t Cast(uint64_t val);
	static int16_t  Cast(int16_t val);
	static int32_t  Cast(int32_t val);
	static int64_t  Cast(int64_t val);

	static bool IsLittleEndian(void);
};

#endif //	 _GFX_BASECODE_BYTEORDER_H_

