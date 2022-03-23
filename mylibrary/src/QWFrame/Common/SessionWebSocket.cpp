/*
* =====================================================================================
*
*       Filename:  Session.cpp
*
*    Description:  for connection session
*
*        Version:  1.0
*        Created:  2015年09月08日 14时23分32秒
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
*   Organization:  www.skyunion.net(IGG)
*
* =====================================================================================
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "SessionWebSocket.h"
#include "NetWork/NetFunc.h"
#include "../Protocol/protoSystem.h"
static const int32_t s_nHeartBeat = 5 * 1000;		// 心跳超时设置
static const int32_t s_nMaxHeartCount = 6;			// 心跳检查最大次数
static const int32_t s_nZombieTimeout = 1000;		// zombie connect
static const int32_t s_nAuthTimeout = 1000;		// authorize timeout
static const int32_t s_nCloseTimeout = 500;		// 延迟关闭时间

CSessionWebSocket::CSessionWebSocket(enum Actor actor, const CMyString &actorName
	, ISessionMgr *pSessionMgr, int32_t fd, mm::Reactor &reactor
	, uint32_t ip, const CMyString &strIp)
	: CSession(actor, actorName, pSessionMgr, fd, reactor, ip, strIp)
{
	m_bReceived = false;
}

CSessionWebSocket::~CSessionWebSocket()
{
	if (m_timerCloseWait != 0)
	{
		this->cancelTimer(m_timerCloseWait);
	}
	LogDebug2("HTTP",0, "session [%u] closed", m_nSessionId);
}

void CSessionWebSocket::SendMsg(void *pBuf, size_t cbBuf)
{
	CGuard guard(m_pLock, false);
	if (this->IsWebSocketConnected())
	{
		//LogDebug("m_websocketConnect TRUE");
		this->WebSocketAddHeader(cbBuf);
	}

	m_output.Write(pBuf, cbBuf);

	this->setEvents(EPOLLOUT);
}

void CSessionWebSocket::OnClose()
{
	CGuard guard(m_pLock, false);
	m_timerCloseWait = this->setTimer(s_nCloseTimeout);
	return;
}

bool CSessionWebSocket::handleRead(void)
{
	if (m_eState == stateInit)
	{
		m_eState = stateActive;
		this->cancelTimer(m_timerZombie);
		m_timerZombie = 0;
		m_timerAuth = this->setTimer(s_nAuthTimeout);
	}

	char buf[8192] = "";
	int32_t max_recv_count(10);
	//接收全部数据
	do 
	{
		ssize_t n = recv(m_fd, buf, sizeof(buf), 0);
		if (0 == n)
		{
			//连接已关闭
			m_eReason = closePassive;
			return false;
		}
		else if (-1 == n)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				break;
			}
			LogErr("ERR:[%s#%u ch#%u]call recv(), socket occur error [%d] %s",
				m_actorName.c_str(), m_svrId, m_channelId, errno, strerror(errno));
			m_eReason = closeNetFault;
			return false;
		}

		size_t bufLen((size_t)n);
		++m_pkgInput;
		m_tLastRecv = Now();
		m_nHeartBeatCount = 0;
		// 发送数据，并统计
		m_input.Write((const void*)buf, bufLen);
		//IPMON.OnNetReceive(m_ip, n);
		LogTrace("[%s#%d ch#%u]recv %ld bytes, input pkgIndex %d",
			m_actorName.c_str(), m_svrId, m_channelId, n, m_pkgInput);

		--max_recv_count;
	} while (max_recv_count > 0);

	if (this->process_data_ex() < 0)
	{
		return false;
	}

	return true;
}

bool CSessionWebSocket::handleTimeout(unsigned int timerId)
{
	// TODO: 必须下发客户端的异常状态
	// 另外，不处理心跳，
	if (timerId == m_timerZombie)
	{
		LogWarn("client #%u is zombie", m_nSessionId);
		//m_eReason = closeZombie;
		//result = -1;
		m_timerZombie = 0;
		this->HttpResponse(404, "FAIL", "connection is zombie", false);
		return false;
	}
	else if (timerId == m_timerAuth)
	{
		m_eReason = closeAuthTimeout;
		LogWarn("Warn: connection authorize timeout");
		m_timerAuth = 0;
		this->HttpResponse(404, "FAIL", "connection authorize timeout", false);
		return false;
	}
	else if (timerId == m_timerCloseWait)
	{
		LogImpt("[CLOSE_WAIT]ws session[%u] catch socket %d close event.", m_nSessionId, m_fd);
		m_timerCloseWait = 0;
		return false;
	}
	else
	{
		LogErr("unsupport timerId %u", timerId);
	}

	return false;
}

int32_t CSessionWebSocket::process_data_ex(void)
{
	int32_t result = 0;

	if (this->IsWebSocketConnected())
	{
		this->WebSocketRemoveHeader();
		result = this->process_data();
	}
	else
	{
		// 握手
		this->WebSocketAck();
	}

	return result;
}

int32_t CSessionWebSocket::process_data(void)
{
	static uint32_t		s_nMaxPkgSize = 16 * 1024;	// 16KB

	struct BASE_MSG_INFO head;
	int32_t ret(0);

	do 
	{
		if (m_buffer.size() < sizeof(struct BASE_MSG_INFO))
		{
			// 包文不完整，继续等待
			LogWarn("m_buffer.size [%lu] less than head len[%lu]", m_buffer.size(), sizeof(struct BASE_MSG_INFO));
			break;
		}

		// 预取出数据，用于判断是否完整包
		m_buffer.SetPeekMode(true);
		m_buffer.Read(&head, sizeof(head));
		m_buffer.SetPeekMode(false);
		head.Cast();

		if (head.usLen > s_nMaxPkgSize)
		{
			// 协议乱了
			uint32_t buffSize = m_buffer.size();
			ret = -1;
			LogFatal("[%s#%u, ch#%u]protocol currpted, protoId [%u], protoLen [%u], origin buffer size %u", 
				m_actorName.c_str(), m_svrId, m_channelId, head.usMsgId, head.usLen, buffSize, m_buffer.size());
			
			break;
		}
		if (m_buffer.size() < head.usLen)
		{
			// 包文不完整，继续等待
			LogWarn("packge not intact, m_buffer.size [%lu], packge len[%lu]", m_buffer.size(), head.usLen);
			break;
		}

		// 包文完整，逐个取出处理
		m_buffer.skip(sizeof(struct BASE_MSG_INFO));

		size_t cbBuf = head.usLen - sizeof(struct BASE_MSG_INFO);
		CTempBuffer buff(cbBuf);
		m_buffer.Read((void*)buff.data(), cbBuf);
		m_pSessionMgr->OnRcvWebSocketMsg(this, head.usMsgId, buff.data(), cbBuf);
		ret = 0;
		if (ret < 0)
		{
			break;
		}
	} while (!m_buffer.empty());

	if (ret < 0)
	{
		// 出错了，不再接收新消息
		this->setEvents(EPOLLOUT);
		return -1;
	}

	{
		CGuard guard(m_pLock, false);
		if (m_output.empty())
		{
			this->setEvents(EPOLLIN);
		}
		else
		{
			this->setEvents(EPOLLIN | EPOLLOUT);
		}
	}


	return 0;
}


void CSessionWebSocket::HttpResponse(uint32_t code, const char *status, const char *data, bool bSuccess)
{
	m_httpResp.setStatusCode(code);
	m_httpResp.setStatus(status);
	m_httpResp.setHeader("Data", getGMTDate(time(NULL)));
	m_httpResp.setHeader("Content-Type", "text/html");
	m_httpResp.setHeader("Server", "Apache/1.3.12(Unix)");

	CByteStream &buff = m_httpResp.getBuff();
	if (data == NULL)
	{
		m_httpResp.setContentLength(buff.size());
	}
	else
	{
		CMyString result(data);
		m_httpResp.setContentLength(result.length());
		buff.clear();
		buff.Write((void*)result.c_str(), result.size());
	}

	CByteStream outBuff;
	m_httpResp.pack(outBuff, true);

	this->SendMsg(outBuff);

	// 注册关闭事件
	if (!bSuccess)
	{
		m_timerCloseWait = setTimer(s_nCloseTimeout);
	}
}

void CSessionWebSocket::SendMsg(CByteStream &outBuff)
{
	CGuard guard(m_pLock, false);

	if (this->IsWebSocketConnected())
	{
		LogDebug("m_websocketConnect TRUE");
		this->WebSocketAddHeader(outBuff.size());
	}

	LogDebug("m_websocketConnect len: %ld", outBuff.size());

	m_output.Write(outBuff);
	this->setEvents(EPOLLOUT);
}

void CSessionWebSocket::WebSocketAck()
{
	//处理握手
	size_t nLen = m_input.size();

	CTempBuffer tempBuff(nLen);
	m_input.SetPeekMode(true);
	m_input.Read((void*)tempBuff.data(), nLen);
	m_input.SetPeekMode(false);

	LogDebug("WebSocketAck, data: %s", tempBuff.data());


	int32_t ret = m_httpReq.read((void*)tempBuff.data(), nLen);

	m_httpResp.setVersion(m_httpReq.getVersion());
	if (ret != http::CHttpData::stateOK) //&& ret != CHttpData::statePending
	{
		CMyString result("NOT FOUND");
		this->HttpResponse(404, "FAIL", result.c_str(), false);
		LogErr("WebSocketAck, invalid request");
		return;
	}

	CMyString Upgrade = m_httpReq.getHeader("Upgrade");
	string SecWebSocketKey = m_httpReq.getHeader("Sec-WebSocket-Key");
	Upgrade.MakeLower();
	if (Upgrade == "websocket"  && SecWebSocketKey != "")
	{
		//包头正确		
		string Accept = ShaWithBase64(SecWebSocketKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

		m_httpResp.setHeader("Access-Control-Allow-Credentials", "true");
		m_httpResp.setHeader("Access-Control-Allow-Headers", "content-type");
		m_httpResp.setHeader("Sec-WebSocket-Accept", Accept);
		m_httpResp.setHeader("Upgrade", "WebSocket");
		m_httpResp.setHeader("Connection", "Upgrade");


		LogDebug("WebSocketAck success, Sec-WebSocket-Accept: %s", Accept.c_str());

		this->HttpResponse(101, "Switching Protocol", NULL, true);
		m_eState = stateAuthorize;
		this->cancelTimer(m_timerAuth);
		m_timerAuth = 0;
		m_input.skip(nLen);
		return;
	}
	else
	{
		CMyString result("NOT FOUND");
		this->HttpResponse(404, "FAIL", result.c_str(), true);

		LogErr("WebSocketAck, header error, Upgrade: %s, SecWebSocketKey: %s", Upgrade.c_str(), SecWebSocketKey.c_str());
		return;
	}

}


struct WebSocketHeader
{
	unsigned char opcode : 4;
	unsigned char rsv3 : 1;
	unsigned char rsv2 : 1;
	unsigned char rsv1 : 1;
	unsigned char fin : 1;

	unsigned char packetLen : 7;
	unsigned char mask : 1;
};


void CSessionWebSocket::WebSocketRemoveHeader()
{
	WebSocketHeader header;
	size_t nLen = m_input.size();

	LogDebug("WebSocketRemoveHeader buf size: %ld", nLen);

	unsigned char* pData1 = new unsigned char[nLen + 1];
	memset(pData1, 0, nLen + 1);
	m_input.SetPeekMode(true);
	m_input.Read((void*)pData1, nLen);
	m_input.SetPeekMode(false);
	int index = 0;
	uint64_t dataLen = 0;

	//unsigned char fin;
	//unsigned char opcode;
	//unsigned char mask;
	//unsigned char packetLen;

	unsigned char* pData = pData1;

	while (nLen > sizeof(header))
	{
		memcpy(&header, pData + index, sizeof(header));

		LogDebug("WebSocketRemoveHeader fin: %d, mask: %d, packetLen: %d, opcode: %d, %u, %u",
			header.fin, header.mask, header.packetLen, header.opcode, pData[0], pData[1]);

		index += sizeof(header);

		if (header.packetLen <= 125)
		{
			dataLen = header.packetLen;
		}
		if (header.packetLen == 126)
		{
			uint16_t len = 0;
			memcpy(&len, pData + index, sizeof(len));
			dataLen = CByteOrder::Cast(len);
			index += 2;
		}
		else if (header.packetLen == 127)
		{
			uint64_t len = 0;
			memcpy(&len, pData + index, sizeof(len));
			dataLen = CByteOrder::Cast(len);
			index += 8;
		}

		if (header.mask == 1)
		{
			index += 4;
		}

		LogDebug("WebSocketRemoveHeader fin: %d, mask: %d, packetLen: %d, index: %d dataLen %ld", header.fin, header.mask, header.packetLen, index, dataLen);

		//读取贞数据
		if (nLen >= index + dataLen)
		{
			unsigned char *pBodyData = pData + index;

			//根据掩码解析数据
			if (header.mask == 1)
			{
				unsigned char* pMask = pBodyData - 4;
				for (uint64_t i = 0; i < dataLen; i++)
				{
					pBodyData[i] = pBodyData[i] ^ pMask[i % 4];
				}
			}

			m_buffer.Write((const void*)pBodyData, dataLen);

			//LogDebug("WebSocketRemoveHeader data: %s", (char *)pBodyData);

			//this->SendMsg((char *)pBodyData, dataLen);


			m_input.skip(dataLen + index); //移除数据

			nLen -= (dataLen + index);
			pData += (dataLen + index);
			index = 0;

		}
		else
		{
			break;
		}

	}
	delete[] pData1;
}

void CSessionWebSocket::WebSocketAddHeader(uint64_t length)
{
	WebSocketHeader header;
	memset(&header, 0, sizeof(0));
	header.fin = 1;
	header.opcode = 2;
	header.mask = 0;

	int headerLen = sizeof (header);

	if (length <= 125)
	{
		header.packetLen = length;
	}
	else if (length <= 0xFFFF)
	{
		header.packetLen = 126;
		headerLen += 2;
	}
	else
	{
		header.packetLen = 127;
		headerLen += 8;
	}

	char* pHeader = new char[headerLen];
	memcpy(pHeader, &header, sizeof(header));
	if (header.packetLen == 126)
	{
		uint16_t len16 = length;
		len16 = CByteOrder::Cast(len16);

		memcpy(pHeader + sizeof(header), &len16, sizeof(len16));
	}
	else if (header.packetLen == 127)
	{
		uint64_t len64 = length;
		len64 = CByteOrder::Cast(len64);
		memcpy(pHeader + sizeof(header), &len64, sizeof(len64));
	}

	m_output.Write(pHeader, headerLen);

	delete[] pHeader;
}

string CSessionWebSocket::ShaWithBase64(const string& data)
{
	SHA_CTX ctx;
	unsigned char outmd[20]; //注意这里的字符个数为20
	memset(outmd, 0, sizeof(outmd));
	if (!SHA1_Init(&ctx))
	{
		return "";
	}
	SHA1_Update(&ctx, data.c_str(), data.size());
	SHA1_Final(outmd, &ctx);
	OPENSSL_cleanse(&ctx, sizeof(ctx));

	string res = Base64Encode(outmd, sizeof(outmd));
	return res;
}

const string Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
string CSessionWebSocket::Base64Encode(const unsigned char * cRawData, unsigned int iLength)
{
	string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (iLength--) {
		char_array_3[i++] = *(cRawData++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				ret += Base64Chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += Base64Chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}


bool CSessionWebSocket::IsWebSocketConnected()
{
	return (m_eState == stateAuthorize);
}

