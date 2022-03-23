/*
 * =====================================================================================
 *
 *       Filename:  MultiPartData.cpp
 *
 *    Description:  对 multipart/form-data 格式的数据进行解析
 *
 *        Version:  1.0
 *        Created:  2020年02月20日 11时53分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "MultiPartData.h"
#include "HttpRequest.h"

namespace http {

extern char* trim_endl(char *pc, int32_t &len);
extern void dumpbin(char *pc, int32_t len);
extern void dumptxt(char *pc, int32_t len);
extern char* find_boundary(char *pc, int32_t nLen, const CMyString &strBoundary, int32_t &pos);


CMultiPartData::CMultiPartData()
    : m_pBuffer(NULL)
    , m_bFinished(false)
{

}

CMultiPartData::~CMultiPartData()
{
    if (NULL != m_pBuffer)
    {
        free(m_pBuffer);
    }

}

void CMultiPartData::PushNodeData(VEC_FILENODE &vecNode, const CMyString &strFile, byte_t *pData,  int32_t size)
{
    bool bFind(false);
    VEC_FILENODE::iterator it(vecNode.begin());
    for(; it != vecNode.end(); ++it)
    {
        NodeItem &node = *it;
        if (node.fileName == strFile)
        {
            node.fileSize += size;
            node.buffer.Write(pData, size);
            bFind =true;
            break;
        }
    }
    if (bFind)
    {
        return;
    }
    
    NodeItem node;
    node.fileName = strFile;
    node.fileSize = size;
    node.buffer.Write(pData, size);
    vecNode.push_back(node);
}

int32_t CMultiPartData::GetNodeData(VEC_FILENODE &vecNode, CMyString &strText, CByteStream &buff)
{
    int32_t size(0);
    VEC_FILENODE::iterator it(vecNode.begin());
    for(; it != vecNode.end(); ++it)
    {
        NodeItem &node = *it;
        strText.Append("%s/%d;", node.fileName.c_str(), node.fileSize);
        buff.Write(node.buffer);
        size += node.fileSize;
    }
    return size;
}

int32_t CMultiPartData::Decode(http::CHttpRequest &req, CByteStream &buff, int32_t nContentLength, const CMyString &strBoundaryData)
{
    int32_t bufSize = (int32_t)buff.size();
    if (bufSize <= 0)
    {
        LogDebug("MP>buffer data match, content [%d], buffer[%d], continue read.", nContentLength, bufSize);
        return 0;
    }

    if (nContentLength > bufSize)
    {
        LogDebug("MP>buffer data match, content [%d], buffer[%d], continue read.", nContentLength, bufSize);
        return 0;
    }

    if (NULL != m_pBuffer)
    {
        free(m_pBuffer);
    }
    m_pBuffer = (byte_t*)calloc(1, bufSize+1);
    m_cbBuffer = bufSize;

    buff.SetPeekMode(true);
    buff.Read(m_pBuffer, bufSize);
    buff.SetPeekMode(false);
    LogImpt("MP>content-length %d, buffer size [%lu], buff size left [%d]", 
            nContentLength, bufSize, (int32_t)buff.size());

    // TODO: 存入到临时文件，以便校对
/* 
    FILE *fp = fopen("./temp1.dat", "wb");
    if (NULL != fp)
    {
        fwrite(m_pBuffer, 1, m_cbBuffer, fp);
        fflush(fp);
        fclose(fp);
    }
*/

    // https://blog.csdn.net/zhangge3663/article/details/81218488
    // NOTE: 分隔符有前导 --
    CMyString strBoundary("--%s", strBoundaryData.c_str());

    // 逐行解析数据
    int32_t nLine(0);
    int32_t nBoundary = (int32_t)strBoundary.length();
    int32_t nLeft(bufSize);
    char szLine[256] = "";
    char *pc = (char*)m_pBuffer;
    do {
        char *pc1 = strstr(pc, strBoundary.c_str());
        if (pc1 == NULL)
        {
            LogWarn("[%s:%s:%d]not found boundary.", __FILE__, __func__, __LINE__);
            break;
        }
        nLeft -= (pc1-pc);  //定位到分隔符

        pc1 += nBoundary;
        nLeft -= nBoundary;
        if (strncmp(pc1, "--", 2) == 0)
        {
            // 最后一行，已结束
            LogTrace("MP>is the last line.");
            nLeft -= 2;
            m_bFinished = true;
            break;
        }
        pc1 += 2; // \r\n
        nLeft -= 2;

        bool bHasData(false);
        // 解析 Content-Disposition
        std::string strType;
        std::string strKey1, strValue1;
        std::string strKey2, strValue2;
        if (strncmp(pc1, "Content-Disposition", 19) == 0)
        {
            //LogTrace("MP>decode Content-Disposition");
            char *pc2 = strstr(pc1, "\r\n");
            if (NULL != pc2)
            {
                nLine = pc2 - pc1;
            }
            else
            {
                nLine = strlen(pc1);
            }
            
            safe_strcpy(szLine, std::min(nLine+1, (int32_t)sizeof(szLine)), pc1);
            int32_t ret = decode_disposition(szLine, strType, strKey1, strValue1, strKey2, strValue2);
            LogTrace("MP>line:[%s], size=%d, ret=%d", szLine, nLine, ret);
            pc1 += (nLine+2);
            nLeft -= (nLine+2);
            bHasData = true;
        }

        // 解析 Content-Type
        CMyString strContentType;
        CMyString strKey3, strValue3;
        if (strncmp(pc1, "Content-Type", 12)== 0)
        {
            char *pc2 = strstr(pc1, "\r\n");
            if (NULL != pc2)
            {
                nLine = pc2 - pc1;
            }
            else
            {
                nLine = strlen(pc1);
            }
            safe_strcpy(szLine, std::min(nLine+1, (int32_t)sizeof(szLine)), pc1);
            int32_t ret = decode_content(szLine, strContentType, strKey3, strValue3);
            LogTrace("MP>line:[%s], size=%d, ret=%d", szLine, nLine, ret);
            pc1 += (nLine+2);
            nLeft -= (nLine+2);
        }

        // 去掉空行
        int32_t trimLen(0);
        char *pc2 = trim_endl(pc1, trimLen);
        nLeft -= trimLen;

        // 判断是否为一个新的part
        if (strncmp(pc2, strBoundary.c_str(), nBoundary) == 0)
        {
            LogTrace("MP>new boundary part.");
            pc = pc2;
            continue;
        }

        if (!bHasData)
        {
            LogTrace("MP>not data, loop");
            continue;
        }

        // 处理文件或其他数据
        char *pc3 = strstr(pc2, strBoundary.c_str());
        if (pc3 == NULL)
        {
            int32_t pos_data(0);
            pc3 = find_boundary(pc2, nLeft, strBoundary.c_str(), pos_data);
            if (pc3 != NULL)
            {
                LogTrace("MP>use binary search, pc2[%p], pc3[%p], pos [%d]",
                    pc2, pc3, pos_data);
                nLine = pc3 - pc2 - 2;
            }
            else
            {
                LogTrace("MP>use binary search, pc2[%p], pc3[NULL], pos [%d]",
                    pc2, pos_data);

                // TODO: 有可能会含到 \r\n , 但无法判断是否数据中的，暂不处理
                nLine = nLeft;  
            }
        }
        else
        {
            nLine = pc3 - pc2 - 2; // 不含 \r\n
        }

        // 数据在些，计划存入指定ByteStream 中
        // nLine = pc3 - pc2 - 2;  // 不含\r\n
        if (strKey2 == "filename" && !strValue2.empty())
        {
            LogTrace("MP>file [%s] data, size %d", strValue2.c_str(), nLine);
            PushNodeData(m_vecFileNode, strValue2, (byte_t*)pc2, nLine);
        }
        else
        {
            safe_strcpy(szLine, std::min(nLine+1, (int32_t)sizeof(szLine)), pc2);
            LogTrace("MP>%s=[%s]", strValue1.c_str(), szLine);
            if (strValue3 == "application/json" || strValue3 == "txt/json")
            {
                req.parseParamByJson(szLine);
            }
            else if (strValue3 == "text/xml")
            {
                req.parseParamByXml(szLine);
            }
            else
            {
                CMyString strTxt("%s=%s", strValue1.c_str(), szLine);
                req.parseParam(strTxt);
            }
        }
        pc = pc3;
        nLeft -= (nLine+2);
        LogTrace("MP>left size %d", nLeft);
    }while(nLeft > 0 && !m_bFinished);

    this->Post(req);

    return 0;
}

