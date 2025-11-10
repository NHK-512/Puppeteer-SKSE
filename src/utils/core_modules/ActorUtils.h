#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "../cmbStl_modules/CombatStyleManager.h"
#include "../cmbStl_modules/CombatStyleProfiles.h"
#include <vector>
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(fmt::format(__VA_ARGS__).c_str())

namespace ActorUtils {
    //RE::NiPoint3 posInFrontOfPlayer(RE::PlayerCharacter player);

    //A check whether the player has taken damage from a certain npc
    bool dmgTaken(RE::PlayerCharacter* player, RE::Actor* npc);
    //Extracts a certain type of actor (L, V, R) from map of roles
    std::vector<RE::Actor*> extractActorsFromRoles(std::unordered_map<RE::FormID, char> roles, char roleType);
    //Flushes dead actors from list and return original combat styles
    void DeadActorsCleanup(
        std::unordered_map<RE::FormID, char>& roles,
        std::unordered_map<RE::FormID, combatStyleProf::mults>& profiles,
        bool combatEnded
    );
    //Gets a RE::Actor base on a target actor
    RE::Actor* getClosestActorToActor(RE::Actor* targetActor, std::vector<RE::Actor*> otherActors);
    //Checks an a group combatstyle via console and form ID
    void checkGroupCombatStyle(std::unordered_map<RE::FormID, char> roles);

    //Get the current Combat Style, modify according to the inputted number, Set the CS
    void csGetModSet(RE::Actor* actor, int type, float newVal);
}