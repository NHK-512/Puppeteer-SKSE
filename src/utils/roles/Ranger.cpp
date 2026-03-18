// roles/Ranger.cpp
//#include <thread>
//#include <chrono>
#include "Ranger.h"
#include <RE/Skyrim.h>

void Ranger::WriteDefaultProfileToJSON(nlohmann::json& j)
{
    j["roles"]["Ranger"]["grpOffensive"] = 0.5f;
    j["roles"]["Ranger"]["fallback"] = 0.8f;
    j["roles"]["Ranger"]["strafe"] = 0.8f;
    j["roles"]["Ranger"]["ConfidenceDownChance"] = 0.8;
}

void Ranger::KeepDistanceAwayPlayer(RE::Actor* ranger, const std::vector<RE::Actor*>& vanguards, RE::PlayerCharacter*& player)
{
    if (!player || vanguards.empty() || !ranger) return;	

    auto playerPos = player->GetPosition();

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
		ranger->InitiateFlee(player, 1, 1, 1, nullptr, nullptr, maxVanguardDistance * 10, maxVanguardDistance * 15);
	}
}

bool Ranger::AssignRole(RE::Actor*& actor, std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& assignedNPCs)
{
    const auto inv = actor->GetInventory();
    for (const auto& [item, data] : inv) {
        if (!item || !data.second) continue;
        if (data.first <= 0) continue;

        if (const auto weap = item->As<RE::TESObjectWEAP>()) {
            if (weap->HasKeywordString("WeapTypeBow") || weap->HasKeywordString("WeapTypeCrossbow"))
            {
                assignedNPCs[actor->GetFormID()].role = 'R';
                return true;
            }
        }
    }

    return false;
}
