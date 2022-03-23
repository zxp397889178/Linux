#ifndef _GFX_BASELIB_XMLFILE_H_
#define _GFX_BASELIB_XMLFILE_H_

#include <string>
#include <assert.h>
#include "StringFunc.h"
#include "IException.h"
#include "MyString.h"

//隐藏tinyxml2的细节
// https://blog.csdn.net/liang_baikai/article/details/78783839#%E6%94%B9
//
class CXmlFile
{
public:
    CXmlFile(void);
    ~CXmlFile(void);

    bool Load(const char *xmlfile);

    // 保存xml文件，返回值：
    // 0 - 未装载成功，无需保存
    // 1 - 保存成功
    // <0 失败
    int32_t Save(void);

	bool Parse(const char *xmlData, size_t nLen);

//下面几个函数是对tinyxml2的简单封装
public:
    //获取指定的根节点，若element_name为空则取首个根节点
    void* GetRoot(const char *element_name = NULL);
    //获取指定的子节点，若element_name为空则获取首个子节点
    void* GetChild(void *pParent, const char *element_name=NULL);
    //获取下一个兄弟节点，若element_name为空则获取紧接pElement的兄弟节点
    void* GetNextSibling(void *pElement, const char *element_name = NULL);
    //获得某个路径的XMLElement,路径是以"abc/def/hig/kl/..."形式,以根结点开始
    //void* GetElement(const char *element_path);
    void* GetElement(const CMyString &element_path);

    //返回 attribute的内容
    const char* GetValue(void *pElement, const char *attribute);
	bool GetValue(void *pElement, const char *attribute, CMyString &val);
	bool GetValue(void *pElement, const char *attribute, int32_t &val);
	bool GetValue(void *pElement, const char *attribute, uint32_t &val);
	bool GetValue(void *pElement, const char *attribute, int64_t &val);
	bool GetValue(void *pElement, const char *attribute, double &val);
	bool GetValue(void *pElement, const char *attribute, bool &val);
    //获取tinyxml2的错误
    int32_t GetLastError(char *szText, size_t cbText);


public:
    /* 获得xml中指定sect, attr 的内容
     * 返回值: -1 　sect 错误
     *         -3   attr 错误
     *         -5   类型转换失败
     *          0   成功
     * */
    int32_t getString(const CMyString &sect, CMyString &val);
    int32_t getString(const CMyString &sect, const CMyString &attr, CMyString &val);
    int32_t getInt32(const CMyString &sect, const CMyString &attr, int32_t &val);
    int32_t getUInt32(const CMyString &sect, const CMyString &attr, uint32_t &val);
    int32_t getInt64(const CMyString &sect, const CMyString &attr, int64_t &val);
    int32_t getFloat(const CMyString &sect, const CMyString &attr, float &val);
    int32_t getDouble(const CMyString &sect, const CMyString &attr, double &val);
    int32_t getBoolean(const CMyString &sect, const CMyString &attr, bool &val);

public:
    // 设置属性值，返回值
    // 1   成功
    // <=0 失败
    bool SetValue(void *pElement, const CMyString &attribute, const CMyString &val);
    bool SetValue(const CMyString &section, const CMyString &attribute, const CMyString &val);
    bool SetValue(void *pElement, const CMyString &attr, int32_t val);
    bool SetValue(void *pElement, const CMyString &attr, uint32_t val);
    bool SetValue(void *pElement, const CMyString &attr, int64_t val);
    bool SetValue(void *pElement, const CMyString &attr, bool val);
    bool SetValue(void *pElement, const CMyString &attr, double val);

public:
    bool            GetBool(const char *pszSection, const char *pszKey);
    int32_t         GetLong(const char *pszSection, const char *pszKey);
    double          GetDouble(const char *pszSection, const char *pszKey);

    //XML中的非ascii字符默认是UTF-8字符的,此时可以通过pszEncoding进行转换代码取出字符串
    // 若 pszEncoding 为空字符串,或者为"utf-8",则不进行任何转换
    std::string     GetString(const char *pszSection, const char *pszKey, const char *pszEncoding = NULL);

    TwinsInt        GetTwinsInt(const char *pszSection, const char *pszKey);
    TriplesInt      GetTriplesInt(const char *pszSection, const char *pszKey);
    QuadInt         GetQuadInt(const char *pszSection, const char *pszKey);
    TwinsNumeric    GetTwinsNumeric(const char *pszSection, const char *pszKey);
    TriplesNumeric  GetTriplesNumeric(const char *pszSection, const char *pszKey);
    QuadNumeric     GetQuadNumeric(const char *pszSection, const char *pszKey);

public:
    bool DelNode(void *pParent, void *pNode);
    bool DelAllChild(void *pNode);
    void* AddChild(void *pParent, const CMyString &attr);

private:
    /* 支持 /abc/def/xyz 逐个节点查询的值，注意pszSection必须保证唯一
        pszSection => "abc/def/hig/..." 必须为xml的节点
        pszKey => 内置属性值
        返回值: -1 参数错误
                -2 XML格式错误
                -3 XML内容错误
                 0 处理成功
        说明：此函数是下面所有函数的基础
    */
    int32_t GetString(const char *pszSection, const char *pszKey, char *szValue, size_t cbValue);

    template<typename T, typename F>
    T GetData(const char *pszSection, const char *pszKey)
    {
        char szValue[128] = "";
        int32_t ret = this->GetString(pszSection, pszKey, szValue, sizeof(szValue));
        if (0 == ret)
        {
            return F()(szValue);
        }
		throw Bad_Config(pszSection, pszKey, ret);
        return F()("");
    }

private:
    int32_t getXmlAttribute(const CMyString &sect, const CMyString &attr, void *&pAttr);

private:
    void*       m_pDocument;
    CMyString   m_strFile;  // XML 文件名
};


#endif  //_GFX_BASELIB_XMLFILE_H_



