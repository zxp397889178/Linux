/*
 * =====================================================================================
 *
 *       Filename:  test_range.cpp
 *
 *    Description:  test range value
 *
 *        Version:  1.0
 *        Created:  2021年03月12日 11时02分31秒
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
#include <stdarg.h>
#include <vector>
#include <algorithm>

class CRange
{
public:
    CRange(int num_args, ...)
    {
        va_list ap;
        int val(0);
        int i(0);
        va_start(ap, num_args);
        for(i=0; i<num_args; ++i)
        {
            val = va_arg(ap, int);
            printf("idx:%d, val:%d\n", i, val);
            m_array.push_back(val);
        }
        va_end(ap);
    }
    ~CRange(){  }

    bool Find(int v)
    {
        if (std::find(m_array.begin(), m_array.end(), v) != m_array.end())
        {
            return true;
        }
        return false;
    }
private:
    std::vector<int>    m_array;
};

bool InRange(int32_t v1, int32_t num_args, ...)
{
    bool bFind(false);
    va_list ap;
    va_start(ap, num_args);
    for(int32_t i(0); i<num_args; ++i)
    {
        int32_t val = va_arg(ap, int32_t);
        if (val == v1)
        {
            bFind = true;
            break;
        }
    }
    va_end(ap);
    return bFind;
}

bool InRangeEx(int32_t v1, ...)
{
    bool bFind(false);
    va_list ap;
    va_start(ap, v1);
    do{
        int32_t val = va_arg(ap, int32_t);
        if (val==-1)
        {
            break;
        }
        if (val == v1)
        {
            bFind = true;
            break;
        }
    }while(true);
    va_end(ap);
    return bFind;
}


int main(int argc, char **argv)
{
    CRange rv(9, 1,2,3,4,5,6,7,8,9);
    if (rv.Find(11))
    {
        printf("\nfind value:11\n");
    }
    else
    {
        printf("\nnot found value:11\n");
    }
    if (rv.Find(8))
    {
        printf("\nfind value:8\n");
    }

    int32_t v1(8);
    if (InRange(v1, 9, 1,2,3,4,5,6,7,8,9))
    {
        printf("\nfind %d\n", v1);
    }
    if (InRangeEx(v1, 1,2,4,5,6,7,9,11,22,21,-1))
    {
        printf("\nfind %d\n", v1);
    }
    else
    {
        printf("\nnot find %d\n", v1);
    }
    return 0;
}
