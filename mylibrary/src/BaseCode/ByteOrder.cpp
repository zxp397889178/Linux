/*
 * =====================================================================================
 *
 *       Filename:  ByteOrder.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/02/2015 09:08:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "ByteOrder.h"

///////////////////////////////////////////////////////
uint16_t CByteOrder::Cast(uint16_t val)
{
	union
	{
		unsigned char b[2];
		uint16_t val;
	}c;
	c.val = 0;
	c.b[1] = val      & 0xFF;
	c.b[0] = (val>>8) & 0xFF;
	return c.val;
}
uint32_t CByteOrder::Cast(uint32_t val)
{
	union
	{
		unsigned char b[4];
		uint32_t val;
	}c;
	c.val = 0;
	c.b[3] = val       & 0xFF;
	c.b[2] = (val>>8)  & 0xFF;
	c.b[1] = (val>>16) & 0xFF;
	c.b[0] = (val>>24) & 0xFF;
	return c.val;
}

uint64_t CByteOrder::Cast(uint64_t val)
{
	union
	{
		unsigned char b[8];
		uint64_t val;
	}c;
	c.val = 0;
	c.b[7] = val       & 0xFF;
	c.b[6] = (val>>8)  & 0xFF;
	c.b[5] = (val>>16) & 0xFF;
	c.b[4] = (val>>24) & 0xFF;
	c.b[3] = (val>>32) & 0xFF;
	c.b[2] = (val>>40) & 0xFF;
	c.b[1] = (val>>48) & 0xFF;
	c.b[0] = (val>>56) & 0xFF;
	return c.val;
}

bool CByteOrder::IsLittleEndian(void)
{
	union
	{
		unsigned int  a;
		unsigned char b;
	}c;
	c.a = 1;
	return (c.b == 1);
}

int16_t  CByteOrder::Cast(int16_t val)
{
	return (int16_t)CByteOrder::Cast((uint16_t)val);
}

int32_t  CByteOrder::Cast(int32_t val)
{
	return (int32_t)CByteOrder::Cast((uint32_t)val);
}

int64_t  CByteOrder::Cast(int64_t val)
{
	return (int64_t)CByteOrder::Cast((uint64_t)val);
}
