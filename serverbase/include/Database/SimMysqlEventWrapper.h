#ifndef _GFX_MYDB_SIMPLE_MYSQL_EVENT_WRAPPER_H_
#define _GFX_MYDB_SIMPLE_MYSQL_EVENT_WRAPPER_H_

#include "ISimDB.h"
#include "BaseCode.h"



class CSimMySqlEventWrapper
{
public:
    CSimMySqlEventWrapper(ISimDBEvent *dbEvent, CMyString &strSql)
        : m_pEvent(dbEvent)
        , m_strSql(strSql)
        , m_bError(false)
    {
        CMyString strStyle = strSql.left(7);
		strStyle.MakeUpper();
        if (strStyle.Find("INSERT") != CMyString::npos)
			m_eSqlStyle = ISimDBEvent::eventInsert;
        else if (strStyle.Find("UPDATE")!= CMyString::npos)
			m_eSqlStyle = ISimDBEvent::eventUpdate;
        else if (strStyle.Find("DELETE")!= CMyString::npos)
			m_eSqlStyle = ISimDBEvent::eventDelete;
        else if (strStyle.Find("SELECT")!= CMyString::npos)
			m_eSqlStyle = ISimDBEvent::eventSelect;
        else if (strStyle.Find("CALL")!= CMyString::npos)
			m_eSqlStyle = ISimDBEvent::eventProcedure;
        else
			m_eSqlStyle = ISimDBEvent::eventOther;
        m_timeStart = Now();
    }

    enum ISimDBEvent::DB_EVENT GetStyle()
    {
        return m_eSqlStyle;
    }

    void SetError(void)
    {
        m_bError = true; 
    }

    ~CSimMySqlEventWrapper()
    {
        if (NULL != m_pEvent)
        {
            if (m_bError)
            {
                m_pEvent->OnError(this->GetStyle(), m_strSql.c_str());
            }
            else
            {
                uint64_t timeCost = Now() - m_timeStart;
                m_pEvent->OnEvent(this->GetStyle(), (double)timeCost);
            }
        }
    }

private:
    ISimDBEvent*                m_pEvent;
    CMyString                   m_strSql;
    enum ISimDBEvent::DB_EVENT  m_eSqlStyle;
    uint64_t                    m_timeStart;
    bool                        m_bError;

};


#endif  //_GFX_MYDB_SIMPLE_MYSQL_EVENT_WRAPPER_H_

