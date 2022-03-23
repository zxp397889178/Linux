/*
 * =====================================================================================
 *
 *       Filename:  MyRedis.h
 *
 *    Description:  wrapper of HiRedis
 *
 *        Version:  1.0
 *        Created:  04/24/2015 09:49:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.gamefunx.com
 *
 * =====================================================================================
 */

#ifndef _GFX_DATABASE_MYREDIS_H_
#define _GFX_DATABASE_MYREDIS_H_

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include <hiredis/hiredis.h>

#define INFINITE			(uint32_t)-1

struct BinaryInfo
{
	size_t cbData;
	void *pData;
	BinaryInfo()
	{
		cbData = 0;
		pData = NULL;
	}
};

struct BinaryData
{
    CMyString   key;
    byte_t*     pData;
    size_t      cbData;
    BinaryData()
        : key(), pData(NULL), cbData(0)
    {
    }
    BinaryData(const CMyString &_key, byte_t* _data, size_t _len)
        : key(_key), pData(_data), cbData(_len)
    {
    }
};
typedef std::vector<BinaryData>     VEC_BINARY;
class CMyRedis
{
public:
    enum ContainDirection { HEAD = 1, TAIL };               // 容器的方向，头或尾

    // 时间类型：毫秒, 秒, 不限制
    enum TimeStyle { MICROSECOND = 0, SECOND, INFINITE2 };

	CMyRedis();
	~CMyRedis();

public:
	/* 打开redis服务器
	 * 参数: strHost	redis服务器的地址
	 * 		 port		端口，缺省为6379
     * 		 passwd     密码，如果为空则不检查密码
	 * 		 timeout	超时，以毫秒计算
	 * */
	bool Open(std::string const strHost, uint32_t port, 
            std::string const strPasswd, uint32_t timeout);

	/* 切换到指定的数据库.
	 * 数据库索引号从0开始,默认使用0号数据库
	 * */
	bool Select(int32_t dbIndex);

	/* 测试数据库是否正常服务
     * timeout: 与最近一次执行的间隔时间，以秒计算， 0表示立即执行
     * 返回值：-1 出错了，0 不需要ping, 1 ping 成功
     *
	 * */
	int32_t Ping(uint32_t timeout);

    void Close(void);

public:
    // 写入数值型内容
    int32_t SetNumber(const CMyString &strKey, int32_t val, enum TimeStyle timeStyle=INFINITE2, int32_t timeValue=1000);
    int32_t SetNumber(const CMyString &strKey, int64_t val, enum TimeStyle timeStyle=INFINITE2, int32_t timeValue=1000);
    int32_t SetNumber(const CMyString &strKey, uint32_t val, enum TimeStyle timeStyle=INFINITE2, int32_t timeValue=1000);

    // 获得数值型内容
    int32_t GetNumber(const CMyString &strKey, int32_t &val);
    int32_t GetNumber(const CMyString &strKey, int64_t &val);
    int32_t GetNumber(const CMyString &strKey, uint32_t &val);

    // 写入文本数据 
    int32_t SetData(const CMyString &strKey, const CMyString &strData, enum TimeStyle=INFINITE2, int32_t timeValue=1000);
    int32_t GetData(const CMyString &strKey, CMyString &strData);

    // 二进制存储函数
    int32_t SetBinary(const CMyString &strKey, const byte_t *pData, size_t cbData, 
            enum TimeStyle timeStyle = INFINITE2, int32_t timeValue = 1000);
    int32_t SetBinary(BinaryData &data, enum TimeStyle timeStyle = INFINITE2, int32_t timeValue = 1000);
    //批量写入二进制数据, 无法设置超时
    int32_t SetBinary(VEC_BINARY &vecBinary);

    int32_t GetBinary(const CMyString &strKey, CByteStream &buff);


public:
	/*  设置指定key的值
	 *  param:	strKey
	 *  		strValue
	 *  		pData		传入的二进制值，长度为cbData
	 *  		timeout		超时时间，以毫秒计算
	 *  		timestyle   [0=>timeout以毫秒计算; 1=>timeout以秒计算]
	 *  return: -1 			出错
	 *  		 0			失败
	 *  		 1			处理成功
	 * */
	int32_t Set(std::string const strKey, std::string const strValue, 
			uint32_t timeout = INFINITE, int32_t timeStyle=0);
	int32_t Set(std::string const strKey, void *pData, size_t cbData, 
			uint32_t timeout = INFINITE, int32_t timeStyle=0);
	int32_t Set(std::string const strKey, int64_t val, 
			uint32_t timeout = INFINITE, int32_t timeStyle=0);

