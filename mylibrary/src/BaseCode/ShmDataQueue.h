#ifndef __SHM_DATA_QUEUE_H__
#define __SHM_DATA_QUEUE_H__

#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define ULNKTP_HIGHEST_PKG	1
#define ULNKTP_LOWEST_PKG	2
#define ULNKTP_SPEC_PKG		3
#define ULNKTP_BATCH		4

#define MAX_QUEUE_ID		500000

/*����ժҪ��Ϣ����������first_data�����׿�����*/
struct DQ_DATA_ABSTR
{
    DQ_DATA_ABSTR()
	{
		pkg_count = 0;
		pkg_size = 0;
	}
    int	pkg_count;
	int pkg_size;
    std::string first_data;
};

/*DataQueue��Ҫ��������Ϣ*/
struct DQ_Cfg
{
    key_t shm_key;          /*�����ڴ�key*/
    key_t free_msgq_key;    /*��Ϣ����key*/
    key_t lock_key;         /*�ź���key*/
    int iDataBlockSize;     /*�����С*/
    int iDataBlockNum;      /*�ܿ���*/
    int init_data_size;     /*��ʼ�������ڴ��С(��ʵ��ʼ����СΪiDataBlockSize*iDataBlockNum+DataQueue�ڲ��ṹ)*/
    int max_data_size;      /*�����ڴ�����С*/
};

struct DQ_TWOKEY_CFG
{
	DQ_TWOKEY_CFG()
	{
		free_msgq_key = -1;
		lock_key = -1;
		m_pShmBaseAddr = NULL;
		m_iDataBlockSize = -1;
		m_iDataBlockNum = -1;
	}

    key_t free_msgq_key;
    key_t lock_key;
	char *m_pShmBaseAddr;
	int	 m_iDataBlockSize;
	int	 m_iDataBlockNum;
};

//-----IPC-----------
class DQ_MsgQ
{
private:
	int m_iMsgID;
public:
	DQ_MsgQ();
	~DQ_MsgQ();
	bool m_bNewCreated;
	bool create(key_t msgkey, bool rebuild=false);
	bool connect(key_t msgkey);
	int  getMsg(long msgtype, void *msgbuf, size_t msgsize, bool sync=true);
	bool putMsg(void *msgbuf, size_t msgsize);
	int  getMsgNum();
	void remove();
};

class DQ_Shm
{
private:
	int m_iShmID;
	char *m_pShmAddr;
public:
	DQ_Shm();
	~DQ_Shm();
	bool m_bNewCreated;
	bool create(key_t shmkey, size_t size, bool rebuild=false);
	bool connect(key_t shmkey);
	char *getShmAddr();
	void detach();
	void remove();
};

class DQ_Sem
{
private:
	int m_iSemID;
public:
	DQ_Sem();
	~DQ_Sem();
	bool m_bNewCreated;
	bool create(key_t semkey, bool rebuild=false);
	bool open(key_t semkey);
	bool lock();
	bool unLock();
};

//#################################################################################
typedef struct
{
	int size;
	time_t create_time;
}DQ_SHM_INFO;

typedef struct
{
	int		all_size;				//ȫ����С(����������ͷ)
	bool	inuse;					//�Ƿ�ռ�ñ�ʶ
	struct	timeval data_time;		//����ʱ��(�Է���ͨ��ʱ��Ϊ׼)
	int		prev_index;				//ǰһ����λ��
	int		next_index;				//��һ����λ��
	int		block_sn;				//�����
	int		next_block_index;		//��һ�������
	int		block_data_size;		//����ʵ�����ݴ�С
}DQ_PKG_HEAD;

//��������������
class ShmDataQueue
{
private:
	DQ_MsgQ m_cFreeDataBlkMsgQ;
	DQ_Sem	 m_cLockSem;
	DQ_Shm  m_cAbstrAndDataShm;
	// static char *m_pShmBaseAddr;
	// static int	 m_iDataBlockSize;
	// static int	 m_iDataBlockNum;
	char *m_pShmBaseAddr;
	int	 m_iDataBlockSize;
	int	 m_iDataBlockNum;
	char *   m_pDataBlockBaseAddr;
	int m_iErrCode;
	std::string m_sErrMsg;
	
	enum DqState{ERROR=-1,NOR =0, AlreadyExist, NEWONE};
	//key_tΪ�����ڴ��ֵ,DQ_TWOKEY_CFG������Ϣ���С��źŵƼ�ֵ��Ψһһ������Ϣ
	static std::map<key_t, DQ_TWOKEY_CFG> sm_dqtcfg;
	void checkIfCreateKey(key_t shm_key,DQ_TWOKEY_CFG twok_cfg,bool rebuild);
	int checkIfUniqueKey(key_t shm_key,DQ_TWOKEY_CFG twok_cfg);
	
	int	 getFreeDataBlock(int &block_index);
	int  getFreeDataBlock(int block_num, std::vector<int> &block_index_set);
	bool putFreeDataBlock(int index);
	bool putFreeDataBlock(std::vector<int> &block_index_set);

	char * getDataBlockAddr(int index);
	DQ_PKG_HEAD *getDataBlockHeadAddr(int index);

	int  writeData(char *buff, int size, int &data_index);
	int  readData(int data_index, char *buff, int buff_size, int &size);
	int  readData(int data_index, std::vector<char> &buff);
	bool removeData(int data_index);
	bool linkData(int queue_id, int size, int data_index);
	int  unLinkData(int queue_id, unsigned char unlink_type, int &data_index, int spc_data_index=-1);
	
	bool lock();
	bool unLock();
	bool getcfgfromfile(const char * filepath,const char * type , struct DQ_Cfg &dqcfg);

public:
	ShmDataQueue();
	~ShmDataQueue();

	//�������ͻ�����
    bool create(DQ_Cfg dq_cfg,bool rebuild=false);
    bool create(const char * filepath,const char * type,bool rebuild=false);
	//���ӷ��ͻ�����
	bool connect(const char * filepath,const char * type);
    bool connect(DQ_Cfg dq_cfg);

	//�������ݵ����ͻ�����
	//����ֵ��1-�ɹ���0-û���㹻�����ݿ��п飬-1-����
	int  putData(char *buff, int size,int queue_id=1);

	//�ڷ��ͻ������л�ȡ����
	//����ֵ��1-�ɹ���0-û�����ݣ�-1-����
	//int  getData(int link_id, char *buff, int buff_size, int &size);
	int  getData(std::vector<char> &buff,int queue_id=1);

	//�ڷ��ͻ�������ɾ������(spc_data_index��Ϊ-1ʱ��link_id��Ч)
	int delData(int queue_id, unsigned char unlink_type=ULNKTP_BATCH, int spc_data_index=-1);

	//ɾ����ʱ����
	int removeTmoutData(int tmout_sec);

	//��ȡ������������Ϣ
	void getQueInfo(int queue_id, int &pkg_count, int &pkg_size);
    bool getQueInfo(int queue_id, DQ_DATA_ABSTR &data_sbstr, const int &data_len);
    /*��������queue_id�е�ժҪ��Ϣ*/
    bool getQueInfo(std::map<int, DQ_DATA_ABSTR> &all_sbstr, const int &data_len);

	//��ȡ������ʹ����Ϣ
	bool getBuffInfo(int &used_blk_num, int &all_blk_num);

	void getLastErr(int &err_code, std::string &err_msg);

	void getShmInfo(DQ_SHM_INFO &shm_info);

	void printAbstrQue(const char*msg=NULL);
	void printDataQue();
};


#endif
