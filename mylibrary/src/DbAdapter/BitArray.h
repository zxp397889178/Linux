/*
 * =====================================================================================
 *
 *       Filename:  BitArray
 *
 *    Description:  比特位数组
 *
 *        Version:  1.0
 *        Created:  2014-4-23
 *       Revision:  none
 *       Compiler:  cl
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.com
 *
 * =====================================================================================
 */

#ifndef _GFX_SERVER_BITARRAY_H_
#define _GFX_SERVER_BITARRAY_H_

#include <iostream>
#include "BaseCode.h"

namespace my_data{

class CData;
class CRecord;
class CBitArray
{
    friend inline std::ostream& operator<<(std::ostream &os, CBitArray &data)
    {
        char szText[1024] = "";
        os << data.GetBinaryStr( szText, sizeof(szText) );
        return os;
    }
public:
    CBitArray(byte_t* pData, size_t cbData, CData *pOwner = NULL );
    ~CBitArray();

    bool Set(size_t index);
    bool Reset(size_t index);
    bool IsSet(size_t index);
    bool IsAnySet(void);
    void ResetAll(void);

    const char* GetBinaryStr(char *szText, size_t cbText);
    const char* GetBinaryStr(CMyString &str);

    inline byte_t* GetData(void) const   { return m_pData; }
    inline size_t  GetLength(void) const { return m_cbData; }
    void Output(void);

public:
    CBitArray(const CBitArray &rhs);
    CBitArray& operator=(const CBitArray &rhs);


private:
    byte_t*         m_pData;
    size_t          m_cbData;       //字节数量
    CRecord*        m_pRecord;
    CData*          m_pOwner;       //属主
};

};  //my_data

#endif      //_GFX_SERVER_BITARRAY_H_