void CMultiPartData::Post(http::CHttpRequest &req)
{
    if (!m_bFinished)
    {
        return;
    }

    CMyString strNodeInd;
    CByteStream &buff = req.GetPost();
    buff.clear();
    LogWarn("[%s:%s:%d]clear CMultiPartData post data.", __FILE__, __func__, __LINE__);
    int32_t nNewContentLength = GetNodeData(m_vecFileNode, strNodeInd, buff);
    if (nNewContentLength > 0)
    {
        // NOTE: filename/size;filename/size;...
        req.setParam("MultiFileIndicator", strNodeInd);
        req.setParam("Contnet-Length", nNewContentLength);
    }

    // 做校验之用
    //LogTrace("nodeind: %s", strNodeInd.c_str());
    CMyString strV1 = req.getParam("MultiFileIndicator");
    LogWarn("MP>MultiFileIndicator=[%s], size=%d", strV1.c_str(), nNewContentLength);
}

char* find_boundary(char *pc, int32_t nLen, const CMyString &strBoundary, int32_t &pos)
{
    pos = -1;
    // char *pData = pc;
    for(int32_t i(0); i<nLen-1; ++i)
    {
        uint8_t c1 = pc[i];
        if (c1 == '-')
        {
            uint8_t c2=pc[i+1];
            if (c2 == '-')
            {
                // 有可能是boundary
                if (strncmp(&pc[i], strBoundary.c_str(), strBoundary.length())== 0)
                {
                    pos = i;
                    break;
                }
            } // if(c2 ...
        } // if (c1 ...
    } // for(int32_t ...

    if (pos == -1)
    {
        return  NULL;
    }
    return &pc[pos];
}

