#include <stdarg.h>
#include <iomanip>
#include <string>
#include <algorithm>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ShmDataQueue.h"

#include "XmlFile.h"
//#include "rapidxml.hpp"
//#include "rapidxml_utils.hpp"
//#include "rapidxml_print.hpp"

using namespace std;

#define DEBUG

#define MSGTP_FREE_DATA  2

//缓冲区空闲块消息队列
typedef struct
{
	long msgtype;
	int  index;
}FREE_BLK_MSG;

//数据队列摘要信息
typedef struct
{
	int	pkg_count;
	int pkg_size;
	int	first_pkg_index;	//第一个包的第一个块的位置
	int last_pkg_index;		//最后一个包的第一个块的位置
}DATA_ABSTR;

//----------------------------------------------------------------------------
// char * ShmDataQueue::m_pShmBaseAddr=NULL;
// int	ShmDataQueue::m_iDataBlockSize;
// int	ShmDataQueue::m_iDataBlockNum;
map<key_t, DQ_TWOKEY_CFG> ShmDataQueue::sm_dqtcfg;
//-----------------------------------------------------------------------------

//###############################################################################################

//-----------数据队列-------------------------------------------------

ShmDataQueue::ShmDataQueue()
{
	m_pShmBaseAddr = NULL;
}

ShmDataQueue::~ShmDataQueue()
{
}

int ShmDataQueue::checkIfUniqueKey(key_t shm_key,DQ_TWOKEY_CFG twok_cfg)
{
	map<key_t, DQ_TWOKEY_CFG>::iterator it = sm_dqtcfg.find(shm_key);
	if(it == sm_dqtcfg.end())
	{
		for(it=sm_dqtcfg.begin(); it != sm_dqtcfg.end(); it++)
		{	
			if( twok_cfg.free_msgq_key == it->second.free_msgq_key ) 
			{
				cerr<<"程序已创建过配置的消息队列键值["<< twok_cfg.free_msgq_key << "],请配置为其他键值重新创建" << endl;
				break;
			} 
			else if( twok_cfg.lock_key == it->second.lock_key ) 
			{
				cerr<<"程序已创建过配置的信号灯键值["<< twok_cfg.lock_key << "],请配置为其他键值重新创建" << endl;
				break;
			}
		}
		if(it == sm_dqtcfg.end()) 
		{
			return NEWONE;
		} else 
			return ERROR;
	} 
	else 
	{
		if( twok_cfg.free_msgq_key == it->second.free_msgq_key && twok_cfg.lock_key == it->second.lock_key) 
		{
			return AlreadyExist;
		} 
		else 
		{
			cerr<<"程序已创建过配置的共享内存键值["<< shm_key << "],请配置为其他键值重新创建" << endl;
			return ERROR;
		}
	}
	
	return 0;
}

void ShmDataQueue::checkIfCreateKey(key_t shm_key,DQ_TWOKEY_CFG twok_cfg,bool rebuild)
{
	map<key_t, DQ_TWOKEY_CFG>::iterator it = sm_dqtcfg.find(shm_key);
	if(it == sm_dqtcfg.end())
	{
		//未创建过，添加到sm_dqtcfg中
		sm_dqtcfg.insert(make_pair(shm_key, twok_cfg));
		m_pShmBaseAddr = twok_cfg.m_pShmBaseAddr;
		m_iDataBlockSize = twok_cfg.m_iDataBlockSize;
		m_iDataBlockNum = twok_cfg.m_iDataBlockNum;
	} 
	else 
	{
		if(rebuild) 
		{
			m_pShmBaseAddr = twok_cfg.m_pShmBaseAddr;
			m_iDataBlockSize = twok_cfg.m_iDataBlockSize;
			m_iDataBlockNum = twok_cfg.m_iDataBlockNum;
			it->second.m_pShmBaseAddr = twok_cfg.m_pShmBaseAddr;
			it->second.m_iDataBlockSize = twok_cfg.m_iDataBlockSize;
			it->second.m_iDataBlockNum = twok_cfg.m_iDataBlockNum; 
		} 
		else 
		{
			m_pShmBaseAddr = it->second.m_pShmBaseAddr;
			m_iDataBlockSize = it->second.m_iDataBlockSize;
			m_iDataBlockNum = it->second.m_iDataBlockNum;
		}
	}
	
}

bool ShmDataQueue::create(const char * filepath,const char * type,bool rebuild)
{
	struct DQ_Cfg dqcfg;
	if(!getcfgfromfile(filepath,type,dqcfg)) return false;

	return create(dqcfg,rebuild);
}