	/*  获得key的值
	 *  param:	strKey
	 *  		strValue	字符串的值
	 *  		pData		传入的二进制值，需要手工释放内存
	 *  		cbData		pData值的长度
	 *  return: -1			出错
	 *  		 0			获取失败
	 *  		 1			处理成功
	 *  */
	int32_t Get(std::string const strKey, std::string& strValue);
	//int32_t Get(std::string const strKey, void* &pData, size_t &cbData);
	int32_t Get(std::string const strKey, int64_t& val);

    // 此函数与Get的区别是，不需要手动释放pData
    //int32_t getData(std::string const strKey, void* pData, size_t &cbData);

	/* 删除主键
	 * 返回值: -1 出错
	 * 			0 未删除
	 * 			1 删除成功
	 * */
	int32_t Del(std::string const strKey);

	/* 判断主键是否存在
	 * 返回值: -1 出错
	 * 			0 不存在
	 * 			1 存在
	 */
	int32_t IsExists(std::string const strKey);

	/*  对 strKey 进行增加或减少value值
	 *  param:	strKey				KEY
	 *  		incValue/decValue	增加或减少的值
	 *  		retVal				增加或减少后的值
	 *  return: -1  系统错误
	 *  		 1  处理成功
	 *  */
	int32_t IncBy(std::string const strKey, int32_t incValue, int64_t& retVal);
	int32_t DecBy(std::string const strKey, int32_t decValue, int64_t& retVal);
	/* 返回 strKey 的实际长度
	 * return: -1  系统错误,或 strKey存储的不是一个字符串值
	 * 			0  key不存在
	 * 		   >0  key存储字符串的长度
	 * */
	int32_t StrLen(std::string const strKey);

	/* 设置过期时间
	 * 参数: strKey		key值
	 * 		 timeout 	超时时间（秒或毫秒）
	 * 		 timeStyle	0 以毫秒计算; 1以秒计算
	 * 返回值: -1 失败或出错　
	 * 			0 没有这个key
	 * 			1 设置成功
	 * */
	int32_t SetExpire(std::string const strKey, uint32_t timeout, int32_t timeStyle=0);

	/* 得到过期时间
	* 参数: strKey		key值
	* 		 timeout 	超时时间（秒或毫秒）
	* 返回值: -1 失败或出错　
	* 			0 没有这个key
	* 			1 设置成功
	* */
	int32_t GetExpire(std::string const strKey, uint32_t &val);

	/* 获得符合patten模式的所有key
	 * strPatten: 可以有?,*
	 * */
	int32_t Keys(std::string const strPatten, std::vector<std::string> &arKeys);

    /* 获得符合patten模式的前n个key
     * patten: 可以用 ?, *，空表示不做限制
     * count: 返回记录的最大个数，<=0 表示不作限制
     * vecKey: 返回符合模式的全部keys
     * next_cursor: 下一次循环所使用cursor, =0表示已全部处理好
     * 返回值： 0   表示操作成功
     *          <0  表示错误
     * */
    int32_t Scan(int32_t cursor, const CMyString &patten, int32_t count, std::vector<CMyString> &vecKey, int32_t &next_cursor);

    int32_t Rename(const CMyString &fromKey, const CMyString &toKey, bool bOverrideNew=false);
public:
	// 以下针对hash表操作
	/* 设置hash值
	 * 参数: strKey		key
	 * 		 strField	field
	 * 		 strValue	值
	 * 返回值: -1 出错或失败
	 * 			0 处理成功，并覆盖旧值
	 * 			1 处理成功，增加新值
	 * */
	int32_t HashSet(std::string const strKey, std::string const strField, std::string const strValue);
	int32_t HashSet(std::string const strKey, std::string const strField, void *pData, size_t cbData);
	int32_t HashSet(std::string const strKey, std::string const strField, int64_t val);

	/* 获取hash值
	 * 参数: strKey		key
	 * 		 strField	field
	 * 		 strValue	值
	 * 返回值: -1 出错或失败
	 * 			0 无此key:field 的值 
	 * 			1 处理成功
	 * */
	int32_t HashGet(std::string const strKey, std::string const strField, std::string &strValue);
	int32_t HashGet(std::string const strKey, std::string const strField, void* &pData, size_t &cbData);
	int32_t HashGet(std::string const strKey, std::string const strField, int64_t &val);

	/* 删除hash值
	 * 参数: strKey		key
	 * 		 strField	field
	 * 返回值: -1 出错或失败
	 * 			0 无此key:field 的值 
	 * 			1 处理成功
	 * */
	int32_t HashDel(std::string const strKey, std::string const strField);

	/* 判断hash值是否存在
	 * 参数: strKey		key
	 * 		 strField	field
	 * 返回值: -1 出错或失败
	 * 			0 无此key:field 的值 
	 * 			1 存在此key:field
	 * */
	int32_t IsHashExists(std::string const strKey, std::string const strField);

