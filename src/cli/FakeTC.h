#pragma once
#include <memory>
#include "Define.h"
#include "DBCStructure.h"
#include "SpellMgr.h"
#ifdef TC_LOG_ERROR
#undef TC_LOG_ERROR
#define TC_LOG_ERROR(filterType__, ...) (void)filterType__
#endif

class MYSQL_RES {};
class MYSQL_FIELD {};
class Field
{
public:
    uint32 GetUInt32() const
    {
        return 0;
    }
    uint8 GetUInt8()
    {
        return 0;
    }
    float GetFloat()
    {
        return 0.f;
    }
};
#ifndef ASSERT
#define ASSERT(x)
#endif

class ResultSet
{
public:
    ResultSet(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount);
    ~ResultSet();

    bool NextRow();
    uint64 GetRowCount() const { return _rowCount; }
    uint32 GetFieldCount() const { return _fieldCount; }

    Field* Fetch() const { return _currentRow; }
    const Field & operator [] (uint32 index) const
    {
        ASSERT(index < _fieldCount);
        return _currentRow[index];
    }

protected:
    uint64 _rowCount;
    Field* _currentRow;
    uint32 _fieldCount;

private:
    void CleanUp();
    MYSQL_RES* _result;
    MYSQL_FIELD* _fields;

    ResultSet(ResultSet const& right) = delete;
    ResultSet& operator=(ResultSet const& right) = delete;
};

typedef std::shared_ptr<ResultSet> QueryResult;

class WorldDatabaseWorkerPool
{
public:
    QueryResult Query(const char* query)
    {
        return QueryResult();
    }
};

extern WorldDatabaseWorkerPool WorldDatabase;
#define sTransportMgr TransportMgr::instance()

class TransportMgr
{
public:
    static TransportMgr* instance()
    {
        return nullptr;
    }

    void AddPathNodeToTransport(uint32 transportEntry, uint32 timeSeg, TransportAnimationEntry const* node)
    {
    }

    void AddPathRotationToTransport(uint32 transportEntry, uint32 timeSeg, TransportRotationEntry const* node)
    {

    }
};