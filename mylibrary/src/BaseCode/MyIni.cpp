#include "MyIni.h"
#include <fstream>
#include <string.h>
#include <assert.h>
#include "StringFunc.h"
#include "TimeFunc.h"
#include "Codec.h"

inline bool iscomment(char c)
{
    if (c==';' || c=='#' || c=='\0')
    {
        return true;
    }
    return false;
}



//////////////////////////////////////////////////////////////////////////
//CMyIni::KEY_VALUE
CMyIni::KEY_VALUE::KEY_VALUE()
    : key()
    , value()
{
}

CMyIni::KEY_VALUE::KEY_VALUE(const char *pszKey, const char *pszValue)
    : key()
    , value()
{
    if (pszKey==NULL || '\0' == pszKey[0])
    {
        assert(0);
    }
    key = pszKey;
    if (NULL != pszValue)
    {
        value = pszValue;
    }
}

const char* CMyIni::KEY_VALUE::toString()
{
    return value.c_str();
}

int32_t CMyIni::KEY_VALUE::toInt()
{
    return (int)StrToLong(value.c_str());
}

int64_t CMyIni::KEY_VALUE::toLong()
{
    return StrToLong(value.c_str());
}

uint32_t CMyIni::KEY_VALUE::toUint()
{
    return StrToUint32(value.c_str());
}


float CMyIni::KEY_VALUE::toFloat()
{
    return (float)StrToDouble(value.c_str());
}

double CMyIni::KEY_VALUE::toDouble()
{
    return StrToDouble(value.c_str());
}

bool CMyIni::KEY_VALUE::toBoolean()
{
    return StrToBoolean(value.c_str());
}

time_t CMyIni::KEY_VALUE::toTime()
{
    return StrToTime(value.c_str());
    //int year, month, day, hour, minute, second;
    //if (!DecodeTimeStr(value.c_str(), year, month, day, hour, minute, second))
    //{
    //    return (time_t)0;
    //}
    //struct tm timev = {0,};
    //timev.tm_year   = year-1900;
    //timev.tm_mon    = month;
    //timev.tm_mday   = day;
    //timev.tm_hour   = hour;
    //timev.tm_min    = minute;
    //timev.tm_sec    = second;
    //time_t val = mktime(&timev);

    //return val;
}

TwinsInt CMyIni::KEY_VALUE::toTwinsInt()
{
    TwinsInt result;
    bool ret = StrToTwinsInt(value.c_str(), result);
    assert(ret);
    return result;
}

TriplesInt CMyIni::KEY_VALUE::toTriplesInt()
{
    TriplesInt result;
    bool ret = StrToTriplesInt(value.c_str(), result);
    assert(ret);
    return result;
}

QuadInt CMyIni::KEY_VALUE::toQuadInt()
{
    QuadInt result;
    bool ret = StrToQuadInt(value.c_str(), result);
    assert(ret);
    return result;
}

TwinsNumeric CMyIni::KEY_VALUE::toTwinsValue()
{
    TwinsNumeric result;
    bool ret = StrToTwinsNumeric(value.c_str(), result);
    assert(ret);
    return result;
}

TriplesNumeric CMyIni::KEY_VALUE::toTriplesValue()
{
    TriplesNumeric result;
    bool ret = StrToTriplesNumeric(value.c_str(), result);
    assert(ret);
    return result;
}

QuadNumeric CMyIni::KEY_VALUE::toQuadValue()
{
    QuadNumeric result;
    bool ret = StrToQuadNumeric(value.c_str(), result);
    assert(ret);
    return result;
}

//////////////////////////////////////////////////////////////////////////
//CMyIni:SECTION_INFO
CMyIni::SECTION_INFO::SECTION_INFO()
    : section()
    , keyinfo()
{
}

void CMyIni::SECTION_INFO::AddKey(const char *pszKey, const char *pszValue)
{
    if (EMPTY_STR(pszKey))
    {
        return;
    }

    bool bFind(false);
    for(std::vector<KEY_VALUE>::iterator it(keyinfo.begin());
        it != keyinfo.end(); ++it)
    {
        KEY_VALUE &refVal = *it;
        if (refVal.key == pszKey)
        {
            refVal.value = pszValue;
            bFind = true;
            break;
        }
    }

    if (!bFind)
    {
        keyinfo.push_back(KEY_VALUE(pszKey, pszValue));
    }
}

CMyIni::KEY_VALUE* CMyIni::SECTION_INFO::GetKey(const char *pszKey)
{
    if (EMPTY_STR(pszKey))
    {
        return NULL;
    }
    for(std::vector<KEY_VALUE>::iterator it(keyinfo.begin());
        it != keyinfo.end(); ++it)
    {
        KEY_VALUE &refVal = *it;
        if (refVal.key == pszKey)
        {
            return &refVal;
        }
    }
    return NULL;
}

