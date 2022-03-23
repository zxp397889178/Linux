/*
* =====================================================================================
*
*       Filename:  UserInfoMgr.h
*
*    Description:  �û���Ϣ����
*
*        Version:  1.0
*        Created:  2018��2��2��
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  wuwenchi
*   Organization:  quwan
*
* =====================================================================================
*/

#ifndef _QW_FRAME_USERINFOMGR_H_
#define _QW_FRAME_USERINFOMGR_H_

#include <stdint.h>
#include "Protocol/protoUserData.h"
#include "QWFrame.h"
#include "MyNet.h"
class UserInfoMgr
{
public:
	UserInfoMgr();
	~UserInfoMgr();

	/*
	*��ѯ�û�������Ϣ
	*roomId��Ϊ0������Ҷ�Ӧ�����·���Ϣ
	*/
	int32_t QueryUserInfo(uint32_t uin, uint32_t roomId, struct protoUserInfoResponse &resp);

	/*
	*�뿪����
	*����ֵ 0-�ɹ� ����-ʧ��
	*/
	int32_t UserLeaveRoom(struct protoSvrUserLeaveRoomRequest &req);

	/*
	*���ӻ��ң�����
	*����ֵ 0-�ɹ� ����-ʧ��
	*/
	int32_t UserAddCurrency(struct protoSvrAddCurrencyRequest &req);

	/*
	*���ӻ��ң�����
	*logType-��¼logcurreny������
	*coin-���
	*ticket-��ȯ
	*money-��ʯ
	*vecItem-���߼���
	*����ֵ 0-�ɹ� ����-ʧ��
	*/
	int32_t UserAddCurrency(uint32_t uin, uint32_t logType, uint32_t coin, uint32_t ticket, uint32_t money, vector<CurrencyInfo> &vecItem);

	/*
	*�۳����ң�����
	*����ֵ 0-�ɹ� ����-ʧ��
	*coin-���
	*ticket-��ȯ
	*money-��ʯ
	*vecItem-���߼���
	*decType �ۼ���������
	*/
	int32_t UserDecCurrency(struct protoSvrDecCurrencyRequest &req);

	int32_t UserDecCurrency(uint32_t uin, uint32_t coin, uint32_t ticket, uint32_t money, vector<CurrencyInfo> &vecItem, uint32_t decType = 0);

    int32_t UpdateCurrencyOpLog(int64_t index, int32_t step, uint32_t result);
	/*
	*Э��ת�����û�
	*����ֵ 0-�ɹ� ����-ʧ��
	*
	*/
	template<typename T>
	int32_t SendMsg(uint32_t uin, T &req, bool bBroadcast = false)
	{
		size_t protoLen = (size_t)req.getProtoLen();
		CTempBuffer tmpBuf(protoLen);
		int32_t ret = req.Serialize(tmpBuf.data(), protoLen);
		if (ret < 0)
		{
			LogFatal("can not serialize %s failure, ret=%d", typeid(T).name(), ret);
			return -1;
		}

		struct protoRepostMsgRequest repostReq;
		repostReq.uin = uin;
		repostReq.bBroadcast = bBroadcast;
		repostReq.cbData = protoLen;
		repostReq.pData = calloc(repostReq.cbData, sizeof(char));
		if (repostReq.pData == NULL)
		{
			return -2;
		}
		memcpy(repostReq.pData, tmpBuf.data(), protoLen);

		struct protoRepostMsgResponse resp;
		int32_t result = this->QueryResult(repostReq, resp);
		if (result != 0)
		{
			return result;
		}
		return resp.result;
	}


private:
	template<typename T1, typename T2>
	int32_t QueryResult(T1 &req, T2 &resp, uint32_t timeout = 2000)
	{
		size_t protoLen = (size_t)req.getProtoLen();
		CServerMsgBuffer tmpBuf(protoLen);
		int32_t ret = req.Serialize(tmpBuf.getProtoData(), tmpBuf.getProtoLen());
		if (ret < 0)
		{
			LogFatal("can not serialize %s failure, ret=%d", typeid(T1).name(), ret);
			return -1;
		}
		tmpBuf.BuildRecord(protoLen, T1::IDD, req.uin, 0, 0);

		ConnectionCfg *pConnectionCfg = QW_FRAME->GetFrameConfig().GetConnectionCfg(actorHallSvrSync);
		if (pConnectionCfg == NULL)
		{
			LogFatal("can not found bgsvr connect config.");
			return -2;
		}

		IClientSocket* pConn = OpenClient(NULL, pConnectionCfg->strHost.c_str(), pConnectionCfg->nPort);
		if (NULL == pConn)
		{
			// hallsvr�޷�����,����
			LogErr("UserInfoMgr::QueryResult error , not connect %s:%d", pConnectionCfg->strHost.c_str(), pConnectionCfg->nPort);
			return -3;
		}

		if (!this->Authorize(pConn))
		{
			LogErr("UserInfoMgr::QueryResult authorize failure.");
			return -4;
		}
		 
		//������Ϣ
		protoLen = tmpBuf.getBufferLen();
		pConn->SendMsg(tmpBuf.getBuffer(), protoLen);

		CByteStream buff;
		uint64_t begin = Now();
		do 
		{
			size_t cbData(1024);
			CTempBuffer tmpData(cbData);
			ret = pConn->RecvMsg((void*)tmpData.data(), cbData);
			if (ret>0 && cbData>0)
			{
				buff.Write(tmpData.data(), cbData);
				break;
			}

			if (ret<1)
			{
				break;
			}
			else if (ret == 1)
			{
				usleep(20);
			}

		} while (Now() < begin + timeout);	// ��ʱ
		pConn->Close();
		SAFE_RELEASE(pConn);

		uint32_t cbData = buff.size();
		if (cbData == 0)
		{
			// �������ݳ���
			LogErr("UserInfoMgr::QueryResult can not recv data.");
			return -5;
		}

		CTempBuffer tmpOut(cbData);
		if (!buff.Read(tmpOut.data(), cbData))
		{
			// �������ݳ���
			LogErr("UserInfoMgr::QueryResult recv data error.");
			return -6;
		}
		ret = resp.Deserialize(tmpOut.data() + sizeof(Record), cbData - sizeof(Record));
		if (ret < 0)
		{
			LogFatal("UserInfoMgr::QueryResult deserialize %s failure, ret=%d", typeid(T2).name(), ret);
			return -7;
		}

		return 0;
	}

private:
	bool Authorize(IClientSocket* pConn);
};




#endif	// _QW_FRAME_USERINFOMGR_H_

