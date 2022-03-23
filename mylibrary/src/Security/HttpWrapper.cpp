/*
 * =====================================================================================
 *
 *       Filename:  HttpWrapper.cpp
 *
 *    Description:  encode & decode for http use
 *
 *        Version:  1.0
 *        Created:  2019年09月03日 11时51分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "HttpWrapper.h"

/*
static uint8_t ToHex(uint8_t x)
{
    return (x>9) ? (x+55) : (x+48);
}

static uint8_t FromHex(uint8_t x)
{
    uint8_t y(0);
    if (x>='A' && x<='Z') y=x-'A'+10;
    else if (x>='a' && x<='z') y=x-'a'+10;
    else if (x>='0' && x<='9') y=x-'0';
    else {}
    return y;
}
*/

static char dec2hexChar(int16_t n) {
    if ( 0 <= n && n <= 9 ) {
        return char( short('0') + n );
    } else if ( 10 <= n && n <= 15 ) {
        return char( short('A') + n - 10 );
    } else {
        return char(0);
    }
}
 
static int16_t hexChar2dec(char c) {
    if ( '0'<=c && c<='9' ) {
        return short(c-'0');
    } else if ( 'a'<=c && c<='f' ) {
        return ( short(c-'a') + 10 );
    } else if ( 'A'<=c && c<='F' ) {
        return ( short(c-'A') + 10 );
    } else {
        return -1;
    }
}
 
void translate_char(uint8_t c, CMyString &str)
{
    int j = (int16_t)c;
    if ( j < 0 ) 
    {
        j += 256;
    }
    int i1 = j / 16;
    int i0 = j - i1*16;
    str += '%';
    str += dec2hexChar(i1);
    str += dec2hexChar(i0);
}

CMyString CHttpWrapper::escape(const CMyString &input)
{
    static char s_keepchar[] = "*/@+-._";

    CMyString strTemp;
    for(size_t i(0); i<input.length(); ++i)
    {
        uint8_t c = input[i];
        if (isalnum(c) || strchr(s_keepchar, c)!=NULL) 
        {
            strTemp += c;
        }
        else
        {
            translate_char(c, strTemp);
        }
    }
    return strTemp;
}

CMyString CHttpWrapper::encodeURI(const CMyString &input)
{
    // static char s_keepchar[] = "~!@#$&*()=:/,;?+'";
    static char s_keepchar[] = "!#$&'()*+,/:;=?@-._~";

    CMyString strTemp;
    for(size_t i(0); i<input.length(); ++i)
    {
        uint8_t c = input[i];
        if (isalnum(c) || strchr(s_keepchar, c)!=NULL) 
        {
            strTemp += c;
        }
        else
        {
            translate_char(c, strTemp);
        }
    }
    return strTemp;
}

CMyString CHttpWrapper::encodeURIComponent(const CMyString &input)
{
    //static char s_keepchar[] = "~!*()'";
    static char s_keepchar[] = "!'()*-._~";

    CMyString strTemp;
    for(size_t i(0); i<input.length(); ++i)
    {
        uint8_t c = input[i];
        if (isalnum(c) || strchr(s_keepchar, c)!=NULL) 
        {
            strTemp += c;
        }
        else
        {
            translate_char(c, strTemp);
        }
    }
    return strTemp;
}

CMyString CHttpWrapper::encodeURIComponentEx(const CMyString &input, bool bUpperCase)
{
    static char s_keepchar[] = "!'()*-._~";

    CMyString strTemp;
    for(size_t i(0); i<input.length(); ++i)
    {
        uint8_t c = input[i];
        if (isalnum(c) || strchr(s_keepchar, c)!=NULL) 
        {
            strTemp += c;
        }
        else
        {
            translate_char(c, strTemp);
        }
    }
    return strTemp;
}

/*
CMyString CHttpWrapper::UrlEncode(CMyString& input)
{
    CMyString strTemp;
    size_t length = input.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((uint8_t)input[i]) || 
            (input[i] == '-') ||
            (input[i] == '_') || 
            (input[i] == '.') || 
            (input[i] == '~'))
            strTemp += input[i];
        else if (input[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((uint8_t)input[i] >> 4);
            strTemp += ToHex((uint8_t)input[i] % 16);
        }
    }
    input = strTemp;
    return input;
}
 
CMyString CHttpWrapper::UrlDecode(CMyString& cipher)
{
    CMyString strTemp;
    size_t length = cipher.length();
    for (size_t i = 0; i < length; i++)
    {
        if (cipher[i] == '+') strTemp += ' ';
        else if (cipher[i] == '%')
        {
            assert(i + 2 < length);
            uint8_t high = FromHex((uint8_t)cipher[++i]);
            uint8_t low = FromHex((uint8_t)cipher[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += cipher[i];
    }
    cipher = strTemp;
    return cipher;
}
*/

CMyString CHttpWrapper::urlEncode(CMyString &input)
{
    CMyString result;
    for ( size_t i=0; i<input.size(); i++ ) 
    {
        char c = input[i];
        if ( ( '0'<=c && c<='9' ) ||
             ( 'a'<=c && c<='z' ) ||
             ( 'A'<=c && c<='Z' ) ||
             c=='/' || c=='.' || c=='_'
        ) 
        {
            result += c;
        } 
        else 
        {
            int j = (int16_t)c;
            if ( j < 0 ) 
            {
                j += 256;
            }
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1*16;
            result += '%';
            result += dec2hexChar(i1);
            result += dec2hexChar(i0);
        }
    }
    return result;
}

bool CHttpWrapper::urlDecode(const CMyString &cipher, CMyString &result)
{
    for ( size_t i=0; i<cipher.size(); i++ ) 
    {
        char c = cipher[i];
        if ( c != '%' ) 
        {
            result += c;
        } 
        else 
        {
            // 字符越界了
            if (i+2 >= cipher.size()) 
            {
                return false;
            }
            char c1 = cipher[++i];
            char c0 = cipher[++i];
            int num = 0;
            num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
            result += char(num);
        }
    }
    return true;
}

void CHttpWrapper::UTF_8ToUnicode(wchar_t* pOut,char *pText)
{
    char* uchar = (char *)pOut;

    uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
    uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

    return;
}

void CHttpWrapper::UnicodeToUTF_8(char* pOut,wchar_t* pText)
{
    // 注意 WCHAR高低字的顺序,低字节在前，高字节在后
    char* pchar = (char *)pText;

    pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
    pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
    pOut[2] = (0x80 | (pchar[0] & 0x3F));

    return;
}

