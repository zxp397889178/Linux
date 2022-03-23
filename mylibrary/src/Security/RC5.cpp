#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "RC5.h"

#ifdef GUOWEI_ONLINE
    const uint32_t  RC5_PW32    = 0xA7D35183;
    const uint32_t  RC5_QW32    = 0xCE3779A9;
#else
    const uint32_t  RC5_PW32    = 0xB7E15163;
    const uint32_t  RC5_QW32    = 0x9E3779B9;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////

static uint32_t rotate_left(uint32_t nData, uint32_t nCount)
{
    nCount %= 32;

    uint32_t nHigh = nData >> (32-nCount);
    return (nData << nCount) | nHigh;
}

static uint32_t rotate_right(uint32_t nData, uint32_t nCount)
{
    nCount %= 32;

    uint32_t nLow = nData << (32-nCount);
    return (nData >> nCount) | nLow;
}

//////////////////////////////////////////////////////////////////////////
//CRC5
CRC5::CRC5(void)
{
    memset(m_bufKey, 0, sizeof(m_bufKey));
    memset(m_bufSub, 0, sizeof(m_bufSub));
    memset(m_cbKey, 0, sizeof(m_cbKey));
}

CRC5::~CRC5(void)
{
}

void CRC5::release(void)
{
    delete this;
}

bool CRC5::SetKey(const void *pKey, size_t cbKey)
{
	if (cbKey != RC5_16)
	{
		return false;
	}

    // 保存m_bufKey
    memcpy(m_bufKey, pKey, RC5_16);
    memcpy(m_cbKey, pKey, RC5_16);

    // 初始化m_bufSub
    m_bufSub[0] = RC5_PW32;
    int32_t i = 0;
    for(i = 1; i < RC5_SUB; i++)
    {
        m_bufSub[i] = m_bufSub[i-1] + RC5_QW32;
    }

    // 生成m_bufSub
    int32_t j;
    uint32_t x, y;
    i = j = x = y = 0;
    for(int32_t k = 0; k < 3 * std::max((int32_t)RC5_KEY, (int32_t)RC5_SUB); k++)
    {
        m_bufSub[i] = rotate_left((m_bufSub[i] + x + y), 3);
        x = m_bufSub[i];
        i = (i + 1) % RC5_SUB;
        m_bufKey[j] = rotate_left((m_bufKey[j] + x + y), (x + y));
        y = m_bufKey[j];
        j = (j + 1) % RC5_KEY;
    }

    return true;
}

size_t CRC5::GetPadSize(size_t nLen)
{
#define PAD_ALIGN(len, pad)	( ((len)+(pad)-1) & ~((pad)-1) )

	return PAD_ALIGN(nLen, this->PadSize());
}

int32_t CRC5::Encrypt(const void *pInput, size_t cbInput, void *&pOutput, size_t &cbOutput)
{
    if (cbInput == 0 || (cbInput % this->PadSize()) != 0 ) 
    {
        return -1;
    }

    cbOutput = cbInput;
    pOutput = const_cast<void*>(pInput);

    //加密过程
    size_t nCount = cbOutput / this->PadSize();
    uint32_t* bufData = (uint32_t*)pInput;
    for(size_t k = 0; k < nCount; k++)
    {
        uint32_t a = bufData[2*k];
        uint32_t b = bufData[2*k + 1];

        // 加密a b
        uint32_t le  = a + m_bufSub[0];
        uint32_t re  = b + m_bufSub[1];
        for(int32_t i = 1; i <= RC5_12; i++)
        {
            le = rotate_left((le ^ re), re) + m_bufSub[2*i];
            re = rotate_left((re ^ le), le) + m_bufSub[2*i + 1];
        }

        bufData[2*k]     = le;
        bufData[2*k + 1] = re;
    }
    
    return 0;
}

int32_t CRC5::Decrypt(const void *pInput, size_t cbInput, void *&pOutput, size_t &cbOutput)
{
    if ( cbInput == 0 || (cbInput % this->PadSize()) != 0)
    {
        return -1;
    }

	cbOutput = cbInput;
	pOutput = const_cast<void*>(pInput);
	
    size_t nCount = cbInput / this->PadSize();
    uint32_t* bufData = (uint32_t*)pInput;
    for(size_t k = 0; k < nCount; k++)
    {
        // 解密 ld rd
        uint32_t ld  = bufData[2*k];
        uint32_t rd  = bufData[2*k + 1];
        for(int32_t i = RC5_12; i >= 1; i--)
        {
            rd = rotate_right((rd - m_bufSub[2*i + 1]),  ld) ^ ld;
            ld = rotate_right((ld - m_bufSub[2*i]),  rd) ^ rd;
        }

        uint32_t b = rd - m_bufSub[1];
        uint32_t a = ld - m_bufSub[0];

        bufData[2*k]     = a;
        bufData[2*k + 1] = b;
    }
    
    return 0;
}

void CRC5::SafeFree(void *pOutput)
{
	return;
}

