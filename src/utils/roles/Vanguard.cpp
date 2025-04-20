// roles/Vanguard.cpp
#include "Vanguard.h"
#include <RE/Skyrim.h>

void Vanguard::Assign(RE::Actor* npc) 
{
    if (npc) 
    {
        auto id = npc->GetFormID();
        CONSOLE_LOG("The NPC {:X} is assigned with Vanguard.", id);
    }
}
