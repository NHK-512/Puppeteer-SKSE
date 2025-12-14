#pragma once
#include <nlohmann/json.hpp>
#include "../cmbStl_modules/CombatStyleManager.h"
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace Vanguard
{
	void WriteDefaultProfileToJSON(nlohmann::json& j);
	void GetCombatProfile(combatStyleProf::mults& prof);
	bool AssignRole(RE::Actor*& actor, std::unordered_map<RE::FormID, char>& assignedNPCs);
}