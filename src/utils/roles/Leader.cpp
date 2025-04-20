#include "Leader.h"
#include <RE/Skyrim.h>

void Leader::Assign(RE::Actor* npc)
{
	if (npc)
	{
		//auto id = npc->GetFormID();
		CONSOLE_LOG("The NPC {} is assigned with Leader.", npc->GetDisplayFullName());
	}
}