/*
 * =====================================================================================
 *
 *       Filename:  IDataOperator.h
 *
 *    Description:  database operator interface
 *
 *        Version:  1.0
 *        Created:  2015年06月25日 09时40分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), apenge2004@gmail.com
 *   Organization:  www.skyunion.net(IGG)
 *
 * =====================================================================================
 */

#ifndef _GFX_DBADAPTER_DATA_OPERATOR_H_
#define _GFX_DBADAPTER_DATA_OPERATOR_H_


namespace my_data {

class CRecord;
class CRecordSet;
class IDataOperator
{
public:
	virtual ~IDataOperator()
	{
	}

	virtual bool Reload(CRecord *pRecord, bool ignoreChange) = 0;
	virtual bool Reload(CRecordSet *pRecordSet, bool ignoreChange) = 0;

	virtual ISimDB* getDBConn(void) { return m_pDB; }

protected:
	IDataOperator()
		: m_pDB(NULL)
	{
	}

protected:
	ISimDB*		m_pDB;
};

} // namespace my_data

#endif // _GFX_DBADAPTER_DATA_OPERATOR_H_


