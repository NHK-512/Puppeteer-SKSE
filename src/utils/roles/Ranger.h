#pragma once
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)
#include <nlohmann/json.hpp>
#include "../core_modules/ActorUtils.h"
#include "../cmbStl_modules/CombatStyleManager.h"
#include "../cmbStl_modules/CombatStyleProfiles.h"

namespace Ranger
{
	void WriteDefaultProfileToJSON(nlohmann::json& j);
	//Core function for rangers to prioritize distance
	void KeepDistanceAwayPlayer(RE::Actor* ranger, const std::vector<RE::Actor*>& vanguards, RE::Actor* player);
	void GetCombatProfile(combatStyleProf::mults& prof);
}