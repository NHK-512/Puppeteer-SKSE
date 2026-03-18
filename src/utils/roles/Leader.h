#pragma once
#include "../core_modules/CombatData.h"
#include <nlohmann/json.hpp>
#include "../cmbStl_modules/CombatStyleManager.h"
#include "../cmbStl_modules/CombatStyleProfiles.h"
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace Leader
{
	void WriteDefaultProfileToJSON(nlohmann::json& j);
	void Execute(RE::Actor* actor);
    bool AssignRole(
		RE::Actor*& actor, 
		std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& assignedNPCs,
		RE::Actor*& leader);
}