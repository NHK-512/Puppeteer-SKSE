#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

//#define CONSOLE_LOG RE::ConsoleLog::GetSingleton()->Print
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(fmt::format(__VA_ARGS__).c_str())


namespace AIManager
{
    void Initialize();
    void ModifyAI(RE::Actor* actor);
}
