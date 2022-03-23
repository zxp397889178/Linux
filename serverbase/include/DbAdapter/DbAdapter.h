/*
 * =====================================================================================
 *
 *       Filename:  DbAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/21/2015 09:34:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_DBADAPTER_INCLUDE_
#define _GFX_DBADAPTER_INCLUDE_

#include "BaseCode.h"

#include "DbAdapter/DaField.hpp"
#include "DbAdapter/DaFieldSet.h"
#include "DbAdapter/DaFieldTable.h"

//#include "DbAdapter/IDaRecord.h"
#include "DbAdapter/IBucketEvent.h"
#include "DbAdapter/DaData.h"
#include "DbAdapter/DaRecord.h"
#include "DbAdapter/DaRecordSet.h"
#include "DbAdapter/Bucket.h"

#include "DbAdapter/UserDbData.h"
#include "DbAdapter/UserData.h"
#include "DbAdapter/UserMap.h"
#include "DbAdapter/BitArray.h"

#include "DbAdapter/DictDbData.h"
#include "DbAdapter/DictData.h"
#include "DbAdapter/DictMap.h"

// pDest && pSource will use free(...)
int CompressData(byte_t *&pDest, size_t &cbDest, const byte_t *pSource, size_t cbSource);
int UncompressData(byte_t *&pDest, size_t &cbDest, const byte_t *pSource, size_t cbSource);


#endif // _GFX_DBADAPTER_INCLUDE_
