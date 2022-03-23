/*
 * pg_crc.h
 *
 * PostgreSQL 64-bit CRC support
 *
 * Portions Copyright (c) 1996-2005, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $PostgreSQL: pgsql/src/include/utils/pg_crc.h,v 1.12 2004/12/31 22:03:46 pgsql Exp $
 */

//
// Excerpt of license
//
/*
PostgreSQL Data Base Management System

Portions Copyright (c) 1996-2005, PostgreSQL Global Development Group 
Portions Copyright (c) 1994-1996 Regents of the University of California

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose, without fee, and without a written agreement 
is hereby granted, provided that the above copyright notice and this 
paragraph and the following two paragraphs appear in all copies.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR 
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING 
LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, 
EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS 
ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS 
TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/


#ifndef _GFX_SECURITY_CRC64_H_
#define _GFX_SECURITY_CRC64_H_

#include "BaseCode/mydef.h"

namespace crc64 {

/*
 * If we have a 64-bit integer type, then a 64-bit CRC looks just like the
 * usual sort of implementation.  (See Ross Williams' excellent introduction
 * A PAINLESS GUIDE TO CRC ERROR DETECTION ALGORITHMS, available from
 * ftp://ftp.rocksoft.com/papers/crc_v3.txt or several other net sites.)
 * If we have no working 64-bit type, then fake it with two 32-bit registers.
 *
 * The present implementation is a normal (not "reflected", in Williams'
 * terms) 64-bit CRC, using initial all-ones register contents and a final
 * bit inversion.  The chosen polynomial is borrowed from the DLT1 spec
 * (ECMA-182, available from http://www.ecma.ch/ecma1/STAND/ECMA-182.HTM):
 *
 * x^64 + x^62 + x^57 + x^55 + x^54 + x^53 + x^52 + x^47 + x^46 + x^45 +
 * x^40 + x^39 + x^38 + x^37 + x^35 + x^33 + x^32 + x^31 + x^29 + x^27 +
 * x^24 + x^23 + x^22 + x^21 + x^19 + x^17 + x^13 + x^12 + x^10 + x^9 +
 * x^7 + x^4 + x + 1
 */

// Imported from c.h
/* Decide if we need to decorate 64-bit constants */
#define INT64CONST(x)  ((int64_t) x##LL)
#define UINT64CONST(x) ((uint64_t) x##ULL)

typedef uint64_t CRC_DATA_TYPE;

extern const uint64_t crc64_table[256];

/* Initialize a CRC accumulator */
__inline void Init_crc(uint64_t & crc)
{
	crc = UINT64CONST(0xffffffffffffffff);
}

/* Finish a CRC calculation */
__inline void Fin_crc(uint64_t & crc)
{
	crc ^= UINT64CONST(0xffffffffffffffff);
}

/* Accumulate some (more) bytes into a CRC */
__inline void Calc_crc(uint64_t & crc, byte_t* pbData, size_t len)
{
	/* Constant table for CRC calculation */

	byte_t*		__pbData = pbData;
	size_t		__len = len;

	while (__len-- > 0)
	{
		uint32_t __tab_index = ((uint32_t) (crc >> 56) ^ *__pbData++) & 0xFF;
		crc = crc64_table[__tab_index] ^ (crc << 8);
	}
}


} // namespace crc64

#endif   /* _GFX_SECURITY_CRC64_H_ */

