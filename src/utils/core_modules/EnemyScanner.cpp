#include "EnemyScanner.h"

std::vector<RE::FormID> EnemyScanner::GetHostileNPCsNearPlayer(float radius)
{
    std::vector<RE::FormID> hostileIDs;

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player)
        return hostileIDs;

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

        const auto dist = playerPos.GetDistance(actor->GetPosition());
        if (dist <= radius) {
            hostileIDs.push_back(actor->GetFormID());

            /*SKSE::log::info("Detected hostile NPC: {} (FormID: {:X}) at distance {:.2f}",
                actor->GetDisplayFullName(),
                actor->GetFormID(),
                dist
            );

            CONSOLE_LOG("Detected hostile NPC: {} (FormID: {:X}) at distance {:.2f}",
                actor->GetDisplayFullName(),
                actor->GetFormID(),
                dist
            );*/
        }
    }

    return hostileIDs;
}