bool ShmDataQueue::getcfgfromfile(const char * filepath,const char * type, struct DQ_Cfg &dqcfg)
{
	if(NULL == filepath) return false;
	CXmlFile xmlFile;
	if (!xmlFile.Load(filepath))
	{
		fprintf(stderr,"load file %s failed\n", filepath);
		return false;
	}
	string nodepath = "configuration/shmdq/";
	nodepath += type;
	dqcfg.shm_key = xmlFile.GetLong(nodepath.c_str(), "shmkey");
	dqcfg.free_msgq_key = xmlFile.GetLong(nodepath.c_str(), "msgqkey");
	dqcfg.lock_key = xmlFile.GetLong(nodepath.c_str(), "lockkey");
	dqcfg.iDataBlockSize = xmlFile.GetLong(nodepath.c_str(), "datablocksize");
	dqcfg.iDataBlockNum = xmlFile.GetLong(nodepath.c_str(), "datablocknum");
	dqcfg.init_data_size = xmlFile.GetLong(nodepath.c_str(), "initdatasize");
	dqcfg.max_data_size = xmlFile.GetLong(nodepath.c_str(), "maxdatasize");

	return true;
}

bool ShmDataQueue::connect(const char * filepath,const char * type)
{
	struct DQ_Cfg dqcfg;
	if(!getcfgfromfile(filepath,type,dqcfg)) return false;

	return connect(dqcfg);
}

