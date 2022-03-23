/*
 * =====================================================================================
 *
 *       Filename:  HttpRespMaker.cpp
 *
 *    Description:  http respone maker
 *
 *        Version:  1.0
 *        Created:  11/05/2019 12:19:14 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "json/json.h"
#include "HttpRespMaker.h"
#include "Security.h"

namespace http {

CHttpRespMaker::CHttpRespMaker()
{

}

CHttpRespMaker::~CHttpRespMaker()
{
    clear();
}

CHttpRespMaker::KvItem* CHttpRespMaker::find(const CMyString &key)
{
    KV_MAP::iterator it(m_mapKV.find(key));
    if (it == m_mapKV.end())
    {
        return NULL;
    }
    return it->second;
}

void CHttpRespMaker::add_ex(const CMyString &key, const CMyString &val, char cType, bool bFormatted)
{
    CMyString key_(key);
    CMyString val_(val);
    key_.trim();
    val_.trim();

    if (cType=='F' && bFormatted)
    {
        // printf("val_[%s]\n", val_.c_str());
        // 去除浮点型尾部的空格
        bool bNeedTrim = (val_.find('.') != std::string::npos);
        if (bNeedTrim)
        {
            // 去除无用的 '0'
            std::string::reverse_iterator p = val_.rbegin();
            do {
                uint8_t c = *p;
                if (c != '0')
                {
                    break;
                }
                ++p;
            }while(p!=val_.rend());
            val_.erase(p.base(), val_.end());
        }

        // printf("val_[%s]\n", val_.c_str());
        // 去除最后的'.'
        size_t pos = val_.find_last_of('.');
        if (pos != std::string::npos)
        {
            if (pos+1 == val_.length())
            {
                val_.erase(pos);
            }
        }
    }
    KvItem *pItem = this->find(key_);
    if (NULL == pItem)
    {
        pItem = new KvItem(key_, val_, cType);

        m_mapKV[key] = pItem;
        m_vecKV.push_back(pItem);
    }
    else
    {
        // printf("replace [%s] => [%s]\n", pItem->val.c_str(), val_.c_str());
        pItem->val = val_;
        pItem->cType = cType;
    }
}

void CHttpRespMaker::add(const CMyString &key, const CMyString &val)
{
    this->add_ex(key, val, 'C');
}

void CHttpRespMaker::add(const CMyString &key, const char* val)
{
    //LogWarn("add resp maker[string]");
    this->add_ex(key, NULL==val ? "" : val, 'C');
}

void CHttpRespMaker::add(const CMyString &key, int32_t val)
{
    this->add_ex(key, CMyString("%d", val), 'N');
}

void CHttpRespMaker::add(const CMyString &key, uint32_t val)
{
    this->add_ex(key, CMyString("%u", val), 'N');
}

void CHttpRespMaker::add(const CMyString &key, int64_t val)
{
    this->add_ex(key, CMyString("%ld", val), 'N');
}

void CHttpRespMaker::add(const CMyString &key, uint64_t val)
{
    this->add_ex(key, CMyString("%lu", val), 'N');
}

void CHttpRespMaker::add(const CMyString &key, double val)
{
    this->add_ex(key, CMyString("%.8lf", val), 'F');
}

void CHttpRespMaker::add(const CMyString &key, bool val)
{
    //LogFatal("add resp maker [boolean]");
    this->add_ex(key, CMyString("%s", (val ? "true" : "false")), 'B');
}

void CHttpRespMaker::erase(const CMyString &key)
{
    m_mapKV.erase(key);
    for(KV_VECTOR::iterator it(m_vecKV.begin()); it != m_vecKV.end(); )
    {
        KvItem *pItem = *it;
        if (pItem->key == key)
        {
            SAFE_DELETE(pItem);
            it = m_vecKV.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CHttpRespMaker::clear(void)
{
    m_mapKV.clear();
    for(KV_VECTOR::iterator it(m_vecKV.begin()); it != m_vecKV.end(); ++it)
    {
        KvItem *pItem = *it;
        SAFE_DELETE(pItem);
    }
    m_vecKV.clear();
}

inline CMyString urlEncodeData(CMyString &val, bool bEncode)
{
    if (bEncode)
        return CHttpWrapper::encodeURIComponent(val);
    else
        return val;
}
CMyString CHttpRespMaker::http_str_manual(char cGroupSpliter, char cKvSpliter, bool ignoreEmpty, bool bSort, bool bUrlEncode)
{
    int32_t cnt(0);
    CMyString result;
    if (bSort)
    {
        KV_MAP::iterator it(m_mapKV.begin()); 
        for(; it != m_mapKV.end(); ++it)
        {
            KvItem *pItem = it->second;
            if (pItem->val.empty() && ignoreEmpty)
            {
                continue;
            }
            if (cnt > 0)
            {
                if (cGroupSpliter!='\0')
                {
                    result += cGroupSpliter;
                }
            }
            result += pItem->key;
            if (cKvSpliter!='\0')
            {
                result += cKvSpliter;
            }
            result += urlEncodeData(pItem->val, bUrlEncode);
            ++cnt;
        }
    }
    else
    {
        KV_VECTOR::iterator it(m_vecKV.begin());
        for(; it != m_vecKV.end(); ++it)
        {
            KvItem *pItem = *it;
            if (pItem->val.empty() && ignoreEmpty)
            {
                continue;
            }
            if (cnt > 0)
            {
                if (cGroupSpliter != '\0')
                {
                    result += cGroupSpliter;
                }
            }
            result += pItem->key;
            if (cKvSpliter != '\0')
            {
                result += cKvSpliter;
            }
            result += urlEncodeData(pItem->val, bUrlEncode);
            ++cnt;
        }
    }
    return result;
}

CMyString CHttpRespMaker::http_str_values(char cGroupSpliter, bool bSort, bool bUrlEncode)
{
    int32_t cnt(0);
    CMyString result;
    if (bSort)
    {
        KV_MAP::iterator it(m_mapKV.begin()); 
        for(; it != m_mapKV.end(); ++it)
        {
            KvItem *pItem = it->second;
            if (pItem->val.empty())
            {
                continue;
            }
            if (cnt > 0)
            {
                if (cGroupSpliter!='\0')
                {
                    result += cGroupSpliter;
                }
            }
            result += urlEncodeData(pItem->val, bUrlEncode);
            ++cnt;
        }
    }
    else
    {
        KV_VECTOR::iterator it(m_vecKV.begin());
        for(; it != m_vecKV.end(); ++it)
        {
            KvItem *pItem = *it;
            if (pItem->val.empty())
            {
                continue;
            }
            if (cnt > 0)
            {
                if (cGroupSpliter != '\0')
                {
                    result += cGroupSpliter;
                }
            }
            result += urlEncodeData(pItem->val, bUrlEncode);
            ++cnt;
        }
    }
    return result;
}

CMyString CHttpRespMaker::http_str(bool bSort)
{
    return this->http_str_manual('&', '=', false, bSort, false);
}

CMyString CHttpRespMaker::json_str_styled(bool bStyled)
{
    Json::Value resp;
    KV_MAP::iterator it(m_mapKV.begin());
    for(; it != m_mapKV.end(); ++it)
    {
        KvItem *pItem = it->second;
        switch(pItem->cType)
        {
        case 'I':
            resp[pItem->key] = (Json::Value::Int64)StrToInt64(pItem->val.c_str());
            break;
        case 'N':
            resp[pItem->key] = (Json::Value::UInt64)StrToUint64(pItem->val.c_str());
            break;
        case 'F':
            resp[pItem->key] = StrToDouble(pItem->val.c_str());
            break;
        case 'B':
            resp[pItem->key] = StrToBoolean(pItem->val.c_str());
            break;
        default:
            resp[pItem->key] = pItem->val;
            break;
        }
    }

    CJsonWrapper jr(resp);
    CMyString result;
    if (bStyled)
    {
        result = jr.toStyledString();
    }
    else
    {
        result = jr.toString();
    }
    return result;
}

CMyString CHttpRespMaker::json_str(bool bSort)
{
    CMyString result("{");
    if (bSort)
    {
        KV_MAP::iterator it(m_mapKV.begin());
        for(; it != m_mapKV.end(); ++it)
        {
            if (it != m_mapKV.begin())
            {
                result += ',';
            }
            KvItem *pItem = it->second;
            switch(pItem->cType)
            {
            case 'I':
            case 'N':
            case 'F':
            case 'B':
                result.Append("\"%s\":%s", pItem->key.c_str(), pItem->val.c_str());
                break;
            default:
                result.Append("\"%s\":\"%s\"", pItem->key.c_str(), pItem->val.c_str());
                break;
            }
        }
        result += '}';
    }
    else
    {
        KV_VECTOR::iterator it(m_vecKV.begin());
        for(; it != m_vecKV.end(); ++it)
        {
            if (it != m_vecKV.begin())
            {
                result += ',';
            }

            KvItem *pItem = *it;
            switch(pItem->cType)
            {
            case 'I':
            case 'N':
            case 'F':
            case 'B':
                result.Append("\"%s\":%s", pItem->key.c_str(), pItem->val.c_str());
                break;
            default:
                result.Append("\"%s\":\"%s\"", pItem->key.c_str(), pItem->val.c_str());
                break;
            }
        }
        result += '}';
    }
    return result;
}

CMyString CHttpRespMaker::sign_by_md5(const CMyString &signName, const CMyString &extraData, bool bSort, bool bLowercase)
{
    CMyString str1 = this->http_str(bSort);
    CMyString str2 = str1;
    if (!extraData.empty())
    {
        str1 += extraData;
    }
    CMyString strMd5 = CMD5Wrapper::encode(str1.c_str());
    if (bLowercase)
    {
        strMd5.MakeLower();
    }
    else
    {
        strMd5.MakeUpper();
    }
    CMyString result("%s&%s=%s", str2.c_str(), signName.c_str(), strMd5.c_str());
    LogDebug("origin text[%s]", str2.c_str());
    LogDebug("with key text[%s]", str1.c_str());
    LogDebug("md5(key text)[%s]", strMd5.c_str());
    LogDebug("sign text [%s]", result.c_str());
    return result;
}

CMyString CHttpRespMaker::signWithMd5(const CMyString &signName, bool bLowerCase)
{
    return this->sign_by_md5(signName, "", true, bLowerCase);
}

CMyString CHttpRespMaker::signWithMd5NoSort(const CMyString &signName, bool bLowerCase, const char *pszExtraKey)
{
    return this->sign_by_md5(signName, (pszExtraKey ? CMyString(pszExtraKey) : ""), false, bLowerCase);
}

bool CHttpRespMaker::AddJsonStr(const CMyString &strJson)
{
    CJsonWrapper jr;
    if (!jr.Load(strJson))
    {
        LogErr("[%s:%s:%d]can not parse string %s to json", __FILE__, __func__, __LINE__, strJson.c_str());
        return false;
    }

    this->AddJson(jr.getJson());
    return true;
}

void CHttpRespMaker::AddUrlParam(const CMyString &strParam)
{
    std::vector<KvstrItem> temp;
    int32_t ret = safe_strtok(strParam.c_str(), temp, "&", "="); 
    if (ret<=0)
    {
        LogFatal("decode urlparam [%s] failure", strParam.c_str());
        return;
    }
    for(std::vector<KvstrItem>::iterator it(temp.begin());
            it != temp.end(); ++it)
    {
        KvstrItem &item = *it;
        this->add(item.key, item.value);
    }
}

void CHttpRespMaker::AddJson(Json::Value &root)
{
    Json::Value::Members mem = root.getMemberNames();
    for(Json::Value::Members::iterator it=mem.begin(); it != mem.end(); ++it)
    {
        CMyString strKey = *it;
        switch(root[strKey].type())
        {
        case Json::objectValue:
            {
                Json::Value val;
                this->AddJson(val);
            }
            break;
        case Json::arrayValue:
            // [v1,v2,v3,v4,...]
            {
                CMyString result("[");
                Json::Value val = root[strKey];
                for(int32_t i(0); i<(int32_t)val.size(); ++i)
                {
                    if (i!=0)
                    {
                        result += ',';
                    }
                    if (val[i].isNull())
                    {
                        result.append("(null)");
                    }
                    else if (val[i].isObject())
                    {
                        result.append("(object)");
                    }
                    else if (val[i].isArray())
                    {
                        result.append("(array)");
                    }
                    else
                    {
                        result.append(val[i].asString());
                    }
                }
                result += ']';
                this->add(strKey, result);
            }
            break;
        case Json::intValue:
            this->add(strKey, (int64_t)root[strKey].asInt64());
            break;
        case Json::uintValue:
            this->add(strKey, (uint64_t)root[strKey].asUInt64());
            break;
        case Json::realValue:
            this->add(strKey, root[strKey].asDouble());
            break;
        case Json::stringValue:
            this->add(strKey, root[strKey].asString());
            break;
        case Json::booleanValue:
            this->add(strKey, root[strKey].asBool());
            break;
        default:
            this->add(strKey, root[strKey].asString());
            break;
        }
    }
}

bool CHttpRespMaker::setFieldType(const CMyString &key, char cType)
{
    KV_MAP::iterator it(m_mapKV.find(key));
    if (it == m_mapKV.end())
    {
        return false;
    }
    KvItem *pItem = it->second;
    bool result(true);
    switch(cType)
    {
    case 'C':
        pItem->cType = cType;
        break;
    case 'I':
    case 'N':
    case 'F':
        // 需要校验值为否为 '+ - 0-9 .'
        pItem->cType = cType;
        break;
    case 'B':
        // 需要校验值是否合法
        pItem->cType = cType;
        break;
    default:
        result = false;
        break;
    }
    return result;
}


}


