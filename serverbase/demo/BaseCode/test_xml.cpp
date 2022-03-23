/*
 * =====================================================================================
 *
 *       Filename:  test_xml.cpp
 *
 *    Description:  test for xml read
 *
 *        Version:  1.0
 *        Created:  04/04/2020 08:18:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("%s <config.xml>\n", argv[0]);
        return -1;
    }

    CXmlFile xml;
    if (!xml.Load(argv[1]))
    {
        printf("%s not a valid xml\n", argv[1]);
        return -2;
    }

    CMyString strDB = xml.GetString("main/supergm", "maindb");
    printf("maindb=>%s\n", strDB.c_str());

    printf("\ntest finished.\n");

    return 0;
}
