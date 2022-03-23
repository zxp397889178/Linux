#ifndef _GFX_MYDB_MYFIELDS_H_
#define _GFX_MYDB_MYFIELDS_H_

#include <mysql.h>
#include "IMyField.h"
#include <map>
#include "BaseCode.h"



class CMyFieldSet
{
    typedef std::vector<IMyField*>            FIELDS;
    typedef std::map<CMyString, IMyField*>    FIELD_MAP_BY_NAME;
    //MYPOOL_EX_DECLARATION(CMyFieldSet, s_objHeap)
public:
    CMyFieldSet();
    ~CMyFieldSet(void);

    void Init(MYSQL_FIELD *pFields, uint8_t nFieldCount);
    void Reset();

    size_t FieldCount(void) const;
    IMyField* Find(uint8_t nIndex);
    IMyField* operator[](int32_t nIndex);
    IMyField* FindByName(const char *pszName);
    int32_t FieldIndex(const char *pszName);

    //PrimaryKey 有可能是多个字段
    bool HasPrimaryField(void);
    size_t GetPrimaryField(std::vector<IMyField*> &arKeyField);
    size_t GetPrimaryFieldIndex(std::vector<int> &arKeyField);

private:
    FIELDS                  m_arFields;
    FIELD_MAP_BY_NAME       m_mapFieldByName;
};


#endif  //_GFX_MYDB_MYFIELDS_H_


