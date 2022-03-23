/*
 * =====================================================================================
 *
 *       Filename:  test_strfunc.cpp
 *
 *    Description:  test safe_strtok
 *
 *        Version:  1.0
 *        Created:  02/06/2020 05:08:35 PM
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
#include <vector>

int main(int argc, char **argv)
{
    typedef std::vector< std::string >  StringArray;
    StringArray arElement;
    const char *element_path="main/paycenter";
    int32_t size = safe_strtok(element_path, arElement, "/");
    printf("size=%d\n", size); 
    return 0;

}