bool ShmDataQueue::create(DQ_Cfg dq_cfg,bool rebuild)
{	
	int shm_data_size=0;
	char * pShmBaseAddr =NULL;
	//检查三个键值配置是否正确
	DQ_TWOKEY_CFG twok_cfg;
	twok_cfg.free_msgq_key = dq_cfg.free_msgq_key;
	twok_cfg.lock_key = dq_cfg.lock_key;
	int ret = checkIfUniqueKey(dq_cfg.shm_key,twok_cfg);
	if( ERROR == ret ) 
	{
		return false;
	} 
	else if( NEWONE == ret || rebuild )
	{	
        shm_data_size=sizeof(DQ_SHM_INFO) + sizeof(DATA_ABSTR)*MAX_QUEUE_ID + dq_cfg.iDataBlockSize*dq_cfg.iDataBlockNum;
        if( dq_cfg.init_data_size > shm_data_size ) shm_data_size=dq_cfg.init_data_size;
        if( dq_cfg.max_data_size < shm_data_size )
		{
			cerr<<"数据队列共享内存最大值memory.MT_DATAQUEUE.max_data_size不够，系统要求至少"<<shm_data_size<<endl;
			return false;
		}

		if( !m_cAbstrAndDataShm.create(dq_cfg.shm_key, shm_data_size, rebuild) )
		{
			cerr<<"创建数据队列共享内存失败\n";
			return false;
		}
		
		pShmBaseAddr=m_cAbstrAndDataShm.getShmAddr();
	}
	/*判断是否已创建过
	*1.未创建过则添加
	*2.创建过且rebuild=false则取已创建过的
	*3.创建过且rebuild=true则重新创建并替换原来的
	*/
	twok_cfg.m_pShmBaseAddr = pShmBaseAddr;
	twok_cfg.m_iDataBlockSize = dq_cfg.iDataBlockSize;
	twok_cfg.m_iDataBlockNum = dq_cfg.iDataBlockNum;
	checkIfCreateKey( dq_cfg.shm_key, twok_cfg,rebuild);

	m_pDataBlockBaseAddr=m_pShmBaseAddr + sizeof(DQ_SHM_INFO) + sizeof(DATA_ABSTR)*MAX_QUEUE_ID;

	if( !m_cFreeDataBlkMsgQ.create(dq_cfg.free_msgq_key, rebuild) )
	{
		cerr<<"创建数据队列的空闲块消息队列失败\n";
		return false;
	}
	if( !m_cLockSem.create(dq_cfg.lock_key, rebuild) )
	{
		cerr<<"创建数据队列锁失败\n";
		return false;
	}
	if( m_cAbstrAndDataShm.m_bNewCreated != m_cFreeDataBlkMsgQ.m_bNewCreated )
	{
		cerr<<"【数据总线】数据队列IPC资源重建状态不统一！\n";
		return false;
	}

	if( m_cAbstrAndDataShm.m_bNewCreated )
	{
		DQ_SHM_INFO * p_shm_info=(DQ_SHM_INFO *)m_pShmBaseAddr;
		p_shm_info->size=shm_data_size;
		time( &(p_shm_info->create_time) );
	}

	if( m_cFreeDataBlkMsgQ.m_bNewCreated )
	{
		int i;
		for(i=0; i<m_iDataBlockNum; i++)
		{
			if( !putFreeDataBlock(i) )
			{
				cerr<<"生成数据队列的空闲块消息队列的第"<<i+1<<"个消息失败！";
				perror("原因");
				return false;
			}
		}
		DQ_PKG_HEAD *head;
		for(i=0; i<m_iDataBlockNum; i++)
		{
			head=getDataBlockHeadAddr(i);
			head->prev_index=-1;
			head->next_index=-1;
			head->next_block_index=-1;
		}
		DATA_ABSTR *data_abstr;
		for(i=0; i<MAX_QUEUE_ID; i++)
		{
			data_abstr=(DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + i;
			data_abstr->first_pkg_index=-1;
			data_abstr->last_pkg_index=-1;
		}
	}
	
	return true;
}

bool ShmDataQueue::connect(DQ_Cfg dq_cfg)
{
	char * pShmBaseAddr =NULL;
	//检查三个键值配置是否正确
	DQ_TWOKEY_CFG twok_cfg;
	twok_cfg.free_msgq_key = dq_cfg.free_msgq_key;
	twok_cfg.lock_key = dq_cfg.lock_key;
	int ret = checkIfUniqueKey(dq_cfg.shm_key,twok_cfg);
	if( ERROR == ret ) 
	{
		return false;
	} 
	else if( NEWONE == ret )
	{
        if( !m_cAbstrAndDataShm.connect(dq_cfg.shm_key) )
		{
			cerr<<"连接数据队列共享内存失败\n";
			return false;
		}
		pShmBaseAddr=m_cAbstrAndDataShm.getShmAddr();
	}
	/*判断是否已创建过
	*1.未创建过则添加
	*2.创建过且rebuild=false则取已创建过的
	*3.创建过且rebuild=true则重新创建并替换原来的
	*/
	twok_cfg.m_pShmBaseAddr = pShmBaseAddr;
	twok_cfg.m_iDataBlockSize = dq_cfg.iDataBlockSize;
	twok_cfg.m_iDataBlockNum = dq_cfg.iDataBlockNum;
	checkIfCreateKey( dq_cfg.shm_key, twok_cfg,false);
	
	m_pDataBlockBaseAddr=m_pShmBaseAddr + sizeof(DQ_SHM_INFO) + sizeof(DATA_ABSTR)*MAX_QUEUE_ID;
	
	if( !m_cFreeDataBlkMsgQ.connect(dq_cfg.free_msgq_key) )
	{
		cerr<<"连接数据队列的空闲块消息队列失败\n";
		return false;
	}
	if( !m_cLockSem.open(dq_cfg.lock_key) )
	{
		cerr<<"连接数据队列锁失败\n";
		return false;
	}
    return true;
}

bool ShmDataQueue::lock()
{
	return m_cLockSem.lock();
}

bool ShmDataQueue::unLock()
{
	return m_cLockSem.unLock();
}

int ShmDataQueue::getFreeDataBlock(int &block_index)
{
	FREE_BLK_MSG free_data_msg;
	int ret=m_cFreeDataBlkMsgQ.getMsg(MSGTP_FREE_DATA, &free_data_msg, sizeof(int), false);
	if( ret<=0 )
	{
		block_index=-1;
		return ret;
	}
	else
	{
		char *pBlockAddr=getDataBlockAddr(free_data_msg.index);
		memset(pBlockAddr, 0, m_iDataBlockSize);
		//DQ_PKG_HEAD *head=(DQ_PKG_HEAD *)pBlockAddr;
		block_index=free_data_msg.index;
		return 1;
	}
}

int ShmDataQueue::getFreeDataBlock(int block_num, vector<int> &block_index_set)
{
	FREE_BLK_MSG free_data_msg;
	int ret;
	block_index_set.clear();
	for(int i=0; i<block_num; i++)
	{
		ret=m_cFreeDataBlkMsgQ.getMsg(MSGTP_FREE_DATA, &free_data_msg, sizeof(int), false);
		if( ret<=0 )
		{
			putFreeDataBlock(block_index_set);
			block_index_set.clear();
			return ret;
		}
		else
		{
			char *pBlockAddr=getDataBlockAddr(free_data_msg.index);
			memset(pBlockAddr, 0, m_iDataBlockSize);
			block_index_set.push_back(free_data_msg.index);
		}
	}
	return block_num;
}

bool ShmDataQueue::putFreeDataBlock(int index)
{
	FREE_BLK_MSG free_data_msg;
	free_data_msg.msgtype=MSGTP_FREE_DATA;
	free_data_msg.index=index;
	getDataBlockHeadAddr(index)->inuse=false;
	return m_cFreeDataBlkMsgQ.putMsg(&free_data_msg, sizeof(int));
}

bool ShmDataQueue::putFreeDataBlock(vector<int> &block_index_set)
{
	for(size_t i=0; i<block_index_set.size(); i++)
	{
		if( !putFreeDataBlock(block_index_set[i]) ) return false;
	}
	return true;
}

char * ShmDataQueue::getDataBlockAddr(int index)
{
	if( index<0  || index>=m_iDataBlockNum ) return NULL;
	return ( m_pDataBlockBaseAddr + m_iDataBlockSize*index );
}

DQ_PKG_HEAD * ShmDataQueue::getDataBlockHeadAddr(int index)
{
	return (DQ_PKG_HEAD *)getDataBlockAddr(index);
}

int ShmDataQueue::writeData(char *buff, int size, int &data_index)
{
	//printf("writeData start\n");
	int all_size=size;
	//cerr << "writeData start" <<endl;
	if( all_size<=0 || all_size>(m_iDataBlockSize*m_iDataBlockNum) )
	{
	//cout << "all_size= " << all_size << endl;
	//cout << "m_iDataBlockSize= " << m_iDataBlockSize << endl;
	//cout << "m_iDataBlockNum= " << m_iDataBlockNum << endl;
	//cout << "m_iDataBlockSize*m_iDataBlockNum= " << m_iDataBlockSize*m_iDataBlockNum << endl;
#ifdef DEBUG
		cerr<<"1111包大小不正确，应该在 0 和 "<<(m_iDataBlockSize*m_iDataBlockNum)<<" 之间"<<endl;
#endif
		return -1;
	}
	//cout << "22222222222222" << endl;
	int data_block_num= (all_size-1) / (int)(m_iDataBlockSize-sizeof(DQ_PKG_HEAD)) + 1;
	char *pBlockAddr;
	DQ_PKG_HEAD *head;
	int ret;
	if( data_block_num==1 )		//只占一个块
	{
		int block_index;
		ret=getFreeDataBlock(block_index);
		if(ret<=0) return ret;
		pBlockAddr=getDataBlockAddr(block_index);
		memcpy(pBlockAddr+sizeof(DQ_PKG_HEAD), buff, all_size);
		head=(DQ_PKG_HEAD *)pBlockAddr;
		head->all_size=all_size;
		gettimeofday(&(head->data_time), 0);
		head->inuse=true;
		head->block_sn=0;
		head->next_block_index=-1;
		head->block_data_size=all_size;
		data_index=block_index;
		return 1;
	}
	else	//占多个块
	{
		vector<int> block_index_set;
		ret=getFreeDataBlock(data_block_num, block_index_set);
		if(ret<=0) return ret;
		char *pData=buff;
		int left_size=all_size;	//用来计算剩多少数据未保存
		for(int i=0; i<data_block_num; i++)
		{
			pBlockAddr=getDataBlockAddr(block_index_set[i]);
			head=(DQ_PKG_HEAD *)pBlockAddr;
			head->all_size=all_size;
			gettimeofday(&(head->data_time), 0);
			head->inuse=true;
			head->block_sn=i;
			head->prev_index=-1;
			head->next_index=-1;
			if( i < data_block_num-1 )	//非最后一个块
			{
				head->next_block_index=block_index_set[i+1];
				head->block_data_size=m_iDataBlockSize-sizeof(DQ_PKG_HEAD);
				memcpy(pBlockAddr+sizeof(DQ_PKG_HEAD), pData, head->block_data_size);
				pData += head->block_data_size;
				left_size -= head->block_data_size;
			}
			else		//最后一个块
			{
				head->next_block_index=-1;
				head->block_data_size=left_size;
				memcpy(pBlockAddr+sizeof(DQ_PKG_HEAD), pData, head->block_data_size);
			}
		}
		data_index=block_index_set[0];
	}
	return 1;
}

bool ShmDataQueue::linkData(int queue_id, int size, int data_index)
{
	DATA_ABSTR *data_abstr;
	data_abstr=(DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + queue_id;
	DQ_PKG_HEAD *head=getDataBlockHeadAddr(data_index);
	if( data_abstr->last_pkg_index==-1)	//空
	{
		head->prev_index=-1;
		head->next_index=-1;
		data_abstr->first_pkg_index=data_index;
		data_abstr->last_pkg_index=data_index;
	}
	else
	{
		DQ_PKG_HEAD *prev_head=getDataBlockHeadAddr(data_abstr->last_pkg_index);
		prev_head->next_index=data_index;
		head->prev_index=data_abstr->last_pkg_index;
		head->next_index=-1;
		data_abstr->last_pkg_index=data_index;
	}
	data_abstr->pkg_count++;
	data_abstr->pkg_size+=size;
	return true;
}

int ShmDataQueue::putData(char *buff, int size,int queue_id)
{
	//printf("file:%s,fuc:%s,line:%d\n",__FILE__, __FUNCTION__, __LINE__);
	if(queue_id<0 || queue_id>=MAX_QUEUE_ID) return -1;
	int ret, data_index;
	//cout << "44444444" << endl;
	ret=writeData(buff, size, data_index);
	//cout << "555555555555555" << endl;
	if(ret<=0) return ret;
	//cout << "66666666666" << endl;
	if( !lock() ) return -1;
	if( !linkData( queue_id, size, data_index ) )
	{
		unLock();
		return -1;
	}
	unLock();
	return 1;
}

int ShmDataQueue::unLinkData(int queue_id, unsigned char unlink_type, int &data_index, int spc_data_index)
{
	DATA_ABSTR *data_abstr;
	data_abstr=(DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + queue_id;
	
	//获取数据块序号
	switch(unlink_type)
	{
	case ULNKTP_HIGHEST_PKG:	//优先级最高的
	case ULNKTP_BATCH:			//批量的
		if( data_abstr->first_pkg_index == -1 ) return 0;
		data_index=data_abstr->first_pkg_index;
		break;

	case ULNKTP_LOWEST_PKG:		//优先级最低的
		if( data_abstr->last_pkg_index == -1 ) return 0;
		data_index=data_abstr->last_pkg_index;
		break;

	case ULNKTP_SPEC_PKG:		//指定序号数据块的
		{
			data_index=spc_data_index;
			DQ_PKG_HEAD *head=getDataBlockHeadAddr(data_index);
			if(head==NULL) return 0;
			if( !(head->inuse) || head->block_sn != 0 ) return 0;
		}
		break;
	}

	int unlink_pkg_count;

	//数据断链
	if( unlink_type == ULNKTP_BATCH )	//批量的
	{
		data_abstr->first_pkg_index=-1;
		data_abstr->last_pkg_index=-1;
		unlink_pkg_count=data_abstr->pkg_count;
		data_abstr->pkg_count=0;
		data_abstr->pkg_size=0;
	}
	else	//单个的
	{
		DQ_PKG_HEAD *head=getDataBlockHeadAddr(data_index);
		if( head==NULL ) return 0;
		if( head->prev_index >= 0 )
		{
			DQ_PKG_HEAD *prev_head=getDataBlockHeadAddr(head->prev_index);
			prev_head->next_index=head->next_index;
		}
		else data_abstr->first_pkg_index=head->next_index;

		if( head->next_index >= 0 )
		{
			DQ_PKG_HEAD *next_head=getDataBlockHeadAddr(head->next_index);
			next_head->prev_index=head->prev_index;
		}
		else data_abstr->last_pkg_index=head->prev_index;

		data_abstr->pkg_count--;
		data_abstr->pkg_size-=head->all_size;
		head->prev_index=-1;
		head->next_index=-1;

		unlink_pkg_count=1;
	}

	return unlink_pkg_count;
}

int ShmDataQueue::readData(int data_index, char *buff, int buff_size, int &size)
{
	char *pBlockAddr=getDataBlockAddr(data_index);
	DQ_PKG_HEAD *head;
	head=(DQ_PKG_HEAD *)pBlockAddr;

	size=head->all_size;	//赋给传出参数
	if( buff_size < size ) return -1;

	if(head->next_block_index==-1)	//该包只有一个数据块
	{
		memcpy(buff, pBlockAddr+sizeof(DQ_PKG_HEAD), size);
		head->inuse=false;
		head->block_sn=0;
		head->block_data_size=0;
		head->next_block_index=-1;
		head->prev_index=-1;
		head->next_index=-1;
		if( !putFreeDataBlock(data_index) ) return -1;	//回收数据块
	}
	else
	{
		char *p=buff;
		int next_block_index;
		while(true)
		{
			memcpy(p, pBlockAddr+sizeof(DQ_PKG_HEAD), head->block_data_size);
			p+=head->block_data_size;
			next_block_index=head->next_block_index;
			head->inuse=false;
			head->block_sn=0;
			head->block_data_size=0;
			head->next_block_index=-1;
			head->prev_index=-1;
			head->next_index=-1;
			if( !putFreeDataBlock(data_index) ) return -1;	//回收数据块
			if(next_block_index==-1) break;
			data_index=next_block_index;	//指向下一数据块
			pBlockAddr=getDataBlockAddr(data_index);
			head=(DQ_PKG_HEAD *)pBlockAddr;
		}
	}
	
	return 1;
}

int ShmDataQueue::readData(int data_index, vector<char> &buff)
{
	char *pBlockAddr=getDataBlockAddr(data_index);
	DQ_PKG_HEAD *head;
	head=(DQ_PKG_HEAD *)pBlockAddr;

	size_t size=head->all_size;
	if( buff.capacity() < size )
	{
		if( size<=1024 ) buff.reserve(1024);
		else if( size<=2048 ) buff.reserve(2048);
		else if( size<=4096 ) buff.reserve(4096);
		else if( size<=10240 ) buff.reserve(10240);
	}
	buff.resize(size);

	if(head->next_block_index==-1)	//该包只有一个数据块
	{
		memcpy(&(buff.front()), pBlockAddr+sizeof(DQ_PKG_HEAD), size);
		
		head->inuse=false;
		head->block_sn=0;
		head->block_data_size=0;
		head->next_block_index=-1;
		head->prev_index=-1;
		head->next_index=-1;

		if( !putFreeDataBlock(data_index) ) return -1;	//回收数据块
	}
	else
	{
		char *p=&(buff.front());
		int next_block_index;
		while(true)
		{
			memcpy(p, pBlockAddr+sizeof(DQ_PKG_HEAD), head->block_data_size);
			p+=head->block_data_size;
			next_block_index=head->next_block_index;

			head->inuse=false;
			head->block_sn=0;
			head->block_data_size=0;
			head->next_block_index=-1;
			head->prev_index=-1;
			head->next_index=-1;

			if( !putFreeDataBlock(data_index) ) return -1;	//回收数据块
			if(next_block_index==-1) break;
			data_index=next_block_index;	//指向下一数据块
			pBlockAddr=getDataBlockAddr(data_index);
			head=(DQ_PKG_HEAD *)pBlockAddr;
		}
	}
	
	return 1;
}

int ShmDataQueue::getData(vector<char> &buff,int queue_id)
{
	if(queue_id<0 || queue_id>=MAX_QUEUE_ID) return -1;
	if( ( (DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + queue_id )->first_pkg_index == -1 ) return 0;
	if( !lock() ) return -1;
	int ret, data_index;
	ret=unLinkData(queue_id, ULNKTP_HIGHEST_PKG, data_index);
	if( ret<=0 )
	{
		unLock();
		return ret;
	}
	unLock();
	return readData(data_index, buff);
}

//批量删除时，删除以data_index开始的数据包链
//删除单个数据包时，由于unLinkData函数已经将该包从数据包链中断开，所以只会删除一个包
bool ShmDataQueue::removeData(int data_index)
{
	DQ_PKG_HEAD *head;
	int next_index, block_index, next_block_index;
	while(true)
	{
		if( data_index<0 ) break;
		head=getDataBlockHeadAddr(data_index);
		next_index=head->next_index;
		block_index=data_index;
		do
		{
			next_block_index=head->next_block_index;
			head->inuse=false;
			head->block_sn=0;
			head->block_data_size=0;
			head->next_block_index=-1;
			head->prev_index=-1;
			head->next_index=-1;
			if( !putFreeDataBlock(block_index) ) return false;	//回收数据块
			block_index=next_block_index;	//指向下一数据块
			if(block_index<0) break;
			head=getDataBlockHeadAddr(block_index);
		}while(true);

		data_index=next_index;
	}
	return true;
}

int ShmDataQueue::delData(int queue_id, unsigned char unlink_type, int spc_data_index)
{
	if(queue_id<0 || queue_id>=MAX_QUEUE_ID) return -1;
	int ret, ret_data_index;
	if( !lock() ) return -1;
	ret=unLinkData(queue_id, unlink_type, ret_data_index, spc_data_index);
	if(ret<=0)
	{
		unLock();
		return ret;
	}
	unLock();
	
	if( ! removeData(ret_data_index) ) return -1;

	return ret;
}

int ShmDataQueue::removeTmoutData(int tmout_sec)
{
	DATA_ABSTR *data_abstr;
	DQ_PKG_HEAD *head;
	time_t now_time;
	time(&now_time);
	int ret, cnt=0;
	for(int i=0; i<MAX_QUEUE_ID; i++)
	{
		data_abstr=(DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + i;
		if( data_abstr->first_pkg_index == -1 ) continue;
		head=getDataBlockHeadAddr(data_abstr->first_pkg_index);
		if( now_time - (head->data_time.tv_sec) >= tmout_sec )
		{
			ret=delData(i, ULNKTP_BATCH);
			if(ret<0) return -1;
			cnt+=ret;
		}
	}
	return cnt;
}

//获取数据队列队列信息
void ShmDataQueue::getQueInfo(int queue_id, int &pkg_count, int &pkg_size)
{
	if(queue_id<0 || queue_id>=MAX_QUEUE_ID)
	{
		pkg_count=0;
		pkg_size=0;
		return;
	}
	DATA_ABSTR *data_abstr;
	data_abstr=(DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + queue_id;
	pkg_count=data_abstr->pkg_count;
	pkg_size=data_abstr->pkg_size;
}

//获取数据队列使用信息
bool ShmDataQueue::getBuffInfo(int &used_blk_num, int &all_blk_num)
{
	int free_num=m_cFreeDataBlkMsgQ.getMsgNum();
	if( free_num<0 ) return false;
	used_blk_num=m_iDataBlockNum-free_num;
	all_blk_num=m_iDataBlockNum;
	return true;
}

void ShmDataQueue::getLastErr(int &err_code, string &err_msg)
{
	err_code=m_iErrCode;
	err_msg=m_sErrMsg;
}

void ShmDataQueue::printAbstrQue(const char*msg)
{
	fprintf(stderr, "################ %s-队列信息 ############################################\n", msg==NULL?"":msg);
	DATA_ABSTR *data_abstr;
	for(int i=0; i<MAX_QUEUE_ID; i++)
	{
		data_abstr=(DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + i;
		if( data_abstr->pkg_count>0 )
		{
			fprintf(stderr, "---------------------------\n"
							"【Queue_ID=%d】"
							"包数量=%d, 包大小=%d\n"
							"第一数据块索引=%d, 最后数据块索引=%d\n"
							"---------------------------\n",
							i,
							data_abstr->pkg_count, data_abstr->pkg_size,
							data_abstr->first_pkg_index, data_abstr->last_pkg_index);
		}
	}
	fprintf(stderr, "################ %s-队列信息 #########################################\n\n", msg==NULL?"":msg);
}

void ShmDataQueue::printDataQue()
{
	DQ_PKG_HEAD *head;
	cerr<<"############## 所有在用数据块信息 ####################################\n";
	int cnt=0;
	for(int i=0; i<m_iDataBlockNum; i++)
	{
		head=getDataBlockHeadAddr(i);
		if( head->inuse )
		{
			fprintf(stderr, "---------------------------\n"
			            "【本节点=%d】"
						"包总大小=%d, 块序列号=%d, 块大小=%d,\n"
						"后一数据块索引=%d, 前一节点=%d, 后一节点=%d\n"
						"---------------------------\n",
						i, head->all_size, head->block_sn, head->block_data_size,
						head->next_block_index, head->prev_index, head->next_index);
			cnt++;
			if( cnt%10==0 )
			{
				cerr<<"按回车继续...\n";
				getchar();
			}
		}
	}
	cerr<<"######################################################################\n";
}

bool ShmDataQueue::getQueInfo(int queue_id, DQ_DATA_ABSTR &dq_data_sbstr, const int &data_len)
{
    dq_data_sbstr.pkg_count = 0;
    dq_data_sbstr.pkg_size = 0;

    if(data_len > m_iDataBlockSize)
        return false;
    if(queue_id<0 || queue_id>=MAX_QUEUE_ID)
	    return false;
    int data_index = -1;
    DATA_ABSTR *data_abstr;
    
    data_abstr=(DATA_ABSTR *)( m_pShmBaseAddr + sizeof(DQ_SHM_INFO) ) + queue_id;
    if( data_abstr->pkg_count>0 )
    {
        dq_data_sbstr.pkg_count = data_abstr->pkg_count;
        dq_data_sbstr.pkg_size = data_abstr->pkg_size,
        data_index = data_abstr->first_pkg_index;
    }
    if(data_index > -1)
    {
        char *pBlockAddr=getDataBlockAddr(data_index);
        DQ_PKG_HEAD *head;
        head=(DQ_PKG_HEAD *)pBlockAddr;
        dq_data_sbstr.first_data.resize(data_len);
        memcpy((void*)dq_data_sbstr.first_data.data(), pBlockAddr+sizeof(DQ_PKG_HEAD), data_len);
    }
    return true;
}

bool ShmDataQueue::getQueInfo(map<int, DQ_DATA_ABSTR> &all_sbstr, const int &data_len)
{
    for(int i=0; i<MAX_QUEUE_ID; i++)
    {
        DQ_DATA_ABSTR dq_data_sbstr;
        getQueInfo(i, dq_data_sbstr, data_len);
        if(dq_data_sbstr.pkg_count > 0)
            all_sbstr.insert(make_pair(i, dq_data_sbstr));
    }
    return true;
}

void ShmDataQueue::getShmInfo(DQ_SHM_INFO &shm_info)
{
	shm_info=*(DQ_SHM_INFO *)m_pShmBaseAddr;
}

//***************************************************************
//消息队列类
DQ_MsgQ::DQ_MsgQ()
{
	m_iMsgID=-1;
}

DQ_MsgQ::~DQ_MsgQ()
{
}

bool DQ_MsgQ::create(key_t msgkey, bool rebuild)
{
	m_iMsgID=msgget(msgkey, 0666);
	if(m_iMsgID>=0 && rebuild)
	{
		if( msgctl(m_iMsgID, IPC_RMID, NULL)!=0 )
		{
			perror("删除消息队列失败原因");
			return false;
		}
	}
	m_bNewCreated=(m_iMsgID==-1 || rebuild);
	if(m_bNewCreated) m_iMsgID=msgget(msgkey, IPC_CREAT|0666);
	if(m_iMsgID<0)	perror("创建消息队列失败原因");
	return (m_iMsgID>=0);
}

bool DQ_MsgQ::connect(key_t msgkey)
{
	m_iMsgID=msgget(msgkey, 0666);
	if(m_iMsgID<0)	perror("连接消息队列失败原因");
	return (m_iMsgID>=0);
}

int DQ_MsgQ::getMsgNum()
{
	if(m_iMsgID==-1) return -1;
	struct msqid_ds buf;
	if( msgctl(m_iMsgID, IPC_STAT, &buf)!=0 ) return -1;
	return buf.msg_qnum;
}

int DQ_MsgQ::getMsg(long msgtype, void *msgbuf, size_t msgsize, bool sync)
{
	if(m_iMsgID==-1) return -1;
	if( msgrcv(m_iMsgID, msgbuf, msgsize, msgtype, sync?0:IPC_NOWAIT)<0 )
	{
		if(errno==ENOMSG) return 0;
		else return -1;
	}

	return 1;
}

bool DQ_MsgQ::putMsg(void *msgbuf, size_t msgsize)
{
	if(m_iMsgID==-1) return false;
	if( msgsnd(m_iMsgID, msgbuf, msgsize, IPC_NOWAIT)<0 ) return false;
	return true;
}

void DQ_MsgQ::remove()
{
	if(m_iMsgID!=-1)
	{
		msgctl(m_iMsgID, IPC_RMID, NULL);
	}
}

//******************************************************************
//共享内存类
DQ_Shm::DQ_Shm()
{
	m_iShmID=-1;
	m_pShmAddr=(char *)-1;
	m_bNewCreated=false;
}

DQ_Shm::~DQ_Shm()
{
}

bool DQ_Shm::create(key_t shmkey, size_t size, bool rebuild)
{
	m_iShmID=shmget(shmkey, 0, 0666);
	
	if(m_iShmID>=0 && rebuild)
	{
		if( shmctl(m_iShmID,IPC_RMID,NULL)==-1 )
		{
			perror("删除共享内存失败原因");
			return false;
		}
	}
	m_bNewCreated=(m_iShmID==-1 || rebuild);

	if(m_bNewCreated) m_iShmID = shmget(shmkey, size, IPC_CREAT|0666);
	if(m_iShmID==-1)
	{
		perror("创建共享内存失败原因");
		return false;
	}

	m_pShmAddr=(char *)shmat(m_iShmID,0,0);
	if(m_pShmAddr==(char *)-1)
	{
		perror("映射共享内存失败原因");
		return false;
	}

	return true;
}

bool DQ_Shm::connect(key_t shmkey)
{
	m_iShmID=shmget(shmkey, 0, 0666);
	if(m_iShmID==-1)
	{
		perror("连接共享内存失败原因");
		return false;
	}
	m_pShmAddr=(char *)shmat(m_iShmID,0,0);
	if(m_pShmAddr==(char *)-1)
	{
		perror("映射共享内存失败原因");
		return false;
	}
	return true;
}

void DQ_Shm::detach()
{
	if(m_pShmAddr!=(char *)-1) 
	{
		shmdt(m_pShmAddr);
		m_pShmAddr=(char *)-1;
	}
}

char *DQ_Shm::getShmAddr()
{
	return m_pShmAddr;
}

void DQ_Shm::remove()
{
	if(m_iShmID!=-1)
	{
		detach();
		shmctl(m_iShmID,IPC_RMID,NULL);
	}
}

//******************************************************************
//信号灯锁类
DQ_Sem::DQ_Sem()
{
	m_iSemID=-1;
	m_bNewCreated=false;
}

DQ_Sem::~DQ_Sem()
{
}

bool DQ_Sem::create(key_t semkey, bool rebuild)
{
	m_iSemID=semget(semkey, 1, 0666);
	
	if(m_iSemID>=0 && rebuild)
	{
		if( semctl(m_iSemID, 0, IPC_RMID)==-1 )
		{
			perror("删除信号灯失败原因");
			return false;
		}
	}
	m_bNewCreated=(m_iSemID==-1 || rebuild);

	if(m_bNewCreated) m_iSemID = semget(semkey, 1, IPC_CREAT|0666);
	if(m_iSemID==-1)
	{
		perror("创建信号灯失败原因");
		return false;
	}
	return true;
}

bool DQ_Sem::open(key_t semkey)
{
	m_iSemID=semget(semkey, 1, 0666);
	if(m_iSemID==-1)
	{
		perror("连接信号灯失败原因");
		return false;
	}
	return true;
}

bool DQ_Sem::lock()
{
	struct sembuf LockOp[2]={{0,0,0}, {0,1,SEM_UNDO}};
	if( semop(m_iSemID, LockOp, 2)==-1 ) return false;
	return true;
}

bool DQ_Sem::unLock()
{
	struct sembuf LockOp[1]={ {0, -1, SEM_UNDO} };
	if( semop(m_iSemID, LockOp, 1)==-1 ) return false;
	return true;
}
