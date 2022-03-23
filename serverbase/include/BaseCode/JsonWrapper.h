/*
 * =====================================================================================
 *
 *       Filename:  JsonWrapper.h
 *
 *    Description:  对jsoncpp的读写封装
 *
 *        Version:  1.0
 *        Created:  2021年07月28日 01时34分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once
#include "BaseCode.h"
#include "json/json.h"

class CJsonWrapper
{
public:
    CJsonWrapper();
    explicit CJsonWrapper(const CMyString &str);
    explicit CJsonWrapper(Json::Value &val);
    ~CJsonWrapper();

    bool Load(const CMyString &str);
    void Load(Json::Value &val);
    bool Append(const CMyString &json_str, bool overrideSameKey = false);
    void Append(Json::Value &json_val, bool overrideSameKey = false);

    bool getJsonObject(const CMyString &key, Json::Value &json_val);

public:
    // 以下函数是为了兼容 CJsonReader, CJsonWriter, Json::FastWriter, Json::Reader 而定义的
    bool Init(const CMyString &str) { return this->Load(str); }
    bool parse(const CMyString &str) { return this->Load(str); }
    CMyString write(Json::Value &val);

public:
    bool Read(const CMyString &key, int32_t &val); 
    bool Read(const CMyString &key, uint32_t &val);
    bool Read(const CMyString &key, int64_t &val);
    bool Read(const CMyString &key, uint64_t &val);
    bool Read(const CMyString &key, float &val);
    bool Read(const CMyString &key, double &val);
    bool Read(const CMyString &key, bool &val);
    bool Read(const CMyString &key, CMyString &str);
    bool Read(const CMyString &key, char *str, size_t cbLen);
    bool Read(const CMyString &key, Json::Value &val);
    Json::Value& operator[](const CMyString &key);

    void Write(const CMyString &key, int32_t val);
    void Write(const CMyString &key, uint32_t val);
    void Write(const CMyString &key, int64_t val);
    void Write(const CMyString &key, uint64_t val);
    void Write(const CMyString &key, float val);
    void Write(const CMyString &key, double val);
    void Write(const CMyString &key, bool val);
    void Write(const CMyString &key, const CMyString &str);
    void Write(const CMyString &key, const char *str);
    void Write(const CMyString &key, const Json::Value &val);

    bool IsExists(const CMyString &key);
    void Remove(const CMyString &key);
    void Clear(void);

    // 以下函数定义是为了便捷使用，建议还是采用Read来操作
public:
    int32_t getInt(const CMyString &key, int32_t defVal=0);
    uint32_t getUInt(const CMyString &key, uint32_t defVal=0);
    int64_t getInt64(const CMyString &key, int64_t defVal=0L);
    uint64_t getUInt64(const CMyString &key, uint64_t defVal=0L);
    bool getBool(const CMyString &key, bool defVal = false);
    float getFloat(const CMyString &key, float defVal = 0.0);
    double getDouble(const CMyString &key, double defVal = 0.0);
    CMyString getString(const CMyString &key, const CMyString &defVal="");

public:
    CMyString toString(void);
    CMyString toStyledString(void);

    Json::Value&        json(void)      { return m_root; }
    const Json::Value&  c_json(void)    { return m_root; }
    Json::Value&        getJson(void)   { return m_root; }

private:
    bool StringToJson(const CMyString &str, Json::Value &json_val);

private:
    Json::Value     m_root;
};

