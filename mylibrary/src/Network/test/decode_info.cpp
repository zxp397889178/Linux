/*
 * =====================================================================================
 *
 *       Filename:  decode_info.cpp
 *
 *    Description:  解析文件名
 *
 *        Version:  1.0
 *        Created:  2020年02月15日 10时54分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyNet.h"
#include <iostream>

int32_t GetDispositionFileName(const CMyString &strDisposition, CMyString &strFile)
{
    size_t pos = strDisposition.find("filename");
    if (pos == std::string::npos)
    {
        return -1;
    }

    printf("pos:[%d]\n", pos);
    CMyString strTemp = strDisposition.substr(pos+8);
    printf("[temp: %s]\n", strTemp.c_str());
    pos += 8;
    size_t pos1 = strDisposition.find_first_of('\"', pos);
    if (pos1 == std::string::npos)
    {
        // 解析是否有 '=';
        pos1 = strDisposition.find_first_of('=', pos);
        if (pos1 == std::string::npos)
        {
            return -3;
        }
        pos1 += 1;
        size_t pos2 = strDisposition.find_first_of(';', pos1);
        if (pos2 == std::string::npos)
        {
            strFile = strDisposition.substr(pos1);
            return 1;
        }
        else
        {
            strFile = strDisposition.substr(pos1, pos2-pos1);
            return 2;
        }
    }

    pos1 += 1;
    size_t pos2 = strDisposition.find_first_of('\"', pos1);
    if (pos2 == std::string::npos)
    {
        return -5;
    }
    printf("pos[%d], pos2[%d]\n", pos1, pos2);
    strFile = strDisposition.substr(pos1, pos2-pos1);
    return 3;
}


// 解析 Content-Disposition: xxxx
int32_t decode_disposition(const char* pszDesposition, CMyString &strType, CMyString &strKey1, CMyString &strValue1, CMyString &strKey2, CMyString &strValue2)
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
    printf("data:[%s]\n", strData.c_str());
    CMyString strLine;
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
                strType.trim();
                break;
            case 2:
                strKey1 = str1;
                strValue1 = str2;
                strKey1.trim();
                strValue1.trim();
                break;
            case 3:
                strKey2 = str1;
                strValue2 = str2;
                strKey2.trim();
                strValue2.trim();
                break;
        }
    }while(!bLast && !bError && nLine < 4);
    return nLine;
#endif
}



int main(int argc, char **argv)
{
    //CMyString strDisp("Content-Disposition: attachment; filename=\"TcpClient.cpp\"");
    //CMyString strDisp("Content-Disposition: attachment; filename=TcpClient.cpp");
    //CMyString strDisp("Content-Disposition: form-data; name=\"\"; filename=\"女孩1 @副本.jpg\";");
    //CMyString strDisp("Content-Disposition: form-data; name=\"\"; filename=\"@#￥% &.jpg\";");
    std::string strDisp("Content-Disposition: form-data; name=\"\"; filename=\"女孩1 @副本.jpg\"; filename*=\"UTF-8\'\'\%E5\%A5\%B3\%E5\%AD\%A91\%20\%40\%E5\%89\%AF\%E6\%9C\%AC.jpg\"");
    // CMyString strDisp("Content-Disposition: attachment; filename=TcpClient.cpp; encoding=utf8");
    //CMyString strDisp("Content-Disposition: attachment; filename=\"TcpClient.cpp; encoding=utf8");
    // CMyString strDisp("Content-Disposition: attachment; filename+TcpClient.cpp; ");
    // 

    std::cout << "[" << strDisp << "], length:"  << strDisp.size() << std::endl;
    printf("[%s]\n", strDisp.c_str());

#if 0
    CMyString strFile;
    int32_t ret = GetDispositionFileName(strDisp, strFile);
#else
    CMyString strType;
    CMyString strKey1, strValue1;
    CMyString strKey2, strValue2;
    int32_t ret = decode_disposition(strDisp.c_str(), strType, strKey1, strValue1, strKey2, strValue2);
    printf("type= [%s]\n", strType.c_str());
    printf("[%s]=[%s]\n", strKey1.c_str(), strValue1.c_str());
    printf("[%s]=[%s]\n", strKey2.c_str(), strValue2.c_str());
    CMyString strFile = strValue2;
#endif

    printf("origin: %s\n", strDisp.c_str());
    printf("result: %d\n", ret);
    printf("file  : [%s]\n", strFile.c_str());

    return 0;
}
