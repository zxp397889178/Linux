/*
 * =====================================================================================
 *
 *       Filename:  test_json.cpp
 *
 *    Description:  test for jsoncpp
 *
 *        Version:  1.0
 *        Created:  03/11/2020 07:43:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include <string>
#include <algorithm>
#include "BaseCode/JsonWriter.h"

/*
int32_t my_isendl(char c)
{
    if (c=='0x0D' || c==0x0A)
        return true;
    return false;
}

void trim_endl(std::string &str)
{
    std::string::reverse_iterator p = std::find_if(str.rbegin(), str.rend(),
            std::not1(std::ptr_fun(my_isendl)));
    str.erase(p.base(), str.end());
}
*/

int main(int argc, char **argv)
{
    CJsonWriter jw;
    jw.Set("name", "yuzp");
    jw.Set("sex", "male");
    jw.Set("age", 45);
    std::string str3 = jw.c_str();
    printf("[%s]\n", str3.c_str());
    //CMyString str4;
    //dumpbin((void*)str3.c_str(), str3.length(), str4);
    //printf("[%s]\n", str4.c_str());
    trim_endl(str3);
    printf("0>[%s]\n", str3.c_str());

/*
    Json::Value data;
    data["name"] = "yuzp";
    data["sex"] = "male";
    data["age"] = 45;
    Json::FastWriter w1;
    CMyString str1 = w1.write(data);
    Json::StyledWriter w2;
    CMyString str2 = w2.write(data);
    printf("1>[%s]\n", str1.c_str());
    printf("2>[%s]\n", str2.c_str());
*/    
    return 0;
}
