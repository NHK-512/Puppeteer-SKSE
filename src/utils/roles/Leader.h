#pragma once
#include <nlohmann/json.hpp>
#include "../cmbStl_modules/CombatStyleManager.h"
#include "../cmbStl_modules/CombatStyleProfiles.h"
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())

namespace Leader
{
	void WriteDefaultProfileToJSON(nlohmann::json& j);
	void GetCombatProfile(combatStyleProf::mults& prof);
	void Execute(RE::Actor* actor);
}