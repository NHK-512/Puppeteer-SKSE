// roles/Striker.cpp
#include "Striker.h"
#include <RE/Skyrim.h>

void Striker::Assign(RE::Actor* npc) 
{
    if (npc) 
    {
        auto id = npc->GetFormID();
        CONSOLE_LOG("The NPC {} is assigned with Striker.", npc->GetDisplayFullName());
    }
}
