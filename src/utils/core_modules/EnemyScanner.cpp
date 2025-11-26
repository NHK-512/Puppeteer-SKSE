#include "EnemyScanner.h"

//std::vector<RE::FormID> 
void EnemyScanner::GetHostileNPCsNearPlayer(float radius, std::vector<RE::FormID>& enemies)
{
    //std::vector<RE::FormID> hostileIDs;

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player)
        return;// hostileIDs;

    const auto playerPos = player->GetPosition();

    // Iterate over all loaded actors (high, low, middle processes)
    for (auto* actor : GetNearbyAggroedActors(radius))
    {
        if (!actor || actor == player)
            continue;

        if (!actor->GetRace()->GetPlayable())
            continue;

        if (!actor->IsHostileToActor(player))
            continue;

        if (actor->IsDead())
            continue;

        if (actor->AsActorState()->IsBleedingOut())
            continue;

        if (actor->IsInKillMove())
            continue;

        const auto dist = playerPos.GetDistance(actor->GetPosition());
        if (dist <= radius) {
            enemies.push_back(actor->GetFormID());
        }
    }

    //return hostileIDs;
}