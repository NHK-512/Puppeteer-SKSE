#pragma once
#include <nlohmann/json.hpp>
//#include <fstream>
#include <chrono>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "../roles/Leader.h"
#include "../roles/Ranger.h"
#include "../roles/Vanguard.h"
#include "../roles/Striker.h"
#include "../core_modules/ConfigLoader.h"
#include "CombatStyleProfiles.h"
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace CombatStyleManager 
{
	const std::string filePath = "Data/SKSE/Plugins/PuppeteerConfig.json";
	static RE::TESCombatStyle* tmpStyle;
	
	struct profileCollection
	{
		std::unordered_map< RE::FormID, combatStyleProf::mults> original;
		std::unordered_map< RE::FormID, combatStyleProf::mults> modified;
		//std::unordered_map< RE::FormID,combatStyleProf::mults> final; //for profile blending step
	};

	//Asssign custom Combat Styles from list and cache original style into output
	void AssignAndCache(
		const std::unordered_map<RE::FormID, char>& roleList, 
		profileCollection& collection);
	//Accepts a cached list of combatants and revert their styles to original
	void ReturnCached(
		const std::unordered_map<RE::FormID, char>& currentRoles,
		profileCollection& collection
	);
	//Accepts a cached list of combatants and revert style of a singular actor
	void ReturnCachedSingle(std::unordered_map<RE::FormID, combatStyleProf::mults> &cachedList, 
		const RE::FormID deadForm);
}