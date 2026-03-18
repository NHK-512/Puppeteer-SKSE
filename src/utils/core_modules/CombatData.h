#pragma once
#include "../cmbStl_modules/CombatStyleProfiles.h"

namespace CombatData
{
    //made to replace CurrentRoles and profCollection
    struct npcCombatInfo
    {
        //RE::FormID formID = 0x0;
        char role;
        combatStyleProf::mults original;
        combatStyleProf::mults modified;
        float originalConfidence = 0.0f;
        float modifiedConfidence = 0.0f;
        int confidenceCheckDiceRoll = 0;
    };
}