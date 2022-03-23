#include "XmlFile.h"
#include "StringFunc.h"
#include "tinyxml2.h"
#include "Codec.h"
#include "mydef.h"
#include <assert.h>
#include <vector>
#include <string>
#include "iLog.h"
#include "StrSpliter.h"

using namespace tinyxml2;
typedef std::vector< std::string >  StringArray;		


struct xml2Bool
{
    bool operator()(const char *val) const
    {
        return StrToBoolean(val);
    }
};

struct xml2Long
{
    int32_t operator()(const char *val) const
    {
        char *end_string(NULL);
        return strtol(val, &end_string, 0);
    }
};

struct xml2Double
{
    double operator()(const char *val) const
    {
        char *end_string(NULL);
        return strtod(val, &end_string);
    }
};

struct xml2Twinsint
{
    TwinsInt operator()(const char *val) const
    {
        TwinsInt result;
        if (StrToTwinsInt(val, result))
        {
            return result;
        }
        ASSERT(0);
        return result;
    }
};

struct xml2Triplesint
{
    TriplesInt operator()(const char *val) const
    {
        TriplesInt result;
        if (StrToTriplesInt(val, result))
        {
            return result;
        }
		printf("val=[%s]\n", val);
        ASSERT(0);
        return result;
    }
};

struct xml2Quadint
{
    QuadInt operator()(const char *val) const
    {
        QuadInt result;
        if (StrToQuadInt(val, result))
        {
            return result;
        }
        ASSERT(0);
        return result;
    }
};

struct xml2TwinsNumeric
{
    TwinsNumeric operator()(const char *val) const
    {
        TwinsNumeric result;
        if (StrToTwinsNumeric(val, result))
        {
            return result;
        }
        ASSERT(0);
        return result;
    }
};

struct xml2TriplesNumeric
{
    TriplesNumeric operator()(const char *val) const
    {
        TriplesNumeric result;
        if (StrToTriplesNumeric(val, result))
        {
            return result;
        }
        ASSERT(0);
        return result;
    }
};

struct xml2QuadNumeric
{
    QuadNumeric operator()(const char *val) const
    {
        QuadNumeric result;
        if (StrToQuadNumeric(val, result))
        {
            return result;
        }
        ASSERT(0);
        return result;
    }
};

//////////////////////////////////////////////////////////////////////////
//CXmlFile
CXmlFile::CXmlFile(void)
    : m_pDocument(NULL)
{
    m_pDocument = new tinyxml2::XMLDocument();
    ASSERT(m_pDocument!=NULL);
}


CXmlFile::~CXmlFile(void)
{
    tinyxml2::XMLDocument *pDoc = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    SAFE_DELETE(pDoc);
}

bool CXmlFile::Load(const char *xmlfile)
{
    if (EMPTY_STR(xmlfile))
    {
        return false;
    }

    tinyxml2::XMLDocument *pDocument = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    ASSERT(pDocument != NULL);
    pDocument->Clear();
    tinyxml2::XMLError errNo = pDocument->LoadFile(xmlfile);
    if (errNo != tinyxml2::XML_SUCCESS)
    {
        return false;
    }

    m_strFile = xmlfile;

    return true;
}

int32_t CXmlFile::Save(void)
{
    if (m_strFile.empty())
    {
        return 0;
    }

    tinyxml2::XMLDocument *pDocument = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    if (NULL == pDocument)
    {
        return -1;
    }
    int32_t ret = pDocument->SaveFile(m_strFile.c_str());
    return (ret==0) ? 1 : -2;
}

bool CXmlFile::Parse(const char *xmlData, size_t nLen)
{
	if (EMPTY_STR(xmlData))
    {
        return false;
    }
	
	tinyxml2::XMLDocument *pDocument = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
	ASSERT(pDocument != NULL);
    pDocument->Clear();
	tinyxml2::XMLError errNo = pDocument->Parse(xmlData, nLen);
	return  errNo == tinyxml2::XML_SUCCESS;
}


int32_t CXmlFile::GetLastError(char *szText, size_t cbText)
{
    tinyxml2::XMLDocument *pDocument = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    ASSERT(pDocument != NULL);

    if (pDocument->Error())
    {
        int32_t nErrorId = pDocument->ErrorID();
        safe_sprintf(szText, cbText, "Error:(%d) %s", 
            nErrorId, 
            SAFE_STR(pDocument->ErrorStr()));
        return nErrorId;
    }
    return 0;
}

