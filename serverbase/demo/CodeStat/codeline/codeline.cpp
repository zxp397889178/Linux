/*
 * =====================================================================================
 *
 *       Filename:  codeline.cpp
 *
 *    Description:  统计svn diff 出来的代码行数
 *
 *        Version:  1.0
 *        Created:  2020年08月12日 17时00分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

void ltrim(char* &text)
{
    int32_t step(0);
    size_t cnt = strlen(text);
    for(size_t i(0); i<cnt; ++i)
    {
        unsigned char c = text[i];
        if (isspace(c))
        {
            ++step;
        }
    }
    char *pc = &text[step];
    text = pc;
}

/* 返回值： 0 非注释行
 *          1 单行注释 # //
 *          2 多行注释开始
 *          3 多行注释结束
 * */
int IsCommentLine(const char* text, size_t cnt)
{
    if (text[0] == '#')
    {
        return 1;
    }

    if (strncmp(text, "//", 2)==0)
    {
        return 1;
    }

    if (strncmp(text, "/*", 2) == 0)
    {
        if (strstr(text, "*/") != NULL)
        {
            return 1;
        }
        return 2;
    }

    if (strstr(text, "*/") != NULL)
    {
        return 3;
    }

    return 0;
}

// 返回当前的时间（微秒数）
unsigned long int now()
{
    struct timeval tv = {0};
    //struct timezome tz;
    gettimeofday(&tv, NULL);
    unsigned long int val(0);
    val = (unsigned long int)(tv.tv_sec*1000000) + (tv.tv_usec);
    return val;
}

int main(int argc, char **argv)
{
    unsigned long int t1 = now(); 
    if (argc < 2)
    {
        printf("syntax: %s <svn diff file>\n", argv[0]);
        return -1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (NULL == fp)
    {
        printf("open svn diff file [%s] failure.\n", argv[1]);
        return -2;
    }

    char *line(NULL);
    size_t len(0);
    int nLine(0);
    bool bMultiComment(false);
    do{
        if (line != NULL)
        {
            free(line);
        }
        len = 0;
        ssize_t read = getline(&line, &len, fp);
        if (read == -1)
        {
            // 结束
            break;
        }

        if (line[0]=='\0')
        {
            // 空行
            continue;
        }

        unsigned char c = line[0];
        // 只读取首字节为 '+'的行
        if (c != '+')
        {
            continue;
        }
        if (strncmp(line, "+++", 3)==0)
        {
            continue;
        }

        char *pc = &line[1];
        ltrim(pc);  // 去除头部的空格和TAB

        size_t len = strlen(pc);
        if (len == 0)
        {
            // 空行
            continue;
        }

        // 判断是否注释行
        int commentLn = IsCommentLine(pc, len);
        switch(commentLn)
        {
        case 0:
            if (!bMultiComment)
            {
                ++nLine;
            }
            break;
        case 1:
            // 单行注释
            break;
        case 2:
            // 多行注释，要直达结束才可以
            bMultiComment = true;
            break;
        case 3:
            bMultiComment = false;
            break;
        }

    }while(true);
    if (line != NULL)
    {
        free(line);
    }
    fclose(fp);

    unsigned long int t2 = now(); 
    unsigned int diff = (unsigned int)(t2-t1);
    char cost[128] = "";
    if (diff > 1000000)
    {
        sprintf(cost, "%.2lfs", diff/1000000.0);
    }
    else if (diff > 1000)
    {
        sprintf(cost, "%.2lfms", diff/1000.0);
    }
    else
    {
        sprintf(cost, "%dus", diff);
    }
//    printf("svn diff file [%s] has %d valid line, cost:%s\n", argv[1], nLine, cost);
    printf("%d %s", nLine, cost); 
    return nLine;
}


