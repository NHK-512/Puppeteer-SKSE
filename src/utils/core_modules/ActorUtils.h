#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
//#include "../cmbStl_modules/CombatStyleManager.h"
#include "../cmbStl_modules/CombatStyleProfiles.h"
#include <vector>
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace CombatStyleManager
{
    struct profileCollection;
}

namespace ActorUtils {
    //Extracts a certain type of actor (L, V, R) from map of roles
    std::vector<RE::Actor*> extractActorsFromRoles(
        const std::unordered_map<RE::FormID, char>& roles, 
        char roleType);
    //Flushes dead actors from list and return original combat styles
    void DeadActorsCleanup(
        std::unordered_map<RE::FormID, char>& roles,
        CombatStyleManager::profileCollection& collection,
        bool IsInCombat
    );
    //Gets a RE::Actor base on a target actor
    RE::Actor* getClosestActorToActor(RE::Actor* targetActor, const std::vector<RE::Actor*>& otherActors);
    //Flashes a multiplier up a certain amount for a certain time
    void flashMultiplier(const std::unordered_map<RE::FormID, char>& roles, json flashParams);
        //, int second, int duration);

    bool dmgTaken(RE::Actor* npc);
}