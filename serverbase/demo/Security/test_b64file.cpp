/*
 * =====================================================================================
 *
 *       Filename:  test_b64file.cpp
 *
 *    Description:  test for base64
 *
 *        Version:  1.0
 *        Created:  2020年04月08日 02时25分28秒
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

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("syntax: %s <input file> <output file>\n");
        return -1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        printf("can not find file [%s]\n", argv[1]);
        return -2;
    }
    fseek(fp, 0, SEEK_END);
    int32_t cbData = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *pc = (char*)calloc(1, cbData+1);
    fread(pc, 1, cbData, fp);
    fclose(fp);

    CMyString strOut = CBase64Wrapper::Encode(pc, cbData, false);
    free(pc);

    FILE *fp2 = fopen(argv[2], "wb");
    if (NULL == fp2)
    {
        printf("can not open file %s to write.\n", argv[2]);
        return -3;
    }
    fwrite(strOut.c_str(), 1, strOut.length(), fp2);
    fflush(fp2);
    fclose(fp2);

    printf("\ntest finished.\n");
    return 0;
}
