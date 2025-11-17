#pragma once
#include <nlohmann/json.hpp>
#include "../cmbStl_modules/CombatStyleManager.h"
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())

namespace Vanguard
{
	void WriteDefaultProfileToJSON(nlohmann::json& j);
	void GetCombatProfile(combatStyleProf::mults& prof);
}