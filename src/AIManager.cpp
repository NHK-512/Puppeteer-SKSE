#include "AIManager.h"
#include "utils/EnemyScanner.h"

void AIManager::Initialize()
{
    SKSE::log::info("AI Manager Initialized.");
    auto* player = RE::PlayerCharacter::GetSingleton();
    AIManager::ModifyAI(player);
}

void AIManager::ModifyAI(RE::Actor* actor)
{
    if (!actor || !actor->IsPlayerRef())
        return;

    const auto enemies = EnemyScanner::GetHostileNPCsNearPlayer();
    for (auto formID : enemies) {
        SKSE::log::info("Hostile nearby NPC FormID: {:X}", formID);
    }
}

//if (!actor || !actor->Is3DLoaded())
//    return;

//// Just as an example, print the actor's name
//const auto name = actor->GetDisplayFullName();
//SKSE::log::info("AI ModifyAI called on actor: {}", name);

//// Example: Detect allies nearby within a radius (simplified)
//constexpr float radius = 1000.0f;
//auto* cell = actor->GetParentCell();
//if (!cell)
//    return;

//for (auto& ref : cell->GetReferences())
//{
//    auto* other = ref.get().As<RE::Actor>();
//    if (!other || other == actor || !other->IsHostileToActor(actor))
//        continue;

//    //auto message = ;
//    float dist = actor->GetPosition().GetDistance(other->GetPosition());
//    if (dist < radius)
//    {
//        SKSE::log::info("{} sees nearby enemy : {}", name, other->GetDisplayFullName());
//        // Here you'd add actual coordination logic
//    }
//}