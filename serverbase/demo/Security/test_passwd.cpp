/*
 * =====================================================================================
 *
 *       Filename:  test_passwd.cpp
 *
 *    Description:  encode / decode password
 *
 *        Version:  1.0
 *        Created:  2019年09月02日 19时13分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "Security.h"
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("syntax: %s {-e clear_text | -d cipher_text}\n", argv[0]);
        return -1;
    }
    int32_t opt=-1;
    CMyString strText;
    bool bEncrypt(false);
    CMyString strOrigin;
    bool bContinue(true);
    while((opt=getopt(argc, argv, "e:d:"))!=-1)
    {
        switch(opt)
        {
        case 'e':
            strText = optarg;
            bEncrypt = true;
            strOrigin.Format("qw%syq%s", strText.substr(0,3).c_str(),strText.substr(3).c_str());
            bContinue = false;
            break;
        case 'd':
            strText = optarg;
            bEncrypt = false;
            strOrigin = strText;
            bContinue = false;
            break;
        default:
            printf("syntax: %s {-e clear_text | -d cipher_text}\n", argv[0]);
            return -2;
            break;
        }
        if (!bContinue) break;
    }
    
    if (bEncrypt)
    {
        printf("encrypt [%s] ", strText.c_str());
    }
    else
    {
        printf("decrypt [%s] ", strText.c_str());
    }
    //printf("origin text [%s]\n", strOrigin.c_str());
    CMyString strOutput;
    bool newLine=false;
    if (bEncrypt)
    {
        strOutput = CBase64Wrapper::Encode(strOrigin.c_str(), strOrigin.length(), newLine);
        //printf("output [%s]\n", strOutput.c_str());
    }
    else
    {
        strOutput = CBase64Wrapper::Decode(strOrigin.c_str(), strOrigin.length(), newLine);
        //printf("output [%s]\n", strOutput.c_str());
        CMyString strTemp("%s%s", strOutput.substr(2,3).c_str(), strOutput.substr(7).c_str());
        strOutput = strTemp;
    }
    printf("output [%s] \n", strOutput.c_str());

    return 0;
}


