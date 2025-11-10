// roles/Ranger.cpp
#include "Ranger.h"
#include <thread>
#include <chrono>
#include <RE/Skyrim.h>

void Ranger::GetCombatProfile(combatStyleProf::mults& prof)
{
	prof.grpOffensive = 0.5f;
	prof.fallback = 0.8f;
	prof.strafe = 0.8f;
}

void Ranger::WriteDefaultProfileToJSON(nlohmann::json& j)
{
    j["roles"]["Ranger"]["grpOffensive"] = 0.5f;
    j["roles"]["Ranger"]["fallback"] = 0.8f;
    j["roles"]["Ranger"]["strafe"] = 0.8f;
}

void Ranger::KeepDistanceAwayPlayer(RE::Actor* ranger, const std::vector<RE::Actor*>& vanguards, RE::Actor* player)
{
    if (!player || vanguards.empty() || !ranger) return;	

    const auto playerPos = player->GetPosition();

    // Find the furthest vanguard distance
    float maxVanguardDistance = 0.0f;
    for (auto* vg : vanguards) {
        if (!vg) continue;
        float dist = vg->GetPosition().GetDistance(playerPos);
        if (dist > maxVanguardDistance)
            maxVanguardDistance = dist;
    }

    // Check each ranger
    float dist = ranger->GetPosition().GetDistance(playerPos);
	if (dist < maxVanguardDistance) {
		ranger->InitiateFlee(player, 0, 1, 1, nullptr, nullptr, maxVanguardDistance * 10, maxVanguardDistance * 15);
	}
}