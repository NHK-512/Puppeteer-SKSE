#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <vector>

namespace ActorUtils {
    //RE::NiPoint3 posInFrontOfPlayer(RE::PlayerCharacter player);

    //A check whether the player has taken damage from a certain npc
    bool dmgTaken(RE::PlayerCharacter* player, RE::Actor* npc);
    //Extracts a certain type of actor (L, V, R) from map of roles
    std::vector<RE::Actor*> extractActorsFromRoles(std::unordered_map<RE::FormID, char> roles, char roleType);
    //Removes dead actors in roles
    void FlushDeadActorsFromRoles(std::unordered_map<RE::FormID, char>& roles);
    //Gets a RE::Actor base on a target actor
    RE::Actor* getClosestActorToActor(RE::Actor* targetActor, std::vector<RE::Actor*> otherActors);
}