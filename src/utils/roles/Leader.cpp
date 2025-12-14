#include "Leader.h"
#include "RE/Skyrim.h"

void Leader::WriteDefaultProfileToJSON(nlohmann::json& j)
{
	j["roles"]["Leader"]["offensive"] = 0.3f;
	j["roles"]["Leader"]["defensive"] = 0.5f;
	j["roles"]["Leader"]["grpOffensive"] = 0.4f;
	j["roles"]["Leader"]["circle"] = 0.05f;
	j["roles"]["Leader"]["flank"] = 0.45f;
	j["roles"]["Leader"]["stalk"] = 0.35f;
}


void Leader::Execute(RE::Actor* actor)
{
	//hostile to player check
	if (!actor || !actor->IsHostileToActor(RE::PlayerCharacter::GetSingleton())) {
		return;
	}

	//gets and checks if player is valid
	const auto player = RE::PlayerCharacter::GetSingleton();
	if (!player) {
		return;
	}

	CONSOLE_LOG("The NPC {} is executing Leader behavior.", actor->GetDisplayFullName());

}

bool comparator(RE::Actor* a, RE::Actor* b)
{
    const auto avA = a->AsActorValueOwner();
    const auto avB = b->AsActorValueOwner();

    float levelA = a->GetLevel();
    float levelB = b->GetLevel();
    if (levelA != levelB)
        return levelA > levelB;

    float armorA = avA->GetActorValue(RE::ActorValue::kDamageResist);
    float armorB = avB->GetActorValue(RE::ActorValue::kDamageResist);
    if (armorA != armorB)
        return armorA > armorB;

    auto weaponA = a->GetEquippedObject(true);
    auto weaponB = b->GetEquippedObject(true);

    float dmgA = 0.0f;
    float dmgB = 0.0f;
    if (weaponA)
    {
        if (auto weap = weaponA->As<RE::TESObjectWEAP>())
        {
            dmgA = weap->attackDamage;
        }
    }
    if (weaponB)
    {
        if (auto weap = weaponB->As<RE::TESObjectWEAP>())
        {
            dmgB = weap->attackDamage;
        }
    }
    return dmgA > dmgB;
}

bool Leader::AssignRole(RE::Actor*& actor, std::unordered_map<RE::FormID, char>& assignedNPCs, RE::Actor*& leader)
{
	if (!leader || comparator(actor, leader)) {
		leader = actor;
		assignedNPCs[leader->GetFormID()] = 'L';
		return true;
	}//thanks for the suggested code, DavidJCobb

	return false;
}
