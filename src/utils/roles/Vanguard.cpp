// roles/Vanguard.cpp
#include "Vanguard.h"
#include <RE/Skyrim.h>

void Vanguard::Assign(RE::Actor* npc) {
    if (npc) {
        auto id = npc->GetFormID();
        RE::ConsoleLog::GetSingleton()->Print(fmt::format("The NPC {:X} is assigned with Vanguard.", id).c_str());
    }
}