	/* 增加hash值
	 * 参数: strKey		key
	 * 		 strField	field
	 * 		 incValue	增量的值
	 * 		 retValue	返回的值
	 * 返回值: -1 出错或失败
	 * 		    1 处理成功
	 * */
	int32_t IncHashValue(std::string const strKey, std::string const strField, 
			int32_t incValue, int64_t &retValue);

	/* 获取key中对应的field的总数
	 * return -1 出错或失败
	 * 		   0 无此 key
	 * 		  >0 hash key的总数 
	 * */
	int32_t GetFieldCount(std::string const strKey);

// --------------------------- 以下操作为批量处理 -------------------------------
//
	/* 获得strKey对应的所有Field
	 * return -1 出错或失败
	 * 		   0 无此 key
	 * 		  >0 field数量 
	 * */
	int32_t GetHashFields(std::string const strKey, std::vector<CMyString> &allField);

	/* 获得strKey寻应的所有value（忽略field）
	 * return -1 出错或失败
	 * 		   0 无此 key
	 * 		  >0 value数量 
	 * */
	int32_t GetHashValues(std::string const strKey, std::vector<CMyString> &allValue);

	/* 获取指定key的全部数据
	 * 参数  strKey		key
	 * 		 values		key-value 映射对
	 * 返回值: -1 出错或失败
	 * 			0 无此key
	 * 		   >0 values中的值的数量
	 * */
	int32_t HashGetAll(std::string const strKey, std::map<CMyString, CMyString> &values);

	/* 获取指定key的全部数据(二进制数据)
	* 参数  strKey		key
	* 		 values		key-value 映射对
	* 返回值: -1 出错或失败
	* 			0 无此key
	* 		   >0 values中的值的数量
	* */
	int32_t HashGetAll(std::string const strKey, std::map<CMyString, BinaryInfo> &values);

	/* 设置指定key下的全部值为val
	 * return 	-1 出错或失败
	 * 			 0 无此key
	 * 			>0 成功设置的数量
	 * */
	int32_t HashSetAll(std::string const strKey, int64_t val);
	int32_t HashSetAll(std::string const strKey, std::string const strVal);

	/* 删除 strKey 下的全部Field
	 * return	-1 出错或失败
	 * 			 0 
	 * */
	int32_t HashDelAll(std::string const strKey);

public:
    // 以下针对list表进行操作，常规是从尾部进，从首部出
    
    // 调用 RPUSH, 成功返回1，失败返回0，若key_不是list返回-1
    int32_t LPush(const CMyString &key_, const CMyString &value_, enum ContainDirection direct_ = CMyRedis::TAIL);
    // 调用 LPOP, 成功返回1，无数据返回0，若key_不是list返回-1
    int32_t LPop(const CMyString &key_, CMyString &value_, enum ContainDirection direct_ = CMyRedis::HEAD );
    // 调用LLEN, 返回key_中有数据的数量，若key_不是list返回-1
    int32_t LSize(const CMyString &key_);

    const char* getLastError(void) { return m_strLastError.c_str(); }
/*
public:
	// 以下针对list表操作
	bool ListSet(std::string const strKey, int32_t idx, std::string const strValue);
	bool ListSet(std::string const strKey, int32_t idx, void *pData, size_t cbData);
	bool ListGet(std::string const strKey, int32_t idx, std::string &strValue);
	bool ListGet(std::string const strKey, int32_t idx, void* &pData, size_t &cbData);
*/

public:
	// 以下针对set进行操作
	int32_t SAdd(std::string const strKey, std::string const strMember);
	int32_t SAdd(std::string const strKey, int32_t value);
	int32_t SAdd(std::string const strKey, uint32_t value);
	int32_t SAdd(std::string const strKey, int64_t value);
	int32_t SDel(std::string const strKey, std::string const strMember);
	int32_t SDel(std::string const strKey, int32_t value);
	int32_t SDel(std::string const strKey, uint32_t value);
	int32_t SDel(std::string const strKey, int64_t value);
	int32_t SMembers(std::string const strKey, std::vector<std::string> &arMember);
	int32_t SIsExists(std::string const strKey, std::string const strMember);
	int32_t SCount(std::string const strKey);
	int32_t SDelKey(std::string const strKey);

private:
    int32_t execScan(const CMyString &cmd, std::vector<CMyString> &vecKeys, int32_t &nCursor);
	int32_t ExecuteRedisCmd(redisReply *&pReply, const char *fmt, ...);

private:
	redisContext	*m_pContext;
    uint64_t        m_lastHeartbeat;

    CMyString       m_strLastError;
};

#endif	// _GFX_DATABASE_MYREDIS_H_
