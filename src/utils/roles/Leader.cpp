#include "Leader.h"
#include "RE/Skyrim.h"

void Leader::Assign(RE::Actor* npc)
{
	if (npc)
	{
		CONSOLE_LOG("The NPC {} ({:X}) is assigned with Leader.", npc->GetDisplayFullName(), npc->GetFormID());
	}
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