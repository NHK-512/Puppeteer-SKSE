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

    //Used to store damage data at the end of battle. Is being referenced by two or more files.
    struct dmgData
    {
        float totalDmgTaken = 0.0f;
        float totalDmgDealt = 0.0f;

        float physical = 0.0f;
        float magic = 0.0f;

        float range = 0.0f;
        float melee = 0.0f;
    };
}