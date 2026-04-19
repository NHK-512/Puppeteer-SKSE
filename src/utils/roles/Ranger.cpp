#include "Ranger.h"
#include <RE/Skyrim.h>

void Ranger::WriteDefaultProfileToJSON(nlohmann::json& j)
{
    j["roles"]["Ranger"]["grpOffensive"] = 0.5f;
    j["roles"]["Ranger"]["fallback"] = 0.8f;
    j["roles"]["Ranger"]["strafe"] = 0.8f;
    j["roles"]["Ranger"]["ConfidenceDownChance"] = 80;
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
