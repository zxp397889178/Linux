#ifndef _GFX_BASELIB_MYINI_H_
#define _GFX_BASELIB_MYINI_H_

#include "mydef.h"
#include <string>
#include <vector>
#include "StringFunc.h"


//能够处理UTF-8及ANSI格式的文件
class CMyIni
{
    // http://www.cnblogs.com/westsource/archive/2010/11/08/1871844.html
    enum FILE_CODEC
    {
        codecAnsi,
        codecUnicode,
        codecUnicodeBigEndian,
        codecUTF8,
        codecUCS2,
        codecUCS2BigEndian,
        codecUCS4,
        codecUCS4BigEndian,
    };
    struct KEY_VALUE
    {
        std::string                     key;
        std::string                     value;

        KEY_VALUE();
        KEY_VALUE(const char *pszKey, const char *pszValue);
        const char*     toString();
        int32_t             toInt();
        int64_t            toLong();
        uint32_t   toUint();
        float           toFloat();
        double          toDouble();
        time_t          toTime();
        bool            toBoolean();

        //扩展结构,为了方便仅传值
        TwinsInt        toTwinsInt();
        TwinsNumeric    toTwinsValue();
        TriplesInt      toTriplesInt();
        TriplesNumeric  toTriplesValue();
        QuadInt         toQuadInt();
        QuadNumeric     toQuadValue();
    };
    struct SECTION_INFO
    {
        std::string                     section;
        std::vector<KEY_VALUE>          keyinfo;

        SECTION_INFO();
        void AddKey(const char *pszKey, const char *pszValue);
        KEY_VALUE* GetKey(const char *pszKey);
        bool IsExistKey(const char *pszKey);
    };
    typedef std::vector<SECTION_INFO>   SECTIONS;

public:
    CMyIni(void);
    ~CMyIni(void);

    bool Init(const char * pszIniFile);
    bool Refresh();

    bool IsExistSection(const char *pszSection);
    bool IsExistKey(const char *pszSection, const char *pszKey);

    bool Get(const char *pszSection, const char *pszKey, char *szText, size_t cbText);
    bool Get(const char *pszSection, const char *pszKey, int32_t &val);
    bool Get(const char *pszSection, const char *pszKey, int64_t &val);
    bool Get(const char *pszSection, const char *pszKey, uint32_t &val);
    bool Get(const char *pszSection, const char *pszKey, float &val);
    bool Get(const char *pszSection, const char *pszKey, double &val);
    bool Get(const char *pszSection, const char *pszKey, bool &val);
    bool Get(const char *pszSection, const char *pszKey, TwinsInt &val);
    bool Get(const char *pszSection, const char *pszKey, TriplesInt &val);
    bool Get(const char *pszSection, const char *pszKey, QuadInt &val);
    bool Get(const char *pszSection, const char *pszKey, TwinsNumeric &val);
    bool Get(const char *pszSection, const char *pszKey, TriplesNumeric &val);
    bool Get(const char *pszSection, const char *pszKey, QuadNumeric &val);

    void Set(const char *pszSection, const char *pszKey, const char *val);
    void Set(const char *pszSection, const char *pszKey, int32_t val);
    void Set(const char *pszSection, const char *pszKey, int64_t val);
    void Set(const char *pszSection, const char *pszKey, uint32_t val);
    void Set(const char *pszSection, const char *pszKey, float val);
    void Set(const char *pszSection, const char *pszKey, double val);
    void Set(const char *pszSection, const char *pszKey, bool val);
    void Set(const char *pszSection, const char *pszKey, const TwinsInt &val);
    void Set(const char *pszSection, const char *pszKey, const TriplesInt &val);
    void Set(const char *pszSection, const char *pszKey, const QuadInt &val);
    void Set(const char *pszSection, const char *pszKey, const TwinsNumeric &val);
    void Set(const char *pszSection, const char *pszKey, const TriplesNumeric &val);
    void Set(const char *pszSection, const char *pszKey, const QuadNumeric &val);

    //该函数存在编码转换的问题,需要重置
    bool Save(const char *pszFile);

    //获取该ini中所的section
    int32_t  QuerySections( std::vector<std::string> &arSection );

protected:
    bool Load(void);

private:
    bool IsSectionStr(const char *pszString);
    void GetSection(const char *pszString, char *szSection, size_t cbSection);
    bool IsKeyValueStr(const char *pszString);
    void DecodeKeyValue(const char *pszString, char *szKey, size_t cbKey, char *szValue, size_t cbValue);

    SECTION_INFO* FindSection(const char *pszSection);
    KEY_VALUE* FindKey(const char *pszSection, const char *pszKey);

    int32_t QueryFileCodec(const char *buffer);

private:
    SECTIONS                            m_vecSection;
    std::string                         m_strIniFile;
    enum FILE_CODEC                     m_eFileCodec;
};

#endif  //_GFX_BASELIB_MYINI_H_



