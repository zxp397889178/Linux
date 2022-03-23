/*
 * =====================================================================================
 *
 *       Filename:  test_httpmaker.cpp
 *
 *    Description:  test HttpRespMaker
 *
 *        Version:  1.0
 *        Created:  03/13/2020 11:33:25 AM
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
#include "MyNet.h"

int main(int argc, char **argv)
{
    http::CHttpRespMaker crm;
    crm.add("name", "yuzp");
    crm.add("sex", "Male");
    crm.add("age", 45);
    crm.add("salary", 1000.0);
    crm.add("authed", true);

    CMyString str1 = crm.http_str(true);
    CMyString str2 = crm.http_str(false);
    printf("[HTTP1]sorted     [%s]\n", str1.c_str());
    printf("[HTTP1]not sorted [%s]\n", str2.c_str());

    crm.add("salary", 1000.49);
    CMyString str3 = crm.json_str(true);
    CMyString str4 = crm.json_str(false);
    printf("[JSON1]sorted     [%s]\n", str3.c_str());
    printf("[JSON1]not sorted [%s]\n", str4.c_str());

    crm.add("salary", 1000.4902);
    CMyString str5 = crm.json_str_styled(true);
    CMyString str6 = crm.json_str_styled(false);
    printf("[JSON2]sorted     [%s]\n", str5.c_str());
    printf("[JSON2]not sorted [%s]\n", str6.c_str());

    crm.add("salary", 1000.490);
    CMyString str7 = crm.http_str_manual(';',':',false, false);
    CMyString str8 = crm.http_str_manual(';',':',false, true);
    printf("[HTTP2]sorted     [%s]\n", str8.c_str());
    printf("[HTTP2]not sorted [%s]\n", str7.c_str());

    printf("\n");
    return 0;
}
