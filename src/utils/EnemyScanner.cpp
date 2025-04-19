#include "EnemyScanner.h"
#include "SKSE/SKSE.h"
#include "NearbyAggroedActors.h"

std::vector<RE::FormID> EnemyScanner::GetHostileNPCsNearPlayer(float radius)
{
    //auto str_radius = (float)radius;
    CONSOLE_LOG("Input radius for plugin detection: {:.2f}", radius);

    std::vector<RE::FormID> hostileIDs;

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player)
        return hostileIDs;

    const auto playerPos = player->GetPosition();

    // Iterate over all loaded actors (high, low, middle processes)
    const auto& processLists = *RE::ProcessLists::GetSingleton();
    for (auto* actor : GetNearbyAggroedActors(radius))
    {
        if (!actor || actor == player)
            continue;

        if (!actor->IsHostileToActor(player))
            continue;

        const auto dist = playerPos.GetDistance(actor->GetPosition());
        if (dist <= radius) {
            hostileIDs.push_back(actor->GetFormID());

            SKSE::log::info("Detected hostile NPC: {} (FormID: {:X}) at distance {:.2f}",
                actor->GetDisplayFullName(),
                actor->GetFormID(),
                dist
            );

            CONSOLE_LOG("Detected hostile NPC: {} (FormID: {:X}) at distance {:.2f}",
                actor->GetDisplayFullName(),
                actor->GetFormID(),
                dist
            );
        }
        /*else
            CONSOLE_LOG("Hostile NPC detection for info is skipped!");*/
    }

    return hostileIDs;
}