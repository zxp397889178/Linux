/*
 * =====================================================================================
 *
 *       Filename:  GeoIPWrapper.cpp
 *
 *    Description:  对GeoIP的封装
 *
 *        Version:  1.0
 *        Created:  2021年07月17日 22时58分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "GeoIPWrapper.h"
#include <arpa/inet.h>
#include "mmdb/mmdb.h"

int32_t MMDBPrint(const mmdb_type_t* const pLookup, CMyString &strData);
void SplitGeoipData(Json::Value &data, const CMyString &regionCode);

CGeoIPWrapper::CGeoIPWrapper()
    : m_pHandler(NULL)
{

}

CGeoIPWrapper::~CGeoIPWrapper()
{
    if (NULL != m_pHandler)
    {
        mmdb_close(m_pHandler);
        m_pHandler = NULL;
    }
}

bool CGeoIPWrapper::Open(const CMyString &mmdb_file)
{
    if (NULL != m_pHandler)
    {
        mmdb_close(m_pHandler);
        m_pHandler = NULL;
    }

    m_pHandler = mmdb_open(mmdb_file.c_str());
    if (NULL == m_pHandler)
    {
        LogErr("can not open GeoIP format file [%s]", mmdb_file.c_str());
        return false;
    }
    LogMsg("open GeoIP format file [%s] succeed.", mmdb_file.c_str());

    return true;
}

bool CGeoIPWrapper::Lookup(const CMyString &ip, Json::Value &resp, const CMyString &region_code)
{
    if (NULL == m_pHandler)
    {
        LogWarn("not open GeoIP format file.");
        return false;
    }

    struct in6_addr bip6;
    struct in_addr   bip4;

    mmdb_type_t *pLookup(NULL);
    if (inet_pton(AF_INET, ip.c_str(), &bip4))
    {
        pLookup = mmdb_lookup4(m_pHandler, (uint8_t *)&bip4);
    }
    else if (inet_pton(AF_INET, ip.c_str(), &bip6))
    {
        pLookup = mmdb_lookup6(m_pHandler, (uint8_t*)&bip6);
    }

    if (pLookup == NULL)
    {
        LogWarn("invalid IP address [%s]", ip.c_str());
        return false;
    }

    CMyString strData;
    MMDBPrint(pLookup, strData);
    mmdb_type_free(pLookup);

    // 转化成Json
    CJsonWrapper jr;
    if (!jr.Load(strData))
    {
        LogErr("invalid json string [%s]", strData.c_str());
        return false;
    }
    resp = jr.getJson();

    SplitGeoipData(resp, region_code);
    resp["ip"] = ip;
    resp["region"] = region_code;

    return true;
}

void LeaveOnlyRegionData(Json::Value &item, const CMyString &regionCode)
{
    Json::Value namesObj = item["names"];
    if (namesObj.isMember(regionCode))
    {
        Json::Value regionObj = namesObj[regionCode];
        item.removeMember("names");
        item["names"] = regionObj;
    }
    else
    {
        Json::Value regionObj = namesObj["en"];
        item.removeMember("names");
        item["names"] = regionObj;
    }
}

void SplitGeoipData(Json::Value &data, const CMyString &regionCode)
{
    Json::Value::iterator it(data.begin());
    for(; it != data.end(); ++it)
    {
        std::string n1 = it.name();
        if (n1.empty())
        {
            continue;
        }
        Json::Value &val = data[n1];
        if (val.isObject())
        {
            if (!val.isMember("names"))
            {
                continue;
            }
            LeaveOnlyRegionData(val, regionCode);
        }
        else if (val.isArray())
        {
            for(Json::ArrayIndex i(0); i<val.size(); ++i)
            {
                Json::Value &arr = val[i];
                if (!arr.isObject())
                {
                    continue;
                }
                if (!arr.isMember("names"))
                {
                    continue;
                }
                LeaveOnlyRegionData(arr, regionCode);
            }
        }
    }
}

int32_t MMDBPrint(const mmdb_type_t* const pLookup, CMyString &strData)
{
    if (pLookup == NULL)
    {
        return -1;
    }

    switch(pLookup->type)
    {
    case MMDB_EXTENDED:
        strData += "INVALID_EXTENDED";
        break;
    case MMDB_DATA_CACHE:
        strData += "DATA_CACHE";
        break;
    case MMDB_END_MARKER:
        strData += "END_MARKER";
        break;
    case MMDB_POINTER:
        strData.Append("*%u", pLookup->data.u_ptr);
        break;
    case MMDB_STRING:
        strData +='\"';
        for(mmdb_length_t i(0); i < pLookup->data.u_string.length; ++i)
        {
            switch(pLookup->data.u_string.data[i])
            {
            case '\"':
                strData += "\\\"";
                break;
            case '\'':
                strData += "\\\'";
                break;
            case '\\':
                strData += "\\\\";
                break;
            case '\a':
                strData += "\\a";
                break;
            case '\b':
                strData += "\\b";
                break;
            case '\n':
                strData += "\\n";
                break;
            case '\t':
                strData += "\\t";
                break;
                // and so on
            default:
                if (iscntrl(pLookup->data.u_string.data[i]))
                    strData.Append("\\%03o", pLookup->data.u_string.data[i]);
                else
                    strData += pLookup->data.u_string.data[i];
                break;
            }
        }
        strData += '\"';
        break;
    case MMDB_DOUBLE:
        strData.Append("%f", pLookup->data.u_double);
        break;
    case MMDB_FLOAT:
        strData.Append("%f", pLookup->data.u_float);
        break;
    case MMDB_BYTES:
        strData += 'b';
        strData += '\"';
        for(mmdb_length_t i(0); i < pLookup->data.u_bytes.length; ++i)
        {
            strData.Append("\\%03o", pLookup->data.u_bytes.data[i]);
        }
        strData += '\"';
        break;
    case MMDB_UINT16:
        strData.Append("%u", pLookup->data.u_uint16);
        break;
    case MMDB_UINT32:
        strData.Append("%u", pLookup->data.u_uint32);
        break;
    case MMDB_INT32:
        strData.Append("%d", pLookup->data.u_int32);
        break;
    case MMDB_UINT64:
        strData += "0x";
        for(int32_t i(8); i>0; --i)
        {
            strData.Append("%02x", pLookup->data.u_uint64.data[i]);
        }
        break;
    case MMDB_UINT128:
        strData += "0x";
        for(int32_t i(16); i>0; --i)
        {
            strData.Append("%02x", pLookup->data.u_uint128.data[i]);
        }
        break;
    case MMDB_BOOL:
        strData+= (pLookup->data.u_bool ? "true" : "false");
        break;
    case MMDB_ARRAY:
        strData += "[  ";
        for(mmdb_length_t i(0); i < pLookup->data.u_array.length; ++i)
        {
            MMDBPrint(pLookup->data.u_array.entries+i, strData);
            if (i != pLookup->data.u_array.length-1)
            {
                strData += ", ";
            }
        }
        strData += " ]";
        break;
    case MMDB_MAP:
        strData += "{  ";
        for (mmdb_length_t i(0); i < pLookup->data.u_map.length; ++i)
        {
            mmdb_type_t key;
            key.type = MMDB_STRING;
            key.data.u_string = pLookup->data.u_map.keys[i];
            MMDBPrint(&key, strData);
            strData += ": ";
            MMDBPrint(pLookup->data.u_map.values+i, strData);
            if (i != pLookup->data.u_map.length-1)
            {
                strData += ", ";
            }
        }
        strData += " }";
        break;
    }
    return 0;
}


