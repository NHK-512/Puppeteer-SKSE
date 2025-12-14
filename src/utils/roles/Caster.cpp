#include "Caster.h"

void Caster::WriteDefaultProfileToJSON(nlohmann::json& j)
{
	j["roles"]["Caster"]["offensive"] = 0.65f;
	j["roles"]["Caster"]["defensive"] = 0.2f;
	j["roles"]["Caster"]["grpOffensive"] = 0.65f;
	j["roles"]["Caster"]["avoidThreatChance"] = 0.9f;
	j["roles"]["Caster"]["circle"] = 0.8f;
	j["roles"]["Caster"]["flank"] = 0.25f;
	j["roles"]["Caster"]["fallback"] = 0.95f;
	j["roles"]["Caster"]["stalk"] = 0.6f;
}

bool Caster::AssignRole(RE::Actor*& actor, std::unordered_map<RE::FormID, char>& assignedNPCs)
{
	if (!actor)
		return false;

	auto* process = actor->GetActorRuntimeData().currentProcess;
	if (!process)
		return false;

	auto* left = process->GetEquippedLeftHand();
	auto* right = process->GetEquippedRightHand();

	if ((left && left->As<RE::SpellItem>()) ||
		(right && right->As<RE::SpellItem>()))
	{
		assignedNPCs[actor->GetFormID()] = 'C';
		return true;
	}
	
	return false;
}
