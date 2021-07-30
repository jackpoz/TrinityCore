/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Player.h"
#include "SpellInfo.h"

bool Player::IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index, WorldObject const* caster) const
{
    // players are immune to taunt (the aura and the spell effect)
    if (spellInfo->Effects[index].IsAura(SPELL_AURA_MOD_TAUNT))
        return true;
    if (spellInfo->Effects[index].IsEffect(SPELL_EFFECT_ATTACK_ME))
        return true;

    return Unit::IsImmunedToSpellEffect(spellInfo, index, caster);
}

bool Player::HasItemFitToSpellRequirements(SpellInfo const* spellInfo, Item const* ignoreItem) const
{
    if (spellInfo->EquippedItemClass < 0)
        return true;

    // scan other equipped items for same requirements (mostly 2 daggers/etc)
    // for optimize check 2 used cases only
    switch (spellInfo->EquippedItemClass)
    {
    case ITEM_CLASS_WEAPON:
    {
        for (uint8 i = EQUIPMENT_SLOT_MAINHAND; i < EQUIPMENT_SLOT_TABARD; ++i)
            if (Item* item = GetUseableItemByPos(INVENTORY_SLOT_BAG_0, i))
                if (item != ignoreItem && item->IsFitToSpellRequirements(spellInfo))
                    return true;
        break;
    }
    case ITEM_CLASS_ARMOR:
    {
        // most used check: shield only
        if (spellInfo->EquippedItemSubClassMask & ((1 << ITEM_SUBCLASS_ARMOR_BUCKLER) | (1 << ITEM_SUBCLASS_ARMOR_SHIELD)))
        {
            if (Item* item = GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                if (item != ignoreItem && item->IsFitToSpellRequirements(spellInfo))
                    return true;

            // special check to filter things like Shield Wall, the aura is not permanent and must stay even without required item
            if (!spellInfo->IsPassive())
            {
                for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                    if (spellInfo->Effects[i].IsAura())
                        return true;
            }
        }

        // tabard not have dependent spells
        for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_MAINHAND; ++i)
            if (Item* item = GetUseableItemByPos(INVENTORY_SLOT_BAG_0, i))
                if (item != ignoreItem && item->IsFitToSpellRequirements(spellInfo))
                    return true;

        // ranged slot can have some armor subclasses
        if (Item* item = GetUseableItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
            if (item != ignoreItem && item->IsFitToSpellRequirements(spellInfo))
                return true;
        break;
    }
    default:
        TC_LOG_ERROR("entities.player", "Player::HasItemFitToSpellRequirements: Not handled spell requirement for item class %u", spellInfo->EquippedItemClass);
        break;
    }

    return false;
}

void Player::SendSupercededSpell(uint32 oldSpell, uint32 newSpell) const
{
    WorldPacket data(SMSG_SUPERCEDED_SPELL, 8);
    data << uint32(oldSpell) << uint32(newSpell);
    SendDirectMessage(&data);
}