void* CXmlFile::GetRoot(const char *element_name /* = NULL */)
{
    tinyxml2::XMLDocument *pDocument = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    ASSERT(pDocument != NULL);
    if (EMPTY_STR(element_name))
    {
        return pDocument->RootElement();
    }
    tinyxml2::XMLElement *pRoot = pDocument->RootElement();
    while(pRoot != NULL)
    {
        if (!strcmp(pRoot->Value(), element_name))
        {
            return pRoot;
        }
        pRoot = pRoot->NextSiblingElement();
    }
    return NULL;
}

/*
void* CXmlFile::GetElement(const char *element_path)
{
    if (EMPTY_STR(element_path))
    {
		LogErr("element path is EMPTY.\n");
        return NULL;
    }
    StringArray arElement;
    int32_t size = safe_strtok(element_path, arElement, "/");
    if (0 == size)
    {
		LogErr("can not decode element_path [%s]\n", element_path);
        return NULL;
    }

    //确定根结点
    tinyxml2::XMLDocument *pDocument = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    ASSERT(pDocument!=NULL);
    StringArray::iterator it(arElement.begin());
    tinyxml2::XMLElement *pElement = pDocument->RootElement();
    while(pElement != NULL)
    {
        if (!strcmp(pElement->Value(), it->c_str()) )
        {
            break;
        }
        pElement = pElement->NextSiblingElement();
    }
    if (NULL == pElement)
    {
		LogErr("not found root element\n");
        return NULL;
    }

    //遍历以寻找路径中指定的结点
    for(++it; it != arElement.end() && pElement!=NULL; ++it)
    {
        pElement = pElement->FirstChildElement(it->c_str());
    }

    arElement.clear();
    return pElement;
}
*/

void* CXmlFile::GetElement(const CMyString &element_path)
{
    tinyxml2::XMLDocument *pDocument = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    if (NULL == pDocument)
    {
        return NULL;
    }
    tinyxml2::XMLElement *pRoot = pDocument->RootElement();
    if (NULL == pRoot)
    {
        return NULL;
    }
    if (element_path.empty())
    {
        return pRoot;
    }

    // 解析字符串
    CStrSpliter css;
    if (!css.Split(element_path, '/'))
    {
        return NULL;
    }

    // 定位第一个根目录
    int32_t idx(0);
    do{
        CMyString val1 = css[idx];
        val1.trim();
        if (!val1.empty())
        {
            break;
        }
        ++idx;
    }while(idx<css.size());
    if (idx >= css.size())
    {
        return NULL;
    }

    // 查找第一级目录
    tinyxml2::XMLElement *pItem = pRoot;
    while(pItem != NULL)
    {
        if (pItem->Value() == css[idx])
        {
            break;
        }
        pItem = pItem->NextSiblingElement();
    }
    if (NULL == pItem)
    {
        return NULL;
    }

    // 遍历查找路径
    while( (++idx<css.size()) && (pItem!=NULL) )
    {
        CMyString val2 = css[idx];
        val2.trim();
        if (val2.empty())
        {
            continue;
        }
        pItem = pItem->FirstChildElement(val2.c_str());
    }
    if (idx < css.size())
    {
        return NULL;
    }

    return pItem;
}

void* CXmlFile::GetChild(void* pParent, 
    const char *element_name/* =NULL */)
{
    tinyxml2::XMLElement *super = static_cast<tinyxml2::XMLElement*>(pParent);
    ASSERT(super);

    return super->FirstChildElement(element_name);
}

void* CXmlFile::GetNextSibling(void* pElement,
    const char *element_name /* = NULL */)
{
    tinyxml2::XMLElement *pBrother = static_cast<tinyxml2::XMLElement*>(pElement);
    ASSERT(pBrother);

    return pBrother->NextSiblingElement(element_name);
}

const char* CXmlFile::GetValue(void *pElement, const char *attribute)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
    ASSERT(pElementPtr);

	const char *pVal = pElementPtr->Attribute(attribute);
	if (pVal == NULL)
	{
		throw Bad_Config(pElementPtr->Name(), attribute, -1);
	}
    return pVal;
}

