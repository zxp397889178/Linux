/*
 * =====================================================================================
 *
 *       Filename:  test_json2.cpp
 *
 *    Description:  test boolean json
 *
 *        Version:  1.0
 *        Created:  2021年04月26日 21时51分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "BaseCode/JsonWriter.h"
#include "BaseCode/JsonReader.h"

int32_t main(int32_t argc, char **argv)
{
    CMyString jsonstr="{\"purchaseTimeMillis\": \"1619420112024\", \"purchaseState\": 2, \"consumptionState\": 0, \"developerPayload\": \"\", \"orderId\": \"GPA.3331-4582-0859-83361\", \"purchaseType\": 0, \"acknowledgementState\": 0, \"kind\": \"androidpublisher#productPurchase\", \"regionCode\": \"IN\", \"preordered\": False}";

    CJsonReader jr;
    printf("json string[%s]\n", jsonstr.c_str());
    bool ret = jr.Init(jsonstr);
    if (!ret)
    {
        printf("parse json string failure.\n");
        return -1;
    }

    printf("json prase ok.\n");

    if (jr.getBool("preordered"))
    {
        printf("preordered is true.\n");
    }
    else
    {
        printf("preordered is false.\n");
    }
    return 0;
}
