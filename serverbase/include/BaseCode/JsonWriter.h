/*
 * =====================================================================================
 *
 *       Filename:  JsonWriter.h
 *
 *    Description:  write json file
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 14时36分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _QW_CRAZYFISH_JSON_WRITER_H_
#define _QW_CRAZYFISH_JSON_WRITER_H_

#include "BaseCode.h"
#include "json/json.h"


class DEPRECATED("Use CJsonWrapper instead.") CJsonWriter
{
public:
	CJsonWriter();
	~CJsonWriter();

    bool AddJsonStr(const CMyString &strJson);

	void Set(const CMyString &strKey, const CMyString &strValue);
	void Set(const CMyString &strKey, int32_t val);
	void Set(const CMyString &strKey, uint32_t val);
	void Set(const CMyString &strKey, int64_t val);
	void Set(const CMyString &strKey, uint64_t val);
	void Set(const CMyString &strKey, double val);

    void Set(const CMyString &strKey, CJsonWriter &root);
    void Set(const CMyString &strKey, Json::Value &val);

    Json::Value& getJsonValue(const CMyString &strKey);

    CMyString c_str(void);
    CMyString asString(bool bStyled);

    Json::Value& getJson(void) { return m_root; }
private:
    Json::Value     m_root;
};

#endif // _QW_CRAZYFISH_JSON_WRITER_H_