bool CXmlFile::GetValue(void *pElement, const char *attribute, CMyString &val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

	const char *pVal = pElementPtr->Attribute(attribute);
	if (NULL == pVal)
	{
		return false;
	}
	val =  pVal;
	return true;
}

bool CXmlFile::GetValue(void *pElement, const char *attribute, int32_t &val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

	const char *pVal = pElementPtr->Attribute(attribute);
	if (NULL == pVal)
	{
		return false;
	}

	val = StrToInt32(pVal, 10);
	return true;
}

bool CXmlFile::GetValue(void *pElement, const char *attribute, int64_t &val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

	const char *pVal = pElementPtr->Attribute(attribute);
	if (NULL == pVal)
	{
		return false;
	}

	val = StrToInt64(pVal, 10);
	return true;
}

bool CXmlFile::GetValue(void *pElement, const char *attribute, uint32_t &val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

	const char *pVal = pElementPtr->Attribute(attribute);
	if (NULL == pVal)
	{
		return false;
	}

	val = StrToUint32(pVal, 10);
	return true;
}

bool CXmlFile::GetValue(void *pElement, const char *attribute, double &val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

	const char *pVal = pElementPtr->Attribute(attribute);
	if (NULL == pVal)
	{
		return false;
	}

	val = StrToDouble(pVal);
	return true;
}


bool CXmlFile::GetValue(void *pElement, const char *attribute, bool &val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

	const char *pVal = pElementPtr->Attribute(attribute);
	if (NULL == pVal)
	{
		return false;
	}

	val = StrToBoolean(pVal);
	return true;
}

int32_t CXmlFile::GetString(const char *pszSection, const char *pszKey, char *szValue, size_t cbValue)
{
    if (EMPTY_STR(pszSection) || EMPTY_STR(pszKey) || NULL==szValue || cbValue==0)
    {
        //参数错误
        return -1;
    }

	void *pRoot = this->GetElement(pszSection);
	if (NULL == pRoot)
	{
		return -2;
	}
    tinyxml2::XMLElement *pElement = static_cast<tinyxml2::XMLElement*>(pRoot);
    if (NULL == pElement)
    {
        return -3;
    }

    //赋值
    int32_t ret = safe_strcpy(szValue, cbValue, pElement->Attribute(pszKey));
    if (ret < 0)
    {
        //参数错误，空间不足
        return -4;
    }
    return 0;
}

int32_t CXmlFile::getString(const CMyString &sect, CMyString &val)
{
    void *pRoot = this->GetElement(sect.c_str());
    if (NULL == pRoot)
    {
        return -1;
    }
    tinyxml2::XMLElement *pElement = static_cast<tinyxml2::XMLElement*>(pRoot);
    if (NULL == pElement)
    {
        return -3;
    }
    val = pElement->GetText();
    return 0;
}

int32_t CXmlFile::getXmlAttribute(const CMyString &sect, const CMyString &attr, void *&pAttr)
{
    void *pRoot = this->GetElement(sect.c_str());
    if (NULL == pRoot)
    {
        return -1;
    }
    tinyxml2::XMLElement *pElement = static_cast<tinyxml2::XMLElement*>(pRoot);
    if (NULL == pElement)
    {
        return -1;
    }
    const tinyxml2::XMLAttribute *pAttrTemp = pElement->FindAttribute(attr.c_str()); 
    if (NULL == pAttrTemp)
    {
        // 无此attribute
        return -3;
    }

    pAttr = (void*)(pAttrTemp);
    return 0;
}

int32_t CXmlFile::getString(const CMyString &sect, const CMyString &attr, CMyString &val)
{
    void *ptr(NULL);
    int32_t ret = this->getXmlAttribute(sect, attr, ptr);
    if (ret != 0) 
    {
        return ret;
    }
    tinyxml2::XMLAttribute *pAttr = (tinyxml2::XMLAttribute*)(ptr);
    val = pAttr->Value();
    return 0;
}

int32_t CXmlFile::getInt32(const CMyString &sect, const CMyString &attr, int32_t &val)
{
    void *ptr(NULL);
    int32_t ret = this->getXmlAttribute(sect, attr, ptr);
    if (ret != 0) 
    {
        return ret;
    }
    XMLAttribute *pAttr = (XMLAttribute*)(ptr);
    XMLError err = pAttr->QueryIntValue(&val);
    if (err != XML_SUCCESS)
    {
        return -5;
    }
    return 0;
}

