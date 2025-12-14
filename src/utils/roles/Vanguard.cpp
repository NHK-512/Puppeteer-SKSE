// roles/Vanguard.cpp
#include "Vanguard.h"
#include <RE/Skyrim.h>

void Vanguard::WriteDefaultProfileToJSON(nlohmann::json& j)
{
	j["roles"]["Vanguard"]["offensive"] = 0.08f;
	j["roles"]["Vanguard"]["defensive"] = 0.85f;
	j["roles"]["Vanguard"]["grpOffensive"] = 0.15f;
	j["roles"]["Vanguard"]["circle"] = 0.65f;
	j["roles"]["Vanguard"]["stalk"] = 0.05f;
}

bool Vanguard::AssignRole(RE::Actor*& actor, std::unordered_map<RE::FormID, char>& assignedNPCs)
{
    if (!actor) return false;

    const auto inv = actor->GetInventory();

    for (const auto& [item, data] : inv) {
        if (!item || !data.second) continue;
        if (data.first <= 0) continue;

        if (const auto armor = item->As<RE::TESObjectARMO>()) {
            if (armor->HasKeywordString("ArmorShield")) {
                //assignedVang.push_back(actor);
                assignedNPCs[actor->GetFormID()] = 'V';
                return true;
            }
        }

        if (const auto weap = item->As<RE::TESObjectWEAP>()) {
            if (weap->IsTwoHandedSword() || weap->IsTwoHandedAxe()) {
                //assignedVang.push_back(actor);
                assignedNPCs[actor->GetFormID()] = 'V';
                break;
            }
        }
    }

	return false;
}
