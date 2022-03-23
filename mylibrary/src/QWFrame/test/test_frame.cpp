/*
 * =====================================================================================
 *
 *       Filename:  QWFrame.cpp
 *
 *    Description:  session manager, for accept callback
 *
 *        Version:  1.0
 *        Created:  2015年09月08日 16时44分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zengyh, zengyh@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "../QWFrame.h"

// 传到BgSvr写数据库日志
struct protoServerDbLog : MSG_HEAD
{
	enum { IDD = 5006, };

	int32_t		priority;		// 优先级: 1-9, 缺省为1级
	char		szTable[128];	// 表名
	char		szItem[512];	// 项目数据

	protoServerDbLog();
	int32_t Serialize(void *pBuf, size_t cbBuf);
	int32_t Deserialize(void *pBuf, size_t cbBuf);
	int32_t getProtoLen(void);
};
/////////////////////////////////////////////////////////
protoServerDbLog::protoServerDbLog()
{
	protoId = protoServerDbLog::IDD;
	protoLen = 0;
	checkCode = 0;
	priority = 1;
}

int32_t protoServerDbLog::Serialize(void *pBuf, size_t cbBuf)
{
	INIT_SERIALIZE_ENV;

	SERIALIZE_DATA(priority, -4);
	SERIALIZE_CHAR(szTable, -5);
	SERIALIZE_CHAR(szItem, -6);

	GENERATE_CODE;
	return (int32_t)offset;
}

int32_t protoServerDbLog::Deserialize(void *pBuf, size_t cbBuf)
{
	INIT_DESERIALIZE_ENV;

	DESERIALIZE_DATA(priority, -4);
	DESERIALIZE_CHAR(szTable, -5);
	DESERIALIZE_CHAR(szItem, -6);

	return (int32_t)offset;
}

int32_t protoServerDbLog::getProtoLen(void)
{
	return sizeof(MSG_HEAD)
		+sizeof(int32_t)
		+LENGTH_STR(szTable)
		+ LENGTH_STR(szItem);
}

////////////////////////////////////////////////////
class Test : public IMessage
{
public:

	bool Init()
	{
		REGISTER_MSG(10028, this, Test::callback1, "测试1");
		REGISTER_MSG(5077, this, Test::callback2, "测试2");
		return true;
	}

	int32_t callback1(LogicReq& para)
	{
		cout << "Test::callback1" << endl;
		return 0;
	}
	int32_t callback2(LogicReq& para)
	{

		CMyDateTime now;
		now.now();
		CMyString strSql("(112233,'wwdc','ffff',234,'%s')", now.c_str("%Y-%m-%d %H-%M-%S"));
		protoServerDbLog req;
		SAFE_STRCPY(req.szTable, "tLogModifyNick");
		SAFE_STRCPY(req.szItem, strSql.c_str());

		CONNECTION_MGR.SendMsgToSvr(actorBgsvr, para.record, req);
		cout << "Test::callback2" << endl;
		return 0;
	}

	void Run()
	{

	}

	void Finish()
	{
		LogDebug("Test............");
	}
private:

};

class Test2 : public IMessage
{
public:

	bool Init()
	{
		REGISTER_MSG(5002, this, Test2::callback1, "测试1");
		REGISTER_MSG(5016, this, Test2::callback2, "测试2");

		return true;
	}

	int32_t callback1(LogicReq& para)
	{
		cout << "Test::callback3" << endl;
		return 0;
	}
	int32_t callback2(LogicReq& para)
	{
		cout << "Test::callback4" << endl;
		return 0;
	}

	void Run()
	{

	}

	void Finish()
	{

	}
private:

};

class MsgFactory : public IMsgRegisterFactory
{
public:

	virtual ~MsgFactory()
	{

	}

	void Init()
	{
		IMessage *pTest = new Test();
		m_vecMsg.push_back(pTest);

		IMessage *pTest2 = new Test2();
		m_vecMsg.push_back(pTest2);
	}

	std::vector<IMessage*> GetMessgeList()
	{
		return m_vecMsg;
	}
private:
	//Test m_test;
	//Test2 m_test2;
	std::vector<IMessage*> m_vecMsg;
};



int main(int argc, char **argv)
{
	Comm::OpenLog("test_frame", 4, "/data/fish/log");
	MsgFactory msgFactory;

	if (QW_FRAME->Init(argc, argv, &msgFactory))
	{
		QW_FRAME->Run();
	}

	return 0;
}














