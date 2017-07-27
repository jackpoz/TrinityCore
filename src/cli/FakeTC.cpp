#include "FakeTC.h"

WorldDatabaseWorkerPool WorldDatabase;

bool ResultSet::NextRow()
{
    return false;
}

SpellMgr::SpellMgr()
{}

SpellMgr::~SpellMgr()
{}

SpellMgr* SpellMgr::instance()
{
    static SpellMgr instance;
    return &instance;
}

void SpellMgr::SetSpellDifficultyId(uint32 spellId, uint32 id)
{
}