bool CMyIni::SECTION_INFO::IsExistKey(const char *pszKey)
{
    return (this->GetKey(pszKey) != NULL);
}

//////////////////////////////////////////////////////////////////////////
//CMyIni

CMyIni::CMyIni(void)
    : m_eFileCodec(codecAnsi)
{
}

CMyIni::~CMyIni(void)
{
}

bool CMyIni::Init(const char * pszIniFile)
{
    if (EMPTY_STR(pszIniFile))
    {
        return false;
    }

    m_strIniFile = pszIniFile;

    return this->Load();
}

bool CMyIni::Refresh(void)
{
    m_vecSection.clear();
    return this->Load();
}

bool CMyIni::Load(void)
{
    std::ifstream ifile;
    ifile.open(m_strIniFile.c_str(), std::ios_base::in);
    if (!ifile.is_open())
    {
        return false;
    }
    char buffer[256] = "";
    char szSection[128] = "";
    int32_t nLine(0);
    while(!ifile.eof())
    {
        memset(buffer, 0, sizeof(buffer));
        ifile.getline(buffer, sizeof(buffer));
        if (0 == nLine++)
        {
            int32_t nLen = this->QueryFileCodec(buffer);
            if (nLen>0)
            {
                int32_t buffLen = strlen(buffer) - nLen;
                memmove(&buffer[0], &buffer[nLen], buffLen);
                buffer[buffLen] = '\0';
                if (m_eFileCodec != codecAnsi && m_eFileCodec != codecUTF8)
                {
                    assert(0);
                    return false;
                }
            }
        }

        if (strlen(buffer)==0)
        {
            continue;
        }

        if (m_eFileCodec==codecUTF8)
        {
            unsigned char buf[256] = {0, };
            size_t nLen = strlen(buffer);
            memcpy(buf, buffer, nLen);
            memset(buffer, 0, sizeof(buffer));
            utf8_to_gbk((char*)&buf, nLen, buffer, sizeof(buffer));
        }

        if (this->IsSectionStr(buffer))
        {
            memset(szSection, 0, sizeof(szSection));
            this->GetSection(buffer, szSection, sizeof(szSection));
            if (this->IsExistSection(szSection))
            {
                //提示有重复的Section,但仍将后续的Key Value导入系统
                continue;
            }

            SECTION_INFO sectionInfo;
            sectionInfo.section = szSection;
            m_vecSection.push_back(sectionInfo);
        }
        else
        {
            if (!this->IsKeyValueStr(buffer))
            {
                //不可识别的Key Value的字符串，忽略
                continue;
            }
            SECTION_INFO *pSection = this->FindSection(szSection);
            if (pSection == NULL)
            {
                //未找到对应的Section，忽略
                continue;
            }

            char szKey[128] = "";
            char szValue[256] = "";
            this->DecodeKeyValue(buffer, szKey, sizeof(szKey), szValue, sizeof(szValue) );

            //若存在Key，则覆盖之，否则添加之
            pSection->AddKey(szKey, szValue);
        }
    }
    ifile.close();

    return true;
}

bool CMyIni::IsExistSection(const char *pszSection)
{
    return (this->FindSection(pszSection) != NULL);
}

bool CMyIni::IsExistKey(const char *pszSection, const char *pszKey)
{
    SECTION_INFO* pSection = this->FindSection(pszSection);
    if (pSection == NULL)
    {
        return false;
    }
    return pSection->IsExistKey(pszKey);
}

CMyIni::SECTION_INFO* CMyIni::FindSection(const char *pszSection)
{
    if (NULL == pszSection || '\0' == pszSection[0])
    {
        return NULL;
    }
    for(SECTIONS::iterator it(m_vecSection.begin()); 
        it != m_vecSection.end(); ++it)
    {
        SECTION_INFO &refSection = *it;
        if (refSection.section == pszSection)
        {
            return &refSection;
        }
    }

    return NULL;
}

CMyIni::KEY_VALUE* CMyIni::FindKey(const char *pszSection, const char *pszKey)
{
    SECTION_INFO *pSection = this->FindSection(pszSection);
    if (NULL == pSection)
    {
        return NULL;
    }
    KEY_VALUE *pKey = pSection->GetKey(pszKey);
    if (NULL == pKey)
    {
        return NULL;
    }
    return pKey;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, char *szText, size_t cbText)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    safe_strcpy(szText, cbText, pKey->toString());
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, int32_t &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toInt();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, int64_t &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toLong();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, uint32_t &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toUint();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, float &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toFloat();
    return true;
}
bool CMyIni::Get(const char *pszSection, const char *pszKey, double &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toDouble();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, bool &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toBoolean();
    return true;
}