int32_t CXmlFile::getUInt32(const CMyString &sect, const CMyString &attr, uint32_t &val)
{
    void *ptr(NULL);
    int32_t ret = this->getXmlAttribute(sect, attr, ptr);
    if (ret != 0) 
    {
        return ret;
    }
    XMLAttribute *pAttr = (XMLAttribute*)(ptr);
    XMLError err = pAttr->QueryUnsignedValue(&val);
    if (err != XML_SUCCESS)
    {
        return -5;
    }
    return 0;
}

int32_t CXmlFile::getInt64(const CMyString &sect, const CMyString &attr, int64_t &val)
{
    void *ptr(NULL);
    int32_t ret = this->getXmlAttribute(sect, attr, ptr);
    if (ret != 0) 
    {
        return ret;
    }
    XMLAttribute *pAttr = (XMLAttribute*)(ptr);
    tinyxml2::XMLError err = pAttr->QueryInt64Value(&val);
    if (err != XML_SUCCESS)
    {
        return -5;
    }
    return 0;
}

int32_t CXmlFile::getFloat(const CMyString &sect, const CMyString &attr, float &val)
{
    void *ptr(NULL);
    int32_t ret = this->getXmlAttribute(sect, attr, ptr);
    if (ret != 0) 
    {
        return ret;
    }
    XMLAttribute *pAttr = (XMLAttribute*)(ptr);
    tinyxml2::XMLError err = pAttr->QueryFloatValue(&val);
    if (err != XML_SUCCESS)
    {
        return -5;
    }
    return 0;
}

int32_t CXmlFile::getDouble(const CMyString &sect, const CMyString &attr, double &val)
{
    void *ptr(NULL);
    int32_t ret = this->getXmlAttribute(sect, attr, ptr);
    if (ret != 0) 
    {
        return ret;
    }
    XMLAttribute *pAttr = (XMLAttribute*)(ptr);
    tinyxml2::XMLError err = pAttr->QueryDoubleValue(&val);
    if (err != XML_SUCCESS)
    {
        return -5;
    }
    return 0;
}

int32_t CXmlFile::getBoolean(const CMyString &sect, const CMyString &attr, bool &val)
{
    void *ptr(NULL);
    int32_t ret = this->getXmlAttribute(sect, attr, ptr);
    if (ret != 0) 
    {
        return ret;
    }
    XMLAttribute *pAttr = (XMLAttribute*)(ptr);
    tinyxml2::XMLError err = pAttr->QueryBoolValue(&val);
    if (err != XML_SUCCESS)
    {
        return -5;
    }
    return 0;
}

bool CXmlFile::GetBool(const char *pszSection, const char *pszKey)
{
    return GetData<bool, xml2Bool>(pszSection, pszKey);
}

int32_t CXmlFile::GetLong(const char *pszSection, const char *pszKey)
{
    return GetData<long, xml2Long>(pszSection, pszKey);
}

double CXmlFile::GetDouble(const char *pszSection, const char *pszKey)
{
    return GetData<double, xml2Double>(pszSection, pszKey);
}

std::string CXmlFile::GetString(const char *pszSection, const char *pszKey, const char *pszEncoding/* = "gbk"*/)
{
    char szValue[2048] = "";
	int32_t ret = this->GetString(pszSection, pszKey, szValue, sizeof(szValue));
	if (0 == ret)
    {
        bool bNoConvert(true);
        if (!EMPTY_STR(pszEncoding) && ::IsUTF8String(szValue, strlen(szValue)) )
        {
            bNoConvert = false;
        }

        if (!bNoConvert)
        {
            char szValueOut[4096] = "";
            size_t cbValueOut = sizeof(szValueOut);
            if (0 == Convert("utf-8", pszEncoding, szValueOut, cbValueOut, szValue, strlen(szValue) ) )
            {
                return std::string(szValueOut);
            }
        }
        return std::string(szValue);
    }
	throw Bad_Config(pszSection, pszKey, ret);
    return std::string("");
}

TwinsInt CXmlFile::GetTwinsInt(const char *pszSection, const char *pszKey)
{
    return GetData<TwinsInt, xml2Twinsint>(pszSection, pszKey);
}

