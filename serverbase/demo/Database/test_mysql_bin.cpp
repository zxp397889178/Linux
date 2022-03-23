/*
 * =====================================================================================
 *
 *       Filename:  test_mysql_bin.cpp
 *
 *    Description:  测试读写 varbinary字段
 *
 *        Version:  1.0
 *        Created:  2020年03月29日 17时51分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "MyDB.h"

int32_t LoadQrcodeData(const CMyString &strFile, CMyString &strData)
{
    FILE *fp = fopen(strFile.c_str(), "rb");
    if (fp == NULL)
    {
        printf("can not open qrcode file [%s]\n", strFile.c_str());
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int32_t cnt = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    printf("file %s size %d\n", strFile.c_str(), cnt);
#if 1
    uint8_t buff[2] = {0, };
    while(!feof(fp))
    {
        size_t r = fread(buff, 1, 1, fp);
        if (r> 0)
        {
            uint8_t c = buff[0];
            strData.Append("%02x", c);
        }
    }
    fclose(fp);
#else
    char buffer[2048] = "";
    size_t r = fread((void*)&buffer[0], 1, cnt, fp);
    fclose(fp);
    for(int32_t i(0); i<r; ++i)
    {
        uint8_t c = buffer[i];
        strData.Append("%02x", c);
    }
#endif
    printf("%s\n", strData.c_str());
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("syntax: %s <db password> <qrcode-file>\n", argv[0]);
        return -1;
    }

    ISimDB *pDB = CreateMysqlConn("yuzp", argv[1], "119.3.59.200", "supergm", 3306,0, 0, "utf8mb4" );
    if (NULL == pDB)
    {
        printf("connect to database failure.\n");
        return -2;
    }
    CAutoDB guard(pDB);

    pDB->Cmd("SELECT `data`, `id` FROM test");
    while(pDB->More())
    {
        int32_t id = pDB->GetInt(1);
        byte_t *pData(NULL);
        size_t cbData(0);
        pDB->GetData(0, pData, cbData);
        CMyString strTo;
        dumpbin(pData, cbData, strTo);
        printf("row %d, len:%d\n%s\n", id, cbData, strTo.c_str());
    }

    // 二维码写入
    CMyString strData;
    int32_t ret = LoadQrcodeData(argv[2], strData);
    if (ret < 0)
    {
        return -3;
    }
    pDB->Cmd("UPDATE test SET `data`=x'%s' WHERE `id`=1", strData.c_str());
    ret = pDB->Exec();
    if (ret > 0)
    {
        printf("update binary data succeed.\n");
    }

    printf("test varbinary finished.\n");
    return 0;
}
