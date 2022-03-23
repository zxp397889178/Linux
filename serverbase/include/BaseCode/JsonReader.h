/*
 * =====================================================================================
 *
 *       Filename:  JsonReader.h
 *
 *    Description:  json 读取功能
 *
 *        Version:  1.0
 *        Created:  2016年04月04日 22时06分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_AUTHSVR_JSON_READER_H_
#define _QW_CRAZYFISH_AUTHSVR_JSON_READER_H_
#pragma once 

#include "BaseCode.h"
#include "json/json.h"

class DEPRECATED("Use CJsonWrapper instead.") CJsonReader 
{
public:
	CJsonReader();
	~CJsonReader();

    bool Init(const CMyString &jsonstr);
	//bool Init(const char *pszText);
    void Init(Json::Value &val);

	bool IsExists(const char *pszKey);

	bool GetInt(const char *pszKey, int32_t &val);
	bool GetUint32(const char *pszKey, uint32_t &val);
	bool GetInt64(const char *pszKey, int64_t &val);
	bool GetDouble(const char *pszKey, double &val);
	bool GetString(const char *pszKey, CMyString &val);
	bool GetBool(const char *pszKey, bool &val);
	
public:
    int32_t getInt(const CMyString &strKey);
    uint32_t getUint32(const CMyString &strKey);
    int64_t getInt64(const CMyString &strKey);
    uint64_t getUint64(const CMyString &strKey);
    double getDouble(const CMyString &strKey);
    bool getBool(const CMyString &strKey);
    CMyString getString(const CMyString &strKey);
    const char* getCString(const CMyString &strKey);
    bool getObject(const CMyString &strKey, Json::Value &obj);
    bool getReader(const CMyString &strKey, CJsonReader &reader);

    Json::Value& getJson(void) { return m_root; }

private:
    bool StringToJson(const CMyString &str, Json::Value &json_val);

private:
	Json::Value 	m_root;
};

#endif  // _QW_CRAZYFISH_AUTHSVR_JSON_READER_H_



