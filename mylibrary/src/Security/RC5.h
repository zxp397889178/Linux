//本类是从CRc5_321216修改而来的

#ifndef _GFX_SECURITY_RC5_H_
#define _GFX_SECURITY_RC5_H_

#include "ISecurity.h"

class CRC5 
{
    enum
    {
        RC5_12      = 12,
        RC5_SUB     = (RC5_12 *2 + 2),

        RC5_16      = 16,
        RC5_KEY     = (RC5_16/4),

        RC5_32      = 32,
    };
public:
    CRC5(void);

public:
    virtual bool SetKey(const void *pKey, size_t cbKey);
    virtual void release();
    virtual int32_t Encrypt(const void *pInput, size_t cbInput, void *&pOutput, size_t &cbOutput);
    virtual int32_t Decrypt(const void *pInput, size_t cbInput, void *&pOutput, size_t &cbOutput);
    virtual size_t PadSize() { return 8; }
    virtual size_t GetPadSize(size_t nLen);
	virtual void SafeFree(void *pOutput);
	
private:
    virtual ~CRC5(void);
    CRC5(const CRC5 &rhs);
    CRC5& operator=(const CRC5 &rhs);

private:
    uint32_t	m_bufKey[RC5_KEY];
    uint32_t	m_bufSub[RC5_SUB];

    uint8_t		m_cbKey[RC5_16+1];
};

#endif  //_GFX_SECURITY_RC5_H_

