/*
 * =====================================================================================
 *
 *       Filename:  JsonReader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月04日 22时08分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "JsonReader.h"
#include "json/json.h"

CJsonReader::CJsonReader()
{

}

CJsonReader::~CJsonReader()
{

}

bool CJsonReader::Init(const CMyString &jsonstr)
{
    CMyString str1 = jsonstr;
#ifdef JSONCPP_USE_OLD_WAY
    Json::Reader reader;
#else
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
#endif
    bool ret(false);
    int32_t cnt(0);
    bool dealBoolean(false);
    bool result(false);
    do {
#ifdef JSONCPP_USE_OLD_WAY
        ret = reader.parse(str1.c_str(), m_root, false);
#else
        ret = jsonReader->parse(str1.c_str(), str1.c_str() + str1.length(), &m_root, &errs); 
#endif
        if (ret)
        {
            result = true;
            break;
        }
        else
        {
            if (!dealBoolean)
            {
                // 替换不合规的boolean 值
                str1.Replace("False", "false", true);
                str1.Replace("True", "true", true);
                str1.Replace("FALSE", "false", true);
                str1.Replace("TRUE", "true", true);
                dealBoolean = true;
            }
            else
            {
#ifdef JSONCPP_USE_OLD_WAY
                CMyString strErr = reader.getFormattedErrorMessages();
                LogErr("parse json failure, error:%s", strErr.c_str());
#else
                LogErr("parse json failure, error:%s", errs.c_str());
#endif
            }
        }
        ++cnt;
    }while(cnt<2);

    return result;
}

void CJsonReader::Init(Json::Value &val)
{
    m_root = val;
}

bool CJsonReader::IsExists(const char *pszKey)
{
	return !m_root[pszKey].isNull();
}

bool CJsonReader::GetInt(const char *pszKey, int32_t &val)
{
	Json::Value jval = m_root[pszKey];
	if (jval.isNull())
	{ 
		LogDebug("not found key [%s] in json str.", pszKey);
		return false;
	}
	if (jval.isInt() || jval.isUInt())
	{
		val = jval.asInt();
		return true;
	}

	CMyString temp = jval.asString();
	val = StrToLong(temp.c_str(), 10);
	return true;
}

bool CJsonReader::GetUint32(const char *pszKey, uint32_t &val)
{
	Json::Value jval = m_root[pszKey];
	if (jval.isNull())
	{
		LogDebug("not found key [%s] in json str.", pszKey);
		return false;
	}
	if (jval.isInt() || jval.isUInt())
	{
		val = jval.asUInt();
		return true;
	}

	CMyString temp = jval.asString();
	val = StrToUint32(temp.c_str(), 10);
	return true;
}

bool CJsonReader::GetInt64(const char *pszKey, int64_t &val)
{
	Json::Value jval = m_root[pszKey];
	if (jval.isNull())
	{
		LogDebug("not found key [%s] in json str.", pszKey);
		return false;
	}
	if (jval.isInt64() || jval.isUInt64())
	{
		val = jval.asInt64();
		return true;
	}

	CMyString temp = jval.asString();
	val = StrToInt64(temp.c_str(), 10);	
	return true;
}

bool CJsonReader::GetDouble(const char *pszKey, double &val)
{
	Json::Value jval = m_root[pszKey];
	if (jval.isNull())
	{
		LogDebug("not found key [%s] in json str.", pszKey);
		return false;
	}

	if (jval.isDouble())
	{
		val = jval.asDouble();
		return true;
	}

	CMyString temp = jval.asString();
	val = StrToDouble(temp.c_str());
	return true;
}

bool CJsonReader::GetString(const char *pszKey, CMyString &val)
{
	Json::Value jval = m_root[pszKey];
	if (jval.isNull())
	{
		LogDebug("not found key [%s] in json str.", pszKey);
		return false;
	}
		
	if (!jval.isConvertibleTo(Json::stringValue))
	{
		LogWarn("key [%s] can not convertible to json stringValue.", pszKey);
		return false;
	}
	val = jval.asString();
	return true;
}

bool CJsonReader::GetBool(const char *pszKey, bool &val)
{
	Json::Value jval = m_root[pszKey];
	if (jval.isNull())
	{
		LogDebug("not found key [%s] in json str.", pszKey);
		return false;
	}
	if (jval.isBool())
	{
		val = jval.asBool();
		return true;
	}

	CMyString temp = jval.asString();
	val = StrToBoolean(temp.c_str());
	return true;
}

int32_t CJsonReader::getInt(const CMyString &strKey)
{
    int32_t val(0);
    this->GetInt(strKey.c_str(), val);
    return val;
}

uint32_t CJsonReader::getUint32(const CMyString &strKey)
{
    uint32_t val(0);
    this->GetUint32(strKey.c_str(), val);
    return val;
}

int64_t CJsonReader::getInt64(const CMyString &strKey)
{
    int64_t val(0);
    this->GetInt64(strKey.c_str(), val);
    return val;
}

uint64_t CJsonReader::getUint64(const CMyString &strKey)
{
    return (uint64_t)this->getInt64(strKey);
}

double CJsonReader::getDouble(const CMyString &strKey)
{
    double val(0);
    this->GetDouble(strKey.c_str(), val);
    return val;
}

bool CJsonReader::getBool(const CMyString &strKey)
{
    bool val(false);
    this->GetBool(strKey.c_str(), val);
    return val;
}

CMyString CJsonReader::getString(const CMyString &strKey)
{
    CMyString val;
    this->GetString(strKey.c_str(), val);
    return val;
}

const char* CJsonReader::getCString(const CMyString &strKey)
{
	Json::Value jval = m_root[strKey.c_str()];
	if (jval.isNull())
	{
		LogDebug("not found key [%s] in json str.", strKey.c_str());
		return NULL;
	}
		
	if (!jval.isConvertibleTo(Json::stringValue))
	{
		LogWarn("key [%s] can not convertible to json stringValue.", strKey.c_str());
		return NULL;
	}
	return jval.asCString();
}

bool CJsonReader::getObject(const CMyString &strKey, Json::Value &obj)
{
    obj = m_root[strKey.c_str()];
    return !(obj.isNull() );
}

bool CJsonReader::getReader(const CMyString &strKey, CJsonReader &reader)
{
    Json::Value jval = m_root[strKey.c_str()];
    if (!jval.isObject() || jval.isNull())
    {
        LogWarn("key [%s] not object or isNull", strKey.c_str());
        return false;
    }
    reader.Init(jval);

    return true;
}

