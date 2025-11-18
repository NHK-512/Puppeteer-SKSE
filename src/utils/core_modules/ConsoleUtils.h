#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <unordered_map>

#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())

static RE::ObjectRefHandle cachedTarget;
namespace consoleUtils
{
	bool TriggerOnce(const char* key, bool condition);
	void inspectCBStyleOfSelected(std::unordered_map<RE::FormID, char> roleList);
}