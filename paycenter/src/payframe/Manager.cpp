/*
 * =====================================================================================
 *
 *
 *    Description:  client manager
 *
 *        Version:  1.0
 *        Created:  09/08/2019 11:47:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Manager.h"
#include "Client.h"
#include "Common.h"
#include "MyUtility.h"

CManager::CManager()
    : mm::EventHandler()
    , m_fd(-1)
    , m_reactor()
    , m_workerId(0)
    , m_startId(0)
    , m_maxId(0)  
    , m_nextId(0)
    , m_timerId(0)
{
    m_cgiPay = "api/pay";
    //m_cgiReload = "api/reload";
    m_cgiGetChannel = "api/getChannel";
    m_cgiGetWealthStat = "api/getWealthStat";
    m_cgiSign = "api/sign";
    m_cgiVerify = "api/verify";

    //m_ipReload = "127.0.0.1";
    m_cgiReload = CONFIG.getReloadCfg().cgi;
}

CManager::~CManager()
{
    // CLIENT MAP 清理
    //
    if (m_timerId)
    {
        this->cancelTimer(m_timerId);
    }
    if (m_procReqId)
    {
        this->cancelTimer(m_procReqId);
    }
}

bool CManager::Init(int32_t fd, uint32_t workerId, uint32_t clientId, uint32_t maxId)
{
    m_fd = fd;
    m_workerId = workerId;
    
    if (this->load_config()< 0)
    {
        return false;
    }

    int32_t val = m_payMgr.Init(m_workerId);
    if (val < 0)
    {
        return false;
    }

	//设置为非阻塞
	::fcntl(m_fd, F_SETFL, O_NONBLOCK);

	//初始化Reactor
	this->setReactor(&m_reactor);
	m_reactor.addErrorHandler(this);

    bool ret = m_plugin.Init();
    if (!ret)
    {
        return false;
    }

    m_startId = clientId;
    m_maxId = maxId;
    m_nextId = m_startId;
    m_timerId = this->setTimer(50);

    int32_t loop_time = CONFIG.getReloadCfg().loop_time *1000;
    m_procReqId = this->setTimer(loop_time);
    return true;
}

void CManager::Run(void)
{
    this->setEvents(EPOLLIN);
    try
    {
        m_reactor.run();
    }
    catch(...)
    {
        LogFatal("worker #%u manager failure, errno:%d, error:%s",
                m_workerId, errno, strerror(errno));
    }
    LogImpt("worker #%u manager stopped.", m_workerId);
}

uint32_t CManager::getNextClientId(void)
{
    uint32_t nextId = m_nextId++;
    if (m_nextId >= m_maxId)
    {
        m_nextId = m_startId;
    }
    return nextId;
}

bool CManager::handleRead(void)
{
    //获得连接的fd
    char data[256] = "";
    struct iovec iov;
    iov.iov_base = &data[0];
    iov.iov_len = sizeof(data);

    char buf[1024] ="";
    struct msghdr msg = {0};
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    ssize_t s = ::recvmsg(m_fd, &msg, msg.msg_flags);
    if (s<0)
    {
        LogErr("worker #%u recv accept data failure. errno:%d, error:%s",
                m_workerId, errno, strerror(errno));
        return false;
    }
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    if (0==cmsg)
    {
        LogErr("worker #%u recv invalid cmsg.", m_workerId);
        return false;
    }
    int32_t n=(cmsg->cmsg_len - ((char*)CMSG_DATA(cmsg)-(char*)cmsg) );
    if (n % sizeof(int32_t) != 0)
    {
        LogErr("worker #%u recv invalid cmsg len.", m_workerId);
        return false;
    }

    // allocate CClient object
    n /= sizeof(int32_t);
    int32_t *fds = (int32_t*)CMSG_DATA(cmsg);
    for(int32_t i=0; i<n; ++i)
    {
        int32_t fd = fds[i];
        ::fcntl(fd, F_SETFL, O_NONBLOCK);
        set_sock_linger(fd, false, 0);

        struct sockaddr_in remote_addr;
        socklen_t len = sizeof(remote_addr);
        //int32_t ret = ::getpeername(fd, reinterpret_cast<struct sockaddr*>(&remote_addr), &len);
        ::getpeername(fd, reinterpret_cast<struct sockaddr*>(&remote_addr), &len);
        CMyString strIp = ::inet_ntoa(remote_addr.sin_addr);
        uint32_t ip = str2ip(strIp.c_str());

        uint32_t clientId = this->getNextClientId();
        LogDebug("worker #%u accept client #%u, sck:%d, ip:%s",
                m_workerId, clientId, fd, strIp.c_str());

        CClient *pClient = new CClient(m_reactor, clientId, fd, ip, this, m_workerId);
        pClient->setEvents(EPOLLIN);
        m_mapClient.insert(CLIENT_MAP::value_type(clientId, pClient));
    }

    return true;
}

bool CManager::handleWrite(void)
{
    LogFatal("worker #%u write something.", m_workerId);
    return false;
}

bool CManager::handleTimeout(uint32_t timerId)
{
    if (timerId == m_timerId)
    {
        uint64_t begin = Now(true);
#if 0
        bool ret = m_plugin.Maintance();
        uint64_t now = Now(true);
        if (!ret)
        {
            LogFatal("[%s:%s:%d]handleTimeout failure, stop time handler.",
                    __FILE__, __func__, __LINE__);
            return false;
        }
#else
        // 所有插件定时处理，忽略错误
        m_plugin.Maintance();

        // 计费点定时管理
        BPDATA_LIST.Maintance();

        uint64_t now = Now(true);
#endif

        uint32_t cost = now-begin;
        int32_t nextTime=100;
        if (cost>5000)
        {
            LogWarn("worker #%u execute maintance cost %d ms", 
                m_workerId, (int32_t)cost/1000);
            nextTime = 100-cost;
            if (nextTime <=0) nextTime=5;
        }
        m_timerId = this->setTimer(nextTime);
    }
    else if (timerId == m_procReqId)
    {
        // 检查config.xml 是否有变动
        if (CONFIG.Update()==1)
        {
            LogWarn("reload config succeed.");
        }

        // 检查是否有其他进程转发过来的操作请求
        this->DealSiblingMsg();

        int32_t loop_time = CONFIG.getReloadCfg().loop_time * 1000;
        m_procReqId = this->setTimer(loop_time);
    }

    // LogFatal("worker #%u timeout.", m_workerId);
    return true;
}

bool CManager::handleClose(void)
{
    LogImpt("worker #%u shutdown.", m_workerId);
    return true;
}

bool CManager::handleError(int32_t events, mm::EventHandler *pHandler)
{
    CClient *pClient = dynamic_cast<CClient*>(pHandler);
    if (NULL==pClient)
    {
		//非CClientHandler，不予处理
        LogWarn("handle error, events %d, object 0x%p", events, pHandler);
        return true;
    }

    uint32_t clientId(0);
    int32_t fd(-1);
    std::string strIp;
    uint32_t ownerId = pClient->getWorkerId();
    if (ownerId != m_workerId)
    {
		// 不是本manager拥有的client,传递到下一个管理器来处理
        LogWarn("client workerId #%u, this workerId #%u", ownerId, m_workerId);
        return true;
    }

	bool error = (events & (EPOLLHUP | EPOLLERR));
	if (!error && error != 0)
	{
		// 不是想要处理的错误
		LogErr("worker #%u recvie client #%u event [%u]", m_workerId, clientId, events);
		this->closeClient(pClient);
		return false;
	}

	int32_t errCode(0);
	socklen_t len(sizeof(int32_t));
	int32_t err = ::getsockopt(fd, SOL_SOCKET, SO_ERROR, &errCode, &len);
	if (0 == err)
	{
		//TODO:处理特定错误
	}
	else
	{
		//TODO:未捕获错误
	}
	this->closeClient(pClient);
    return false;
}

bool CManager::confirmDispose(void)
{
	// NOTE: 由于是本类拥有 mm::Reactor, 出错时不释放本类
    return false;
}

void CManager::closeClient(CClient *pClient)
{
    if (NULL == pClient)
    {
        return;
    }
    uint32_t clientId = pClient->getClientId();
    uint32_t workerId = pClient->getWorkerId();
    const char* pszIp = pClient->getIpStr();
    LogDebug("client #%u, workerId %u, ip %s closed.",
            clientId, workerId, pszIp);
    m_mapClient.erase(clientId);
}

void CManager::OnPayFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp)
{
    Json::Value resp;
    resp["code"] = 1;
    resp["message"] = CMyString("error :%d, message %s", code, msg);
    Json::Value data;
    data["tradeNo"] = "N/A";
    data["url"] = "N/A";
    resp["data"] = data;

    CJsonWrapper writer(resp);
    CMyString strResp = writer.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

void CManager::OnCgiFailure(int32_t code, const char *msg, char *szResp, uint32_t &cbResp)
{
    Json::Value resp;
    resp["code"] = code;
    resp["message"] = msg;
    CJsonWrapper writer(resp);
    CMyString strResp = writer.toString();
    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
}

int32_t CManager::doGetPayFunc(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, PCGIFUNC &cgiFunc, CMyString &strCgiNew)
{
    int32_t val = m_payMgr.QueryCgi(req, szResp, cbResp, strCgiNew);
    if (val < 0)
    {
        return val;
    }
    
    // 获得cgi的执行函数
    cgiFunc = m_plugin.getCgiFunc(strCgiNew);
    if (NULL == cgiFunc)
    {
        CMyString strErr("not function match for cgi[%s]", strCgiNew.c_str());
        LogFatal(strErr.c_str());
        this->OnPayFailure(-1009, strErr.c_str(), szResp, cbResp);
    	return -1009;
    }
    return 0;
}

int32_t CManager::doGetSignFunc(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, PCGIFUNC &cgiFunc, CMyString &strCgiNew)
{
    int32_t val = m_payMgr.QuerySignCgi(req, szResp, cbResp, strCgiNew);
    if (val < 0)
    {
        return val;
    }
    
    // 获得cgi的执行函数
    cgiFunc = m_plugin.getCgiFunc(strCgiNew);
    if (NULL == cgiFunc)
    {
        CMyString strErr("not function match for cgi[%s]", strCgiNew.c_str());
        LogFatal(strErr.c_str());
        this->OnCgiFailure(-1009, strErr.c_str(), szResp, cbResp);
    	return -1009;
    }
    return 0;
}
    
int32_t CManager::doGetVerifyFunc(http::CHttpRequest &req, char *szResp, uint32_t &cbResp, PCGIFUNC &cgiFunc, CMyString &strCgiNew)
{
    int32_t val = m_payMgr.QueryVerifyCgi(req, szResp, cbResp, strCgiNew);
    if (val < 0)
    {
        return val;
    }
    
    // 获得cgi的执行函数
    cgiFunc = m_plugin.getCgiFunc(strCgiNew);
    if (NULL == cgiFunc)
    {
        CMyString strErr("not function match for cgi[%s]", strCgiNew.c_str());
        LogFatal(strErr.c_str());
        this->OnCgiFailure(-1009, strErr.c_str(), szResp, cbResp);
    	return -1009;
    }
    return 0;
}

void CManager::doReload(http::CHttpRequest& req, char *szResp, uint32_t &cbResp, IClient* pClient)
{
    // 重新加载配置信息，需要校验IP
    CMyString strNote;
    int32_t ret = this->ReloadConfig(req, pClient, strNote);
    //ext_header["Content-Type"] = "application/json";
    if (ret < 0)
    {
        CMyString strError("{\"code\":%d, \"message\":\"%s\"}", ret, strNote.c_str());
        safe_strcpy(szResp, cbResp, strError.c_str());
    }
    else
    {
        ReloadCfg &rc = CONFIG.getReloadCfg();
        CMyString temp("{\"code\":0, \"message\":\"reload config SUCCEED\",\"applyTime\":\"%dseconds\"}", rc.loop_time);
        safe_strcpy(szResp, cbResp, temp.c_str());
        // safe_strcpy(szResp, cbResp, "{\"code\":0, \"message\":\"reload config SUCCEED\"}");
    }
    cbResp = strlen(szResp);
    LogDebug("reload config result %d, resp [%s]", ret, szResp);
}

int32_t CManager::ExecuteCGI(const CMyString& strCgi, 
            http::CHttpRequest& req, char *szResp, uint32_t &cbResp,
            STRING_MAP& ext_header, 
            IClient* pClient)
{
    CMyString strCgiNew(strCgi);
    PCGIFUNC cgiFunc;

    CMyString strErr;
    ext_header["Content-Type"] = "application/json";
    bool bPayCenterReq(false);
    if (strCgi == m_cgiPay)
    {
        // 调用充值插件, 重定向获得 cgiFunc 和 strCgiNew
        bPayCenterReq = true;
        int32_t ret = this->doGetPayFunc(req, szResp, cbResp, cgiFunc, strCgiNew);
        if (ret != 0)
        {
            return ret;
        }
        LogDebug("cgi: [%s] => [%s]", strCgi.c_str(), strCgiNew.c_str());
        // 后续执行获得cgiFunc
    }
    else if (strCgi == m_cgiGetChannel)
    {
        // 获得充值渠道
        int32_t ret = m_payMgr.GetChannel(req, szResp, cbResp, pClient);
        return ret;
    }
    else if (strCgi == m_cgiGetWealthStat)
    {
        // 获得充值渠道
        int32_t ret = m_payMgr.GetWealthStat(req, szResp, cbResp, pClient);
        return ret;
    }
    else if (strCgi== m_cgiReload)
    {
        // 重载参数
        this->doReload(req, szResp, cbResp, pClient);
        return 0;
    }
    else if (strCgi == m_cgiSign)
    {
        // 调用充值插件, 重定向获得 cgiFunc 和 strCgiNew
        int32_t ret = this->doGetSignFunc(req, szResp, cbResp, cgiFunc, strCgiNew);
        if (ret != 0)
        {
            return ret;
        }
        LogDebug("cgi: [%s] => [%s]", strCgi.c_str(), strCgiNew.c_str());
        // 后续执行获得cgiFunc
    }
    else if (strCgi == m_cgiVerify)
    {
        // 调用充值插件, 重定向获得 cgiFunc 和 strCgiNew
        int32_t ret = this->doGetVerifyFunc(req, szResp, cbResp, cgiFunc, strCgiNew);
        if (ret != 0)
        {
            return ret;
        }
        LogDebug("cgi: [%s] => [%s]", strCgi.c_str(), strCgiNew.c_str());
        // 后续执行获得cgiFunc
    }
    else
    {
         // 获得cgi的执行函数
         cgiFunc = m_plugin.getCgiFunc(strCgiNew);
         if (NULL == cgiFunc)
         {
             strErr.Format("not function match for cgi [%s]", strCgiNew.c_str());
             LogFatal(strErr.c_str());
            this->OnCgiFailure(-1009, strErr.c_str(), szResp, cbResp);
         	return -1009;
         }
    }

    // 执行cgi函数
	int32_t ret(0);
	try
	{
		ret = cgiFunc(pClient, req, szResp, cbResp, ext_header);
	}
    catch(CException &e)
    {
        LogFatal("execute cgi [%s] error, msg:%s", strCgiNew.c_str(), e.what());
        strErr.Format("execute cgi[%s] failure :%s", strCgiNew.c_str(), e.what());
        if (bPayCenterReq)
        {
            this->OnPayFailure(-1011, strErr.c_str(), szResp, cbResp);
        }
        else
        {
            this->OnCgiFailure(-1011, strErr.c_str(), szResp, cbResp);
        }
		return -1010;
    }
	catch(...)
	{
        LogFatal("execute cgi [%s] error", strCgiNew.c_str());
        strErr.Format("call cgi [%s] corruptered.", strCgiNew.c_str());
        if (bPayCenterReq)
        {
            this->OnPayFailure(-1011, strErr.c_str(), szResp, cbResp);
        }
        else
        {
            this->OnCgiFailure(-1011, strErr.c_str(), szResp, cbResp);
        }
		return -1011;
	}
    LogDebug("call cgi [%s], result %d, resp [%s]", strCgiNew.c_str(), ret, szResp);

	return ret;
}

int32_t CManager::DealSiblingMsg(void)
{
    pid_t pid = getpid();
    ReloadCfg &rc = CONFIG.getReloadCfg();
    CMyRedis redis;
    int32_t ret = open_redis(rc.redis_name, redis);
    if (ret != 0)
    {
        LogErr("can not open redis, name:%s", rc.redis_name.c_str());
        return -1;
    }
    CMyString key(rc.keyformat.c_str(), pid);
    int32_t cnt = redis.LSize(key);
    if (cnt<0)
    {
        LogErr("get data size of key [%s] failure, error:%s", key.c_str(), redis.getLastError());
        return -3;
    }

    CMyString value;
    int32_t result(0);
    do{
        ret = redis.LPop(key, value);
        if (ret == 0)
        {
            break;
        }
        else if (ret < 0)
        {
            LogErr("push data from key [%s] failure, error:%s", key.c_str(), redis.getLastError());
            result = -5;
            break;
        }
        this->ExecuteSiblingReq(value);
        ++result;
    }while(true);

    return result;
}

int32_t CManager::reloadConfig(void)
{
    CMyString strNote;
    // 重新装载config.xml
    int32_t ret = CONFIG.Update(true);
    if (ret < 0)
    {
        strNote.Format("reload config [%s] FAILURE", CONFIG.getConfigPath().c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str());
        return -1;
    }

    ret = this->load_config();
    if (ret < 0)
    {
        strNote.Format("reload cgi_pay / cgi_reload FAILURE");
        return -1;
    }
    LogImpt("reload %s SUCCEED.", CONFIG.getConfigPath().c_str());
    return 0;
}

int32_t CManager::reloadPlugin(void)
{
    CMyString strNote;
    int32_t ret = m_plugin.Update();
    if (ret<0)
    {
        strNote.Format("reload plugins configure FAILURE");
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str());
        return -5;
    }
    LogImpt("reload plugin config SUCCEED");
    return 0;
}

int32_t CManager::reloadPayManager(void)
{
    CMyString strNote;
    int32_t ret = m_payMgr.Init(m_workerId);
    if (ret < 0)
    {
        strNote.Format("reload pay config FIALURE");
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str());
        return -3;
    }
    LogImpt("reload pay config SUCCEED.");
    return 0;
}

void CManager::modifyBpData(int32_t cmd, Json::Value &data)
{
    if (data.isNull())
    {
        return;
    }
    CMyString uin = data["uin"].asString();
    int32_t bpId = data["bpId"].asInt();
    LogWarn("modify billpoint data, uin:%s, bpId:%d", uin.c_str(), bpId);
    switch(cmd)
    {
    case sc_increaseBillpoint:
        BPDATA_LIST.IncBill(uin, bpId);
        break;
    case sc_decreaseBillpoint:
        BPDATA_LIST.DecBill(uin, bpId);
        break;
    default:
        LogFatal("unknown cmd %d when request modify billpoint", cmd);
        break;
    }
}

bool CManager::decodeSiblingRequest(const CMyString &reqstr, Json::Value &data)
{
    if (reqstr.empty())
    {
        return false;
    }

    CJsonWrapper jr;
    if (!jr.Load(reqstr))
    {
        LogErr("sibling request data [%s] invalid", reqstr.c_str());
        return false;
    }
    data = jr.getJson();
    return true;
}

int32_t CManager::ExecuteSiblingReq(const CMyString &req_)
{
    LogWarn("recv sibling request, data[%s]", req_.c_str());
    Json::Value data;
    bool val = this->decodeSiblingRequest(req_, data);
    int32_t cmd;
    if (!val)
    {
        // 全部都装载一下
        cmd = sc_reloadAll;
    }
    else
    {
        cmd = data["command"].asInt();
    }

    switch(cmd)
    {
    case sc_reloadAll:
        this->reloadConfig();
        this->reloadPayManager();
        this->reloadPlugin();
        break;
    case sc_reloadConfig:
        this->reloadConfig();
        break;
    case sc_increaseBillpoint:
    case sc_decreaseBillpoint:
        this->modifyBpData(cmd, data["data"]);
        break;
    default:
        break;
    }

    return 0;
}

#if 0
int32_t CManager::Broadcast2Sibling(int32_t cmd, Json::Value &data)
{
    // 获得父进程ID，父进程下的子进程ID
    pid_t ppid = getppid();
    pid_t pid = getpid();
    LogDebug("RELOAD>process id:%d, parent process id:%d", pid, ppid);
    CMyString proc_name;
    if (!getProcessNameByPid(pid, proc_name))
    {
        LogErr("get process_name by pid failure, pid:%d", pid);
        return -1;
    }
    LogDebug("RELOAD>process name: [%s]", proc_name.c_str());
    std::vector<int32_t> sibling_pid;
    int32_t cnt = getSiblingProcessPid(proc_name, sibling_pid);
    if (cnt<0)
    {
        LogErr("get sibling process failure, pid:%d, name:%s", pid, proc_name.c_str());
        return -3;
    }
    else if(cnt==0)
    {
        LogErr("not any sibling process, pid:%d, name:%s", pid, proc_name.c_str());
        return 0;
    }

    ReloadCfg &rc = CONFIG.getReloadCfg();
    CMyRedis redis;
    int32_t ret = open_redis(rc.redis_name, redis);
    if (ret != 0)
    {
        LogErr("can not open redis, name:%s", rc.redis_name.c_str());
        return -5;
    }

    cnt = 0;
    Json::Value req_data;
    req_data["timestamp"] = (Json::Int64)time(NULL);
    req_data["caller"] = pid;
    req_data["command"] = cmd;
    req_data["data"] = data;
    Json::FastWriter wr;
    CMyString strData = wr.write(req_data);
    for(std::vector<int32_t>::iterator it(sibling_pid.begin());
            it != sibling_pid.end(); ++it) 
    {
        // 逐个进程发送消息,采用queue的格式
        pid_t pid1 = *it;
        CMyString strKey(rc.keyformat.c_str(), pid1); 
        ret = redis.LPush(strKey, strData);
        if (ret==1)
        {
            LogMsg("RELOAD>send request to process %d succeed.", pid1);
            ++cnt;
        }
        else
        {
            LogErr("RELOAD>send request to process %d failure.", pid1);
        }
    }

    return cnt;
}
#endif

int32_t CManager::ReloadConfig(http::CHttpRequest &req, IClient* pClient, CMyString &strNote)
{
    // 检查IP是否合法
    CMyString strIp = getClientIp(pClient, req);
#if 0
    if (m_ipReload != "0.0.0.0" && strIp != m_ipReload)
#else
    ReloadCfg &rc = CONFIG.getReloadCfg();
    if (!rc.ips.Find(strIp))
#endif
    {
        strNote.Format("invalid host ip [%s]", strIp.c_str());
        LogErr("[%s:%s:%d]%s", __FILE__, __func__, __LINE__, strNote.c_str());
        return -7;
    }

    // 获得父进程ID，父进程下的子进程ID
    Json::Value req_data;
    Broadcast2Sibling(sc_reloadAll, req_data);
    int32_t ret = this->ExecuteSiblingReq("");

    return ret;
}

int32_t CManager::load_config(void)
{
    const char *sect_pay = "main/paycenter/uri_pay";
    //const char *sect_reload = "main/paycenter/uri_reload";
    const char *sect_getchannel = "main/paycenter/uri_getchannel";
    const char *sect_sign = "main/paycenter/uri_sign";
    const char *sect_verify = "main/paycenter/uri_verify";
    CXmlFile &xml = CONFIG.getXml();
    try
    {
        m_cgiPay = xml.GetString(sect_pay, "uri");
        //m_cgiReload = xml.GetString(sect_reload, "uri");
        //m_ipReload = xml.GetString(sect_reload, "validIp");
        m_cgiReload = CONFIG.getReloadCfg().cgi;
        m_cgiGetChannel = xml.GetString(sect_getchannel, "uri");
        m_cgiSign = xml.GetString(sect_sign, "uri");
        m_cgiVerify = xml.GetString(sect_verify, "uri");
    }
    catch(CException &e)
    {
        LogErr("[%s:%s:%d]load predefine cgi failure, error %s", __FILE__, __func__, __LINE__, e.what());
        return -1;
    }
    catch(...)
    {
        LogErr("[%s:%s:%d]load predefine cgi failure", __FILE__, __func__, __LINE__);
        return -3;
    }

    LogDebug("DBG>pay=[%s]", m_cgiPay.c_str());
    LogDebug("DBG>sign=[%s]", m_cgiSign.c_str());
    LogDebug("DBG>verify=[%s]", m_cgiVerify.c_str());
    // LogDebug("DBG>reload=[%s], valid[%s]", m_cgiReload.c_str(), m_ipReload.c_str());
    return 0;
}


