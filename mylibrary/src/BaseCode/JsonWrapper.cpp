/*
 * =====================================================================================
 *
 *       Filename:  JsonWrapper.cpp
 *
 *    Description:  对jsoncpp的读写的封装
 *
 *        Version:  1.0
 *        Created:  2021年07月28日 02时00分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "JsonWrapper.h"
#include "TypeInfo.h"

CJsonWrapper::CJsonWrapper()
{
}

CJsonWrapper::CJsonWrapper(const CMyString &str)
{
    this->Load(str);
}

CJsonWrapper::CJsonWrapper(Json::Value &val)
{
    this->Load(val);
}

CJsonWrapper::~CJsonWrapper()
{
}

bool CJsonWrapper::StringToJson(const CMyString &str, Json::Value &json_val)
{
    CMyString tempStr(str);
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    
    bool result(false);
    int32_t cnt(0);
    do {
        bool ret = jsonReader->parse(tempStr.c_str(), tempStr.c_str() + tempStr.length(), &json_val, &errs); 
        if (ret) {
            result = true;
            break;
        }
        else
        {
            if (cnt==0)
            {
                tempStr.Replace("False", "false", false);
                tempStr.Replace("True", "true", false);
            }
            else
            {
                LogErr("parse json falure, error:%s, string:%s",
                        errs.c_str(), str.c_str());
            }
        }
    }while(++cnt<2);
    return result;
}

bool CJsonWrapper::Load(const CMyString &str)
{
    return this->StringToJson(str, m_root);
}

void CJsonWrapper::Load(Json::Value &val)
{
    m_root = val;
}

bool CJsonWrapper::Append(const CMyString &json_str, bool overrideSameKey)
{
    Json::Value tempJson;
    if (!this->StringToJson(json_str, tempJson))
    {
        return false;
    }
    this->Append(tempJson, overrideSameKey);
    return true;
}

void CJsonWrapper::Append(Json::Value &json_val, bool overrideSameKey)
{
    //在 m_root 后附加 json_val， 忽略相同key值
    Json::Value::Members members = json_val.getMemberNames();
    for(Json::Value::Members::iterator it(members.begin());
            it != members.end(); ++it)
    {
        Json::String &key = *it;
        if (overrideSameKey)
        {
            m_root[key] = json_val[key];
        }
        else
        {
            if (!m_root.isMember(key))
            {
                m_root[key] = json_val[key];
            }
        }
    }
}


bool CJsonWrapper::getJsonObject(const CMyString &key, Json::Value &json_val)
{
    return this->Read(key, json_val);
}

static bool CanConvertToNumber(Json::ValueType &jType)
{
    if (jType==Json::intValue || 
        jType==Json::uintValue || 
        jType==Json::realValue || 
        jType==Json::nullValue || 
        jType==Json::booleanValue)
    {
        return true;
    }
    return false;
    
}

bool CJsonWrapper::Read(const CMyString &key, int32_t &val)
{
    val = 0;
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType))
    {
        val = jVal.asInt();
    }
    else if (jType == Json::stringValue)
    {
        val = CMyString(jVal.asString()).asInt();
    }
    else
    {
        result = false;
    }
    return result;
}

bool CJsonWrapper::Read(const CMyString &key, uint32_t &val)
{
    val = 0;
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType))
    {
        val = jVal.asUInt();
    }
    else if (jType == Json::stringValue)
    {
        val = CMyString(jVal.asString()).asUint();
    }
    else
    {
        result = false;
    }
    return result;
}

bool CJsonWrapper::Read(const CMyString &key, int64_t &val)
{
    val = 0;
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType))
    {
        val = jVal.asInt64();
    }
    else if (jType == Json::stringValue)
    {
        val = CMyString(jVal.asString()).asInt64();
    }
    else
    {
        result = false;
    }
    return result;
}

bool CJsonWrapper::Read(const CMyString &key, uint64_t &val)
{
    val = 0;
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType))
    {
        val = jVal.asUInt64();
    }
    else if (jType == Json::stringValue)
    {
        val = CMyString(jVal.asString()).asUint64();
    }
    else
    {
        result = false;
    }
    return result;
}

bool CJsonWrapper::Read(const CMyString &key, float &val)
{
    val = 0;
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType))
    {
        val = jVal.asFloat();
    }
    else if (jType == Json::stringValue)
    {
        val = CMyString(jVal.asString()).asDouble();
    }
    else
    {
        result = false;
    }
    return result;
}

bool CJsonWrapper::Read(const CMyString &key, double &val)
{
    val = 0;
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType))
    {
        val = jVal.asDouble();
    }
    else if (jType == Json::stringValue)
    {
        val = CMyString(jVal.asString()).asDouble();
    }
    else
    {
        result = false;
    }
    return result;
}

bool CJsonWrapper::Read(const CMyString &key, bool &val)
{
    val = false;
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType))
    {
        val = jVal.asBool();
    }
    else if (jType == Json::stringValue)
    {
        CMyString tempStr = jVal.asString();
        val = StrToBoolean(tempStr.c_str());
    }
    else
    {
        result = false;
    }
    return result;
}
bool CJsonWrapper::Read(const CMyString &key, CMyString &str)
{
    if (!this->IsExists(key))
    {
        return false;
    }
    Json::Value &jVal = m_root[key];
    if (jVal.isNull())
    {
        return false;
    }

    bool result(true);
    Json::ValueType jType = jVal.type();
    if (CanConvertToNumber(jType) || jType == Json::stringValue)
    {
        str = jVal.asString();
    }
    else if (jType==Json::arrayValue || jType==Json::objectValue)
    {
        str = jVal.toStyledString();
        str.trim_endl();
    }
    else
    {
        result = false;
    }
    return result;
}

bool CJsonWrapper::Read(const CMyString &key, char *str, size_t cbLen)
{
    CMyString tempStr;
    if (!this->Read(key, tempStr))
    {
        return false;
    }
    if (cbLen < tempStr.length())
    {
        return false;
    }
    safe_strcpy(str, cbLen, tempStr.c_str());
    return true;
}

bool CJsonWrapper::Read(const CMyString &key, Json::Value &val)
{
    if (!this->IsExists(key))
    {
        return false;
    }
    val = m_root[key];
    if (val.isNull())
    {
        return false;
    }
    return true;
}

Json::Value& CJsonWrapper::operator[](const CMyString &key)
{
    return m_root[key];
}

void CJsonWrapper::Write(const CMyString &key, int32_t val)
{
    m_root[key] = val;
}

void CJsonWrapper::Write(const CMyString &key, uint32_t val)
{
    m_root[key] = val;
}

void CJsonWrapper::Write(const CMyString &key, int64_t val)
{
    m_root[key] = (Json::Int64)val;
}

void CJsonWrapper::Write(const CMyString &key, uint64_t val)
{
    m_root[key] = (Json::UInt64)val;
}

void CJsonWrapper::Write(const CMyString &key, float val)
{
    m_root[key] = val;
}

void CJsonWrapper::Write(const CMyString &key, double val)
{
    m_root[key] = val;
}

void CJsonWrapper::Write(const CMyString &key, bool val)
{
    m_root[key] = val;
}

void CJsonWrapper::Write(const CMyString &key, const CMyString &str)
{
    m_root[key] = str;
}

void CJsonWrapper::Write(const CMyString &key, const char *str)
{
    m_root[key] = str;
}

void CJsonWrapper::Write(const CMyString &key, const Json::Value &val)
{
    m_root[key] = val;
}

bool CJsonWrapper::IsExists(const CMyString &key)
{
    return m_root.isMember(key);
}

void CJsonWrapper::Remove(const CMyString &key)
{
    m_root.removeMember(key);
}

void CJsonWrapper::Clear(void)
{
    m_root.clear();
}

CMyString CJsonWrapper::toString(void)
{
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";
    CMyString tempStr = Json::writeString(builder, m_root);
    return tempStr;
}

CMyString CJsonWrapper::toStyledString(void)
{
    Json::StreamWriterBuilder builder;
    CMyString tempStr = Json::writeString(builder, m_root);
    return tempStr;
}

int32_t CJsonWrapper::getInt(const CMyString &key, int32_t defVal)
{
    int32_t val(0);
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

uint32_t CJsonWrapper::getUInt(const CMyString &key, uint32_t defVal)
{
    uint32_t val(0);
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

int64_t CJsonWrapper::getInt64(const CMyString &key, int64_t defVal)
{
    int64_t val(0);
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

uint64_t CJsonWrapper::getUInt64(const CMyString &key, uint64_t defVal)
{
    uint64_t val(0);
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

bool CJsonWrapper::getBool(const CMyString &key, bool defVal)
{
    bool val;
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

float CJsonWrapper::getFloat(const CMyString &key, float defVal)
{
    float val(0);
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

double CJsonWrapper::getDouble(const CMyString &key, double defVal)
{
    double val(0);
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

CMyString CJsonWrapper::getString(const CMyString &key, const CMyString &defVal)
{
    CMyString val;
    if (!this->Read(key, val))
    {
        val = defVal;
    }
    return val; 
}

CMyString CJsonWrapper::write(Json::Value &val)
{
    this->Load(val);
    return this->toString();
}



