#include "NearbyAggroedActors.h"

#include "RE/Skyrim.h"
#include "RE/P/PlayerCharacter.h"
#include "RE/P/ProcessLists.h"
#include "RE/A/Actor.h"
#include "RE/T/TESObjectREFR.h"
#include "RE/B/BSTArray.h"  

std::vector<RE::Actor*> GetNearbyAggroedActors(float maxDistance)
{
    std::vector<RE::Actor*> result;

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player) {
        return result;
    }

    const auto playerPos = player->GetPosition();
    auto* processLists = RE::ProcessLists::GetSingleton();
    if (!processLists) {
        return result;
    }

    auto checkHandleList = [&](const RE::BSTArray<RE::ActorHandle>& handles) 
    {
        for (auto handle : handles) {
            RE::Actor* actor = handle.get().get();
            if (!actor || actor == player || !actor->Is3DLoaded()) {
                continue;
            }

            // Distance check
            const auto actorPos = actor->GetPosition();
            float distance = playerPos.GetDistance(actorPos);
            if (distance > maxDistance) {
                continue;
            }

            // Aggro check
            if (actor->IsHostileToActor(player)) {
                result.push_back(actor);
            }
        }
    };

    checkHandleList(processLists->highActorHandles);
    checkHandleList(processLists->middleHighActorHandles);
    checkHandleList(processLists->middleLowActorHandles);
    checkHandleList(processLists->lowActorHandles);

    if (result.empty())
        CONSOLE_LOG("Resulting vector list of hostiles is empty!");
    else
        CONSOLE_LOG("Resulting vector list has entities inside! Returning!");

    return result;
}