TriplesInt CXmlFile::GetTriplesInt(const char *pszSection, const char *pszKey)
{
    return GetData<TriplesInt, xml2Triplesint>(pszSection, pszKey);
}

QuadInt CXmlFile::GetQuadInt(const char *pszSection, const char *pszKey)
{
    return GetData<QuadInt, xml2Quadint>(pszSection, pszKey);
}

TwinsNumeric CXmlFile::GetTwinsNumeric(const char *pszSection, const char *pszKey)
{
    return GetData<TwinsNumeric, xml2TwinsNumeric>(pszSection, pszKey);
}

TriplesNumeric CXmlFile::GetTriplesNumeric(const char *pszSection, const char *pszKey)
{
    return GetData<TriplesNumeric, xml2TriplesNumeric>(pszSection, pszKey);
}

QuadNumeric CXmlFile::GetQuadNumeric(const char *pszSection, const char *pszKey)
{
    return GetData<QuadNumeric, xml2QuadNumeric>(pszSection, pszKey);
}

bool CXmlFile::SetValue(void *pElement, const CMyString &attribute, const CMyString &val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return -1;
	}

    pElementPtr->SetAttribute(attribute.c_str(), val.c_str());
    return 1;
}

bool CXmlFile::SetValue(const CMyString &section, const CMyString &attribute, const CMyString &val)
{
//    char szAge[64] = "";
//    int32_t val1 = this->GetString(section.c_str(), attribute.c_str(), szAge, sizeof(szAge));
//    printf("query ret=%d, value=%s\n", val1, szAge);
//
	void *pRoot = this->GetElement(section.c_str());
	if (NULL == pRoot)
	{
		return false;
	}
    tinyxml2::XMLElement *pElement = static_cast<tinyxml2::XMLElement*>(pRoot);
    if (NULL == pElement)
    {
        return false;
    }

    pElement->SetAttribute(attribute.c_str(), val.c_str());
    return true;
}

bool CXmlFile::SetValue(void *pElement, const CMyString &attr, int32_t val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

    pElementPtr->SetAttribute(attr.c_str(), val);
	return true;
}

bool CXmlFile::SetValue(void *pElement, const CMyString &attr, uint32_t val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

    pElementPtr->SetAttribute(attr.c_str(), val);
	return true;
}

bool CXmlFile::SetValue(void *pElement, const CMyString &attr, int64_t val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

    pElementPtr->SetAttribute(attr.c_str(), val);
	return true;
}

bool CXmlFile::SetValue(void *pElement, const CMyString &attr, double val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

    pElementPtr->SetAttribute(attr.c_str(), val);
	return true;
}

bool CXmlFile::SetValue(void *pElement, const CMyString &attr, bool val)
{
    tinyxml2::XMLElement *pElementPtr = static_cast<tinyxml2::XMLElement*>(pElement);
	if (NULL == pElementPtr)
	{
		return false;
	}

    pElementPtr->SetAttribute(attr.c_str(), val);
	return true;
}

bool CXmlFile::DelNode(void *pParent, void *pNode)
{
    tinyxml2::XMLNode *_parent = static_cast<tinyxml2::XMLNode*>(pParent);
    if (NULL == _parent)
    {
        return false;
    }
    tinyxml2::XMLNode *_node = static_cast<tinyxml2::XMLNode*>(pNode);
    if (NULL == _node)
    {
        return false;
    }
    _parent->DeleteChild(_node);
    return true;
}

bool CXmlFile::DelAllChild(void *pNode)
{
    tinyxml2::XMLNode *_parent = static_cast<tinyxml2::XMLNode*>(pNode);
    if (NULL == _parent)
    {
        return false;
    }
    _parent->DeleteChildren();
    return true;
}

void* CXmlFile::AddChild(void *pParent, const CMyString &attr)
{
    tinyxml2::XMLDocument *pDoc = static_cast<tinyxml2::XMLDocument*>(m_pDocument);
    if (NULL == pDoc)
    {
        return NULL;
    }
    XMLElement* pElement = pDoc->NewElement(attr.c_str());
    if (NULL == pElement)
    {
        return NULL;
    }
    tinyxml2::XMLNode *_parent = static_cast<tinyxml2::XMLNode*>(pParent);
    if (NULL == _parent)
    {
        return NULL;
    }
    return _parent->InsertEndChild(pElement);
}
