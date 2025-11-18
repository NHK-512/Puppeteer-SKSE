#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <unordered_map>

#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())

namespace consoleUtils
{
	bool TriggerOnce(const char* key, bool condition);
}