char* trim_endl(char *pc, int32_t &len)
{
    len = 0;
    int32_t nLen = (int32_t)strlen(pc);
    for(int32_t i(0); i<nLen-1; )
    {
        uint8_t c1 = pc[i];
        if (c1==0x0D)
        {
            uint8_t c2 = pc[i+1];
            if (c2 == 0x0A)
            {
                len += 2;
                i += 2;
                continue;
            }
            break;
        }
        break;
    }
    char *pc2 = pc+len;
    return pc2;
}

void dumpbin(char *pc, int32_t len)
{
    CMyString str;
    for(int32_t i=0; i<len; ++i)
    {
        uint8_t c = pc[i];
        str.Append("%02x ", c);
    }
    LogDebug("MP>[%p] %s", pc, str.c_str());
}

void dumptxt(char *pc, int32_t len)
{
    CMyString str;
    for(int32_t i(0); i<len; ++i)
    {
        uint8_t c = pc[i];
        str.Append("%c", c);
    }
    LogDebug("MP>[%p] [%s]", pc, str.c_str());
}

// 返回值： 0 解析出错
//          1 仅有strContentType
//          2 含有后续的key value
int32_t CMultiPartData::decode_content(const CMyString &strContent, CMyString &strContentType, CMyString &strKey, CMyString &strValue)
{
    char str1[128] = "";
    char str2[128] = "";
    char str3[128] = "";
    char str4[128] = "";
    int32_t ret = sscanf(strContent.c_str(),  "%[^: ]:%[^;];%[^=]=%[^; ]", str1, str2, str3, str4);
    if (ret<2)
    {
        return -1;
    }
    if (ret>=2 || ret<4)
    {
        strContentType = str2;
        strContentType.trim();
        return 1;
    }
    strContentType = str2;
    strKey = str3;
    strValue = str4;
    strContentType.trim();
    strKey.trim();
    strValue.trim();
    return 2;
}

// 解析 Content-Disposition: xxxx
// !!!NOTE!!! 此处必须要用std::string
int32_t CMultiPartData::decode_disposition(const char* pszDesposition, std::string &strType, 
        std::string &strKey1, std::string &strValue1, std::string &strKey2, std::string &strValue2)
{
#if 0
    char str1[128] = "";
    char str2[128] = "";
    char str3[128] = "";
    char str4[128] = "";
    char str5[128] = "";
    char str6[128] = "";
    int32_t ret = sscanf(pszDesposition, "%[^: ]:%[^;];%[^=]=\"%[^\"]\";%[^=]=\"%[^\"]", str1, str2, str3, str4, str5, str6); 
    if (ret <4)
    {
        return -1;
    }
    else if (ret>=4 && ret<=5)
    {
        strType = str2;
        strKey1 = str3;
        strValue1 = str4;
        strType.trim();
        strKey1.trim();
        strValue1.trim();
        return 1;
    }
    strType = str2;
    strKey1 = str3;
    strValue1 = str4;
    strKey2 = str5;
    strValue2 = str6;
    strType.trim();
    strKey1.trim();
    strValue1.trim();
    strKey2.trim();
    strValue2.trim();
    return 2;
#else
    std::string strData(pszDesposition);
    std::string strLine;
    bool bLast(false);
    bool bError(false);
    char str1[128] = "";
    char str2[128] = "";
    //char str3[128] = "";
    int32_t nLine(0);
    do {
        size_t pos = strData.find_first_of(';');
        if (pos == std::string::npos)
        {
            // last line
            strLine = strData;
            bLast = true;
        }
        else
        {
            strLine = strData.substr(0, pos);
            strData = strData.substr(pos+1);
        }
        ++nLine;
        //printf("line#%d [%s]\n", ++nLine, strLine.c_str());

        int32_t ret = sscanf(strLine.c_str(), "%[^:]:%s", str1, str2);
        if (ret != 2)
        {
            ret = sscanf(strLine.c_str(), "%[^=]=\"%[^;]", str1, str2);
            if (ret != 2)
            {
                // printf("decode error.\n");
                bError = true;
                break;
            }
            char *pc2 = strchr(str2, '\"');
            if (pc2!=NULL)
            {
                *pc2 = '\0';
            }
        }
        // printf("str1[%s], str2[%s]\n", str1, str2);
        switch(nLine)
        {
            case 1:
                strType = str2;
                trim(strType);
                break;
            case 2:
                strKey1 = str1;
                strValue1 = str2;
                trim(strKey1);
                trim(strValue1);
                break;
            case 3:
                strKey2 = str1;
                strValue2 = str2;
                trim(strKey2);
                trim(strValue2);
                break;
        }
    }while(!bLast && !bError && nLine < 4);
    return nLine;
#endif
}


}


