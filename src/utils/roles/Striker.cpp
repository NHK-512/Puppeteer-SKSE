// roles/Striker.cpp
#include "Striker.h"
#include <RE/Skyrim.h>

void Striker::Assign(RE::Actor* npc) {
    if (npc) {
        auto id = npc->GetFormID();
        RE::ConsoleLog::GetSingleton()->Print(fmt::format("The NPC {:X} is assigned with Striker.", id).c_str());
    }
}
