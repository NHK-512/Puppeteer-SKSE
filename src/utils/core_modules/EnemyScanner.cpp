#include "EnemyScanner.h"

void EnemyScanner::GetHostileNPCsNearPlayer(float radius, std::vector<RE::FormID>& enemies)
{
    if (!enemies.empty())
        enemies.clear();

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player)
        return;

    const auto playerPos = player->GetPosition();

    // Iterate over all loaded actors (high, low, middle processes)
    for (auto* actor : GetNearbyAggroedActors(radius))
    {
        if (!actor || actor == player)
            continue;

        if (!actor->GetRace()->GetPlayable() && 
            std::strstr(actor->GetDisplayFullName(),"Vampire") == nullptr) //support for vampires
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
        if (dist <= radius &&
            std::find(enemies.begin(), enemies.end(), actor->GetFormID()) == enemies.end())
        {
            enemies.push_back(actor->GetFormID());
        }
    }
}
