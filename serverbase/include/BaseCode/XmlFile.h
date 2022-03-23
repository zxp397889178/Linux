#ifndef _GFX_BASELIB_XMLFILE_H_
#define _GFX_BASELIB_XMLFILE_H_

#include <string>
#include <assert.h>
#include "StringFunc.h"
#include "IException.h"
#include "MyString.h"

//����tinyxml2��ϸ��
// https://blog.csdn.net/liang_baikai/article/details/78783839#%E6%94%B9
//
class CXmlFile
{
public:
    CXmlFile(void);
    ~CXmlFile(void);

    bool Load(const char *xmlfile);

    // ����xml�ļ�������ֵ��
    // 0 - δװ�سɹ������豣��
    // 1 - ����ɹ�
    // <0 ʧ��
    int32_t Save(void);

	bool Parse(const char *xmlData, size_t nLen);

//���漸�������Ƕ�tinyxml2�ļ򵥷�װ
public:
    //��ȡָ���ĸ��ڵ㣬��element_nameΪ����ȡ�׸����ڵ�
    void* GetRoot(const char *element_name = NULL);
    //��ȡָ�����ӽڵ㣬��element_nameΪ�����ȡ�׸��ӽڵ�
    void* GetChild(void *pParent, const char *element_name=NULL);
    //��ȡ��һ���ֵܽڵ㣬��element_nameΪ�����ȡ����pElement���ֵܽڵ�
    void* GetNextSibling(void *pElement, const char *element_name = NULL);
    //���ĳ��·����XMLElement,·������"abc/def/hig/kl/..."��ʽ,�Ը���㿪ʼ
    //void* GetElement(const char *element_path);
    void* GetElement(const CMyString &element_path);

    //���� attribute������
    const char* GetValue(void *pElement, const char *attribute);
	bool GetValue(void *pElement, const char *attribute, CMyString &val);
	bool GetValue(void *pElement, const char *attribute, int32_t &val);
	bool GetValue(void *pElement, const char *attribute, uint32_t &val);
	bool GetValue(void *pElement, const char *attribute, int64_t &val);
	bool GetValue(void *pElement, const char *attribute, double &val);
	bool GetValue(void *pElement, const char *attribute, bool &val);
    //��ȡtinyxml2�Ĵ���
    int32_t GetLastError(char *szText, size_t cbText);


public:
    /* ���xml��ָ��sect, attr ������
     * ����ֵ: -1 ��sect ����
     *         -3   attr ����
     *         -5   ����ת��ʧ��
     *          0   �ɹ�
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
    // ��������ֵ������ֵ
    // 1   �ɹ�
    // <=0 ʧ��
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

    //XML�еķ�ascii�ַ�Ĭ����UTF-8�ַ���,��ʱ����ͨ��pszEncoding����ת������ȡ���ַ���
    // �� pszEncoding Ϊ���ַ���,����Ϊ"utf-8",�򲻽����κ�ת��
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
    /* ֧�� /abc/def/xyz ����ڵ��ѯ��ֵ��ע��pszSection���뱣֤Ψһ
        pszSection => "abc/def/hig/..." ����Ϊxml�Ľڵ�
        pszKey => ��������ֵ
        ����ֵ: -1 ��������
                -2 XML��ʽ����
                -3 XML���ݴ���
                 0 ����ɹ�
        ˵�����˺������������к����Ļ���
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
    CMyString   m_strFile;  // XML �ļ���
};


#endif  //_GFX_BASELIB_XMLFILE_H_



