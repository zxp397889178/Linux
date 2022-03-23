/*
 * =====================================================================================
 *
 *       Filename:  urlencode.cpp
 *
 *    Description:  test url_encode / url_decode
 *
 *        Version:  1.0
 *        Created:  2020年02月25日 04时06分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "BaseCode.h"
#include "Security.h"

void Syntax(const char *app)
{
    printf("syntax:\n");
    printf("    %s %sencode <string>%s\n", app, _color_lightred, _color_end);
    printf("    %s %sencode <mode> <string>%s\n", app, _color_lightred, _color_end);
    printf("    %s %sdecode <string>%s\n", app, _color_lightred, _color_end);
    printf("param:\n");
    printf("    %stype%s => encode | decode \n", _color_green, _color_end);
    printf("    %smode%s => 1 | 2 | 3 | 4\n", _color_green, _color_end);
    printf("            1: normal, keep char [a-z, A-Z, 0-9, /._ ]\n");
    printf("            2: escape, keep char [a-z, A-Z, 0-9, @*/+,]\n");
    printf("            3: encodeURI, keep char [a-z, A-Z, 0-9, ~!@#$&*()=:/,;?+' ]\n");
    printf("            4: encodeURIComponent, keep char [a-z, A-Z, 0-9, ~!*()' ]\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        Syntax(argv[0]);
        return -1;
    }

    if (argc == 3)
    {
        CMyString strType(argv[1]);
        // app <type> <string>
        if (strType == "encode")
        {
            std::string strInput(argv[2]);
            CMyString str1 = CHttpWrapper::escape(strInput);
            CMyString str2 = CHttpWrapper::encodeURI(strInput);
            CMyString str3 = CHttpWrapper::encodeURIComponent(strInput);
            CMyString strTemp(strInput);
            CMyString str4 = CHttpWrapper::urlEncode(strTemp);

            printf("origin:             %s\n", argv[2]);
            printf("escape:             %s%s%s\n", _color_lightred, str1.c_str(), _color_end);
            printf("encodeURI:          %s%s%s\n", _color_lightred, str2.c_str(), _color_end);
            printf("encodeURIComponent: %s%s%s\n", _color_lightred, str3.c_str(), _color_end);
            printf("urlEncode:          %s%s%s\n", _color_lightred, str4.c_str(), _color_end);
            printf("\n");
        }
        else if (strType =="decode")
        {
            std::string strInput(argv[2]);
            CMyString str5;
            bool ret = CHttpWrapper::urlDecode(strInput, str5);
            if (!ret)
            {
                printf("%scipher [%s] can not decode%s\n", _color_red, strInput.c_str(), _color_end);
                return -7;
            }
            printf("cipher: %s\n", argv[2]);
            printf("origin: %s%s%s\n", _color_lightred, str5.c_str(), _color_end);
            printf("\n");
        }
        else
        {
            printf("%sunknown type [%s]%s\n\n", _color_red, strType.c_str(), _color_end);

            Syntax(argv[0]);
            return -3;
        }

        return 0;
    }

    std::string strInput(argv[3]);
    CMyString str;
    CMyString strTemp(strInput);
    // app <type> <mode> <string>
    CMyString strType(argv[1]);
    if (strType == "encode")
    {
        int32_t mode = atoi(argv[2]);
        switch(mode)
        {
        case 1:
            str = CHttpWrapper::escape(strInput);
            printf("origin: %s\n", argv[3]);
            printf("escape: %s%s%s\n", _color_lightred, str.c_str(), _color_end);
            break;
        case 2:
            str = CHttpWrapper::encodeURI(strInput);
            printf("origin:    %s\n", argv[3]);
            printf("encodeURI: %s%s%s\n", _color_lightred, str.c_str(), _color_end);
            break;
        case 3:
            str = CHttpWrapper::encodeURIComponent(strInput);
            printf("origin:             %s\n", argv[3]);
            printf("encodeURIComponent: %s%s%s\n",_color_lightred,  str.c_str(), _color_end);
            break;
        case 4:
            str = CHttpWrapper::urlEncode(strTemp);
            printf("origin:    %s\n", argv[3]);
            printf("urlEncode: %s%s%s\n",_color_lightred,  str.c_str(), _color_end);
            break;
        default:
            printf("%sinvalid mode [%s]%s\n\n", _color_red, argv[2], _color_end);
            Syntax(argv[0]);
            break;
        }
    }
    else
    {
        printf("syntax error.\n");

        Syntax(argv[0]);
        return -5;
    }

    return 0;
}
