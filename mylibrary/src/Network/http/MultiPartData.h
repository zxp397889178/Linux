/*
 * =====================================================================================
 *
 *       Filename:  MultiPartData.h
 *
 *    Description:  对 multipart/form-data 的数据格式进行解析
 *
 *        Version:  1.0
 *        Created:  2020年02月20日 11时51分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"

namespace http {

class CHttpRequest;
class CMultiPartData
{
    struct NodeItem
    {
        CMyString   fileName;
        int32_t     fileSize;
        CByteStream buffer;
        NodeItem()
            : fileName(), fileSize(0), buffer()
        {
        }
    };
    typedef std::vector<NodeItem>   VEC_FILENODE;
public:
    CMultiPartData();
    ~CMultiPartData();

    int32_t Decode(http::CHttpRequest &req, CByteStream &buff, int32_t nContentLength, const CMyString &strBoundaryData);

    bool IsFinished(void) { return m_bFinished; }

protected:
    void Post(http::CHttpRequest &req);

private:
    // 返回值： 0 解析出错
    //          1 仅有strContentType
    //          2 含有后续的key value
    int32_t decode_content(const CMyString &strContent, CMyString &strContentType, CMyString &strKey, CMyString &strValue);

    // 解析 Content-Disposition: xxxx
    // !!!注意!!! 此处必须用std::string, 而不能用CMyString 
    int32_t decode_disposition(const char* pszDesposition, std::string &strType, 
            std::string &strKey1, std::string &strValue1, 
            std::string &strKey2, std::string &strValue2);


    void PushNodeData(VEC_FILENODE &vecNode, const CMyString &strFile, byte_t *pData,  int32_t size);
    int32_t GetNodeData(VEC_FILENODE &vecNode, CMyString &strText, CByteStream &buff);

private:
    byte_t*         m_pBuffer;
    int32_t         m_cbBuffer;

    VEC_FILENODE    m_vecFileNode;
    bool            m_bFinished;
};

}


