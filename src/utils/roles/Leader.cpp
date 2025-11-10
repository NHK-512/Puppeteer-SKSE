#include "Leader.h"
#include "RE/Skyrim.h"

void Leader::GetCombatProfile(combatStyleProf::mults& prof)
{
	prof.offensive	= 0.3f;
	prof.defensive	= 0.5f;
	prof.grpOffensive	= 0.4f;
	prof.circle	= 0.05f;
	prof.flank	= 0.45f;
	prof.stalk	= 0.35f;
}

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