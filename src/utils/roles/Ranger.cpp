// roles/Ranger.cpp
#include "Ranger.h"
#include <RE/Skyrim.h>

void Ranger::Assign(RE::Actor* npc) {
    if (npc) {
        auto id = npc->GetFormID();
        RE::ConsoleLog::GetSingleton()->Print(fmt::format("The NPC {:X} is assigned with Ranger.", id).c_str());
    }
}
