#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "utils/core_modules/EnemyScanner.h"
#include "utils/core_modules/ConfigLoader.h"
#include "utils/core_modules/ConsoleUtils.h"
#include "utils/Puppeteer.h"
#include "utils/cmbStl_modules/CombatStyleManager.h"

#include <algorithm>
#include <format>

#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())

static std::unordered_map<RE::FormID, char> currentRoles;
static RE::FormID currentLeaderID = 0;
static std::vector<RE::FormID> enemies;
static std::vector<RE::FormID> previousEnemies;
static std::unordered_map<RE::FormID, RE::TESCombatStyle*> cacheCmbStl;

//input number is max distance as radius of the scanning area
//IMPORTANT: MAX DISTANCE AS TO BE HIGH (MINIMUM ~500)
static float scanDistance = 0;
//how many seconds that the cylce of the loop run for
static int secondsPerCycle = 0;
//Minimum amount of NPCs for Puppeteer to run
static int minimumActors = 0;
//static int cycleCount = 0;
static bool shutoffPuppeteer = false;
static RE::Actor* leader;
static RE::Actor* leaderCache;
static int countSinceLeaderDeath = 0;
static int maxSkipCycles = 0;
//necessary for the loop not constantly running the functions when combat ends
static bool wasInCombat = false;
static CombatStyleManager::profileCollection profCollection;


namespace AIManager
{
	void Initialize();
}
