
#pragma once

#define COMM_LOG_SHM_FILE "/tmp/commlogshm.lock"
#define COMM_LOG_SHM_KEY 0x37C4D1
#define COMM_LOG_SHM_SIZE 256  // 256MB

#define COMM_LOG_SHM_MAINVER     1
#define COMM_LOG_SHM_SUBVER      0
#define COMM_LOG_SHM_MAGIC       0x8d33ef8a

#define COMM_LOG_SHM_RESET_FLAG  0x2d5a991f

#define COMM_LOG_SHM_PATH "/home/qspace/log"

#define COMM_LOG_ITEM_MAGICNUM_SIZE 1

#define COMM_LOG_ATTR_SHM_KEY 0x48f233e
#define COMM_LOG_ATTR_SHM_FILE "/tmp/attrlogshm.lock"
namespace Comm {

    class LogShmQueue {
    public:

        static char COMM_LOG_ITEM_MAGICNUM_1;
        static char COMM_LOG_ITEM_MAGICNUM_2;

        static int  COMM_LOG_ITEM_SECT;

#pragma pack(1)
        typedef struct _LogShmQueueHead_t {
            unsigned short hMainVer;
            unsigned short hSubVer;

            unsigned int   iPadding1;

            unsigned int   iMagicNum;
            volatile int   iQueueSize;

            volatile int   iHeadPos; 
            unsigned int   iPadding2;

            volatile int   iTailPos; 

            unsigned int   iResetFlag;

            char sReserved[512 - sizeof(unsigned short)*2 - sizeof(unsigned int)*7];
            char sData[1];
        } LogShmQueueHead_t;
#pragma pack()

        LogShmQueue (int iPushRetryCount = 10);
        virtual ~LogShmQueue ();

        int create(const char *lockpath=COMM_LOG_SHM_FILE, const int shmkey=COMM_LOG_SHM_KEY,
                const int shmsize=COMM_LOG_SHM_SIZE);

        int init( const int shmkey=COMM_LOG_SHM_KEY, const int shmsize=COMM_LOG_SHM_SIZE);

        int push(const int len, const char * log);

        int pop(int &iLogLen, char* pBuffer);

        int reset();

		int pop_with_size( int iMaxSize, int * tLen, char ** ptBuf, int & retCount );

    private:
        void writeownlog(const char *sFormat, ...);

        LogShmQueueHead_t * m_ptHeader;
        size_t m_iShmSize;
        int m_iPushRetryCount;
        char m_sLockPath[512];
        int m_iShmKey;
        int m_iQueueSize;
    };


}

