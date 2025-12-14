#pragma once
#include <nlohmann/json.hpp>
#include "../cmbStl_modules/CombatStyleManager.h"
#include "../cmbStl_modules/CombatStyleProfiles.h"
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace Caster
{
	void WriteDefaultProfileToJSON(nlohmann::json& j);
	bool AssignRole(RE::Actor*& actor, std::unordered_map<RE::FormID, char>& assignedNPCs);
}