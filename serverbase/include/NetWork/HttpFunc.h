/*
 * =====================================================================================
 *
 *       Filename:  HttpFunc.h
 *
 *    Description:  http 公用的函数
 *
 *        Version:  1.0
 *        Created:  2020年02月13日 23时17分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "BaseCode.h"
#include "http/HttpData.h"
#include "http/HttpRespone.h"
#include "NetFunc.h"

int32_t HttpSendFile(const CMyString &strUrl, const CMyString &strFile, http::CHttpRespone &resp);

bool GetContentType(const CMyString &strFile, CMyString &strContentType);