void CMyIni::Set(const char *pszSection, const char *pszKey, const char *val)
{
    SECTION_INFO *pSection = this->FindSection(pszSection);
    if (NULL == pSection)
    {
        SECTION_INFO sec;
        sec.AddKey(pszKey, val);
        m_vecSection.push_back(sec);
        return;
    }

    KEY_VALUE *pKey = pSection->GetKey(pszKey);
    if (NULL == pKey)
    {
        pSection->AddKey(pszKey, val);
        return;
    }

    pKey->value = val;
}

void CMyIni::Set(const char *pszSection, const char *pszKey, int32_t val)
{
    char szVal[64] = "";
    safe_sprintf(szVal, sizeof(szVal), "%d", val);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, int64_t val)
{
    char szVal[64] = "";
    safe_sprintf(szVal, sizeof(szVal), "%ld", val);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, uint32_t val)
{
    char szVal[64] = "";
#if (defined WIN32 && _MSC_VER>1400)
    _ultoa_s(val, szVal, sizeof(szVal), 10);
#else
    //ultoa(val, szVal, 10);
    sprintf(szVal, "%ul", val);
#endif
    this->Set(pszSection, pszKey, szVal);
}


void CMyIni::Set(const char *pszSection, const char *pszKey, float val)
{
    char szVal[64] = "";
    safe_sprintf(szVal, sizeof(szVal), "%.*lf", val);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, double val)
{
    char szVal[64] = "";
    safe_sprintf(szVal, sizeof(szVal), "%.*lf", val);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, bool val)
{
    char szVal[64] = "";
    safe_sprintf(szVal, sizeof(szVal), "%s", val ? "true" : "false");
    this->Set(pszSection, pszKey, szVal);
}


bool CMyIni::IsSectionStr(const char *pszString)
{
    //#pragma todo("判断是否为Section")
    if (NULL == pszString || '\0' == pszString[0])
    {
        return false;
    }

    bool bFind(false);
    const char *pc = pszString;
    if (*pc == '[')
    {
        char c;
        while((c=*pc++) != '\0')
        {
            if (!isascii(c))
            {
                //对于非ASCII码，不作检测
                continue;
            }

            if (isalnum(c))
            {
                continue;
            }
            else if (isspace(c))
            {
                break;
            }
            else if (c==']')
            {
                bFind = true;
                break;
            }
        }
    }
    return bFind;
}

void CMyIni::GetSection(const char *pszString, char *szSection, size_t cbSection)
{
    if (NULL == pszString || '\0' == pszString[0])
    {
        return;
    }

    const char *pc = pszString;
    if (*pc == '[')
    {
        char c;
        while((c=*pc++) != '\0')
        {
            if (!isascii(c))
            {
                //对于非ASCII码，不作检测
                continue;
            }

            if (isalnum(c))
            {
                continue;
            }
            else if (isspace(c))
            {
                break;
            }
            else if (c==']')
            {
                int32_t cnt = pc-pszString-1;
                safe_strncpy(szSection, cbSection, &pszString[1], cnt-1);
                szSection[cnt-1]='\0';
                trim(szSection);
                break;
            }
        }
    }
}

bool CMyIni::IsKeyValueStr(const char *pszString)
{
    if (NULL == pszString || '\0' == pszString[0])
    {
        return false;
    }

    char *pc = (char*)pszString;
    if (iscomment(*pc))
        //if (*pc==';' || *pc=='#' || (*pc=='/' && *(pc+1)=='/') )
    {
        //注释行
        return false;
    }

    if (isascii(*pc))
    {
        if (isalnum(*pc) || *pc=='_')
        {
            if (strchr(pszString, '=') != NULL)
            {
                return true;
            }
        }
    }
    else
    {
        //非ascii碼,只檢查是否存在=
        if (strchr(pszString, '=') != NULL)
        {
            return true;
        }
    }
    return false;
}

void CMyIni::DecodeKeyValue(const char *pszString, char *szKey, size_t cbKey, char *szValue, size_t cbValue)
{
    if (EMPTY_STR(pszString))
    {
        return;
    }

    const char *pc = strchr(pszString, '=');
    if (NULL == pc)
    {
        safe_strcpy(szKey, cbKey, pszString);
        trim(szKey);
        return;
    }
    safe_strncpy(szKey, cbKey, pszString, pc-pszString);
    ++pc;
    const char *pc1 = pc;
    while( !iscomment(*pc1++) );
    safe_strncpy(szValue, cbValue, pc, pc1-pc-1);
    trim(szKey);
    trim(szValue);
}

