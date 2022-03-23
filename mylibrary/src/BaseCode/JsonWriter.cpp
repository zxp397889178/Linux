/*
 * =====================================================================================
 *
 *       Filename:  JsonWriter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 14时39分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#include "JsonWriter.h"

CJsonWriter::CJsonWriter()
{

}

CJsonWriter::~CJsonWriter()
{

}

bool CJsonWriter::AddJsonStr(const CMyString &strJson)
{
    Json::Value root;
    bool ret(false);
#ifdef JSONCPP_USE_OLD_WAY
    Json::Reader reader;
    ret = reader.parse(strJson, root, false);
    if (!ret)
    {
        CMyString strErr = reader.getFormattedErrorMessages();
        LogErr("string [%s] not valid json string, error:%s", strJson.c_str(), strErr.c_str());
        return false;
    }
#else
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    std::unique_ptr<Json::CharReader> const jsonReader(readerBuilder.newCharReader());
    ret = jsonReader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &errs); 
    if (!ret)
    {
        LogErr("string [%s] not valid json string, error:%s", strJson.c_str(), errs.c_str());
        return false;
    }
#endif
    m_root.append(root);
    return true;
}

void CJsonWriter::Set(const CMyString &strKey, const CMyString &strValue)
{
	if (strKey.empty()) return;
    m_root[strKey] = strValue.c_str();
}

void CJsonWriter::Set(const CMyString &strKey, int32_t val)
{
	if (strKey.empty()) return;
    m_root[strKey] = val;
}

void CJsonWriter::Set(const CMyString &strKey, uint32_t val)
{
	if (strKey.empty()) return;
    m_root[strKey] = val;
}

void CJsonWriter::Set(const CMyString &strKey, int64_t val)
{
	if (strKey.empty()) return;
    m_root[strKey] = (Json::Value::Int64)val;
}

void CJsonWriter::Set(const CMyString &strKey, uint64_t val)
{
	if (strKey.empty()) return;
    m_root[strKey] = (Json::Value::UInt64)val;
}

void CJsonWriter::Set(const CMyString &strKey, double val)
{
	if (strKey.empty()) return;
    m_root[strKey] = val;
}

void CJsonWriter::Set(const CMyString &strKey, CJsonWriter &root)
{
	if (strKey.empty()) return;
    m_root[strKey] = root.getJson();
}

void CJsonWriter::Set(const CMyString &strKey, Json::Value &val)
{
	if (strKey.empty()) return;
    m_root[strKey] = val;
}

Json::Value& CJsonWriter::getJsonValue(const CMyString &strKey)
{
    return m_root[strKey];
}

CMyString CJsonWriter::c_str(void)
{
#ifdef JSONCPP_USE_OLD_WAY
    Json::FastWriter writer;
    CMyString result = writer.write(m_root);
#else
    Json::StreamWriterBuilder builder;
    CMyString result = Json::writeString(builder, m_root);
#endif
    result.trim_endl();
    return result;
}

CMyString CJsonWriter::asString(bool bStyled)
{
    CMyString result;
#ifdef JSONCPP_USE_OLD_WAY
    if (bStyled)
    {
        Json::StyledWriter writer;
        result = writer.write(m_root);
    }
    else
    {
        Json::FastWriter writer;
        result = writer.write(m_root);
    }
#else
    Json::StreamWriterBuilder builder;
    result = Json::writeString(builder, m_root);
#endif
    result.trim_endl();
    return result;
}


