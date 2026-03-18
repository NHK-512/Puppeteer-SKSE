#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "../core_modules/CombatData.h"
#include "../ConfidenceModules/ConfidenceChecks.h"
#include "../cmbStl_modules/CombatStyleProfiles.h"
#include "../cmbStl_modules/CombatStyleManager.h"
#include <vector>
#include <cstring>
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace ActorUtils {
    //Extracts a certain type of actor (L, V, R) from map of roles
    std::vector<RE::Actor*> extractActorsFromRoles(
        //const std::unordered_map<RE::FormID, char>& roles, 
        const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& roles,
        char roleType);

    //Flushes dead actors from list and return original combat styles
    void DeadActorsCleanup(
        std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord,
        ConfidenceChecks& CFDMananager,
        CombatStyleManager& CSManager,
        bool IsInCombat
    );

    //Gets a RE::Actor base on a target actor
    RE::Actor* getClosestActorToActor(RE::Actor* targetActor, const std::vector<RE::Actor*>& otherActors);

    std::pair<int, bool> diceRollOnChance(int percentChance);

    RE::ActorValueOwner* GetActorValue(RE::FormID formID);

    bool isEnemySilverHand(RE::FormID formID);
    bool isEnemyVampire(RE::FormID formID);
}