bool CMyIni::Save(const char *pszFile)
{
    std::ofstream ofile;
    ofile.open(pszFile, std::ios_base::out|std::ios_base::trunc);
    if (!ofile.is_open())
    {
        return false;
    }

    for(SECTIONS::iterator it(m_vecSection.begin()); it != m_vecSection.end(); ++it)
    {
        SECTION_INFO &refSection = *it;
        ofile << "[" << refSection.section << "]" << std::endl;
        for(std::vector<KEY_VALUE>::iterator itKey = refSection.keyinfo.begin();
            itKey != refSection.keyinfo.end(); ++itKey)
        {
            KEY_VALUE &refKey = *itKey;
            ofile << refKey.key << "=" << refKey.value << std::endl;
        }
        ofile << std::endl;

    }

    ofile.close();

    return true;
}

int32_t CMyIni::QueryFileCodec(const char *buffer)
{
    if (NULL == buffer)
    {
        return -1;
    }

    char codec[5] = "";
    safe_strncpy(codec, sizeof(codec), buffer, 4);
    int32_t nLen(0);
    if ((uint8_t)codec[0]==0xFE && (uint8_t)codec[1]==0xFF)
    {
        // FE FF > Unicode big endian
        // FE FF > UTF-16/UCS-2, little endian
        m_eFileCodec = codecUnicodeBigEndian;
        nLen = 2;
    }
    else if ((uint8_t)codec[0]==0xFF && (uint8_t)codec[1]==0xFE)
    {
        // FF FE        > Unicode
        // FF FE        > UTF-16/UCS-2, big endian
        // FF FE 00 00  > UTF-32/UCS-4, little endian
        if ((uint8_t)codec[2]==0x00 && (uint8_t)codec[3]==0x00)
        {
            m_eFileCodec = codecUCS4;
            nLen = 4;
        }
        else
        {
            m_eFileCodec = codecUnicode;
            nLen = 2;
        }
    }
    else if ((uint8_t)codec[0]==0xEF && (uint8_t)codec[1]==0xBB &&
        (uint8_t)codec[2]==0xBF)
    {
        // EF BB BF > UTF-8
        m_eFileCodec = codecUTF8;
        nLen = 3;
    }
    else if ((uint8_t)codec[0]==0x00 && (uint8_t)codec[1]==0x00 &&
        (uint8_t)codec[2]==0xFE && (uint8_t)codec[3]==0xFF)
    {
        // 00 00 FE FF > UTF-32/UCS-4, big-endian
        m_eFileCodec = codecUCS4BigEndian;
        nLen = 4;
    }
    else
    {
        m_eFileCodec = codecAnsi;
        nLen = 0;
    }
    return nLen;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, TwinsInt &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toTwinsInt();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, TriplesInt &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toTriplesInt();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, QuadInt &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toQuadInt();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, TwinsNumeric &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toTwinsValue();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, TriplesNumeric &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toTriplesValue();
    return true;
}

bool CMyIni::Get(const char *pszSection, const char *pszKey, QuadNumeric &val)
{
    KEY_VALUE *pKey = this->FindKey(pszSection, pszKey);
    if (pKey == NULL)
    {
        return false;
    }
    val = pKey->toQuadValue();
    return true;
}

void CMyIni::Set(const char *pszSection, const char *pszKey, const TwinsInt &val)
{
    char szVal[128] = "";
    safe_sprintf(szVal, sizeof(szVal), "%d %d", val.x, val.y);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, const TriplesInt &val)
{
    char szVal[128] = "";
    safe_sprintf(szVal, sizeof(szVal), "%d %d %d", val.x, val.y, val.z);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, const QuadInt &val)
{
    char szVal[128] = "";
    safe_sprintf(szVal, sizeof(szVal), "%d %d %d %d", val.x, val.y, val.z, val.a);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, const TwinsNumeric &val)
{
    char szVal[128] = "";
    safe_sprintf(szVal, sizeof(szVal), "%.*lf %.*lf", val.x, val.y);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, const TriplesNumeric &val)
{
    char szVal[128] = "";
    safe_sprintf(szVal, sizeof(szVal), "%.*lf %.*lf %.*lf", val.x, val.y, val.z);
    this->Set(pszSection, pszKey, szVal);
}

void CMyIni::Set(const char *pszSection, const char *pszKey, const QuadNumeric &val)
{
    char szVal[128] = "";
    safe_sprintf(szVal, sizeof(szVal), "%.*lf %.*lf %.*lf %.*lf", val.x, val.y, val.z, val.a);
    this->Set(pszSection, pszKey, szVal);
}

int32_t CMyIni::QuerySections( std::vector<std::string> &arSection )
{
    SECTIONS::iterator it(m_vecSection.begin());
    for(; it != m_vecSection.end(); ++it)
    {
        SECTION_INFO &refSection = *it;
        arSection.push_back(refSection.section);
    }
    return (int)arSection.size();
}


