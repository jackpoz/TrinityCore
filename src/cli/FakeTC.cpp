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

void SpellMgr::SetSpellDifficultyId(uint32 spellId, uint32 id)
{
}
