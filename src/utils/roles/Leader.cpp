#include "Leader.h"
#include <RE/Skyrim.h>

void Leader::Assign(RE::Actor* npc)
{
	if (npc)
	{
		auto id = npc->GetFormID();
		CONSOLE_LOG("The NPC {:X} is assigned with Leader.", id);
	}
}