/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

//
// Excerpt from zlib.h
//
/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.1.3, July 9th, 1998

  Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/


/* @(#) $Id: crc32.h,v 1.1 2003/08/22 08:35:28 nkast Exp $ */

// Code modified by Carlos Klapp on 2005

#ifndef _GFX_SECURITY_CRC32_H_
#define _GFX_SECURITY_CRC32_H_
#include "BaseCode/mydef.h"

namespace crc32 {

typedef 	 uint32_t CRC_DATA_TYPE;
extern const uint32_t crc32_table[256];


/* Initialize a CRC accumulator */
__inline void Init_crc(uint32_t & crc)
{
	crc = 0xffffffffUL;
}

/* Finish a CRC calculation */
__inline void Fin_crc(uint32_t & crc)
{
	crc ^= 0xffffffffUL;
}

/* Accumulate some (more) bytes into a CRC */
__inline void Calc_crc(uint32_t & crc, byte_t* pbData, size_t len)
{
#define DO1(buf) crc = crc32_table[(crc ^ (*pbData++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

	while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) 
	{
		do 
		{
			DO1(buf);
		} while (--len);
	}
}


} // namespace crc32

#endif // _GFX_SECURITY_CRC32_H_

