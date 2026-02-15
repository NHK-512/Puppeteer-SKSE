#pragma once

#include "roles/Leader.h"
#include "roles/Ranger.h"
#include "roles/Vanguard.h"
#include "roles/Striker.h"
#include "roles/Caster.h"
#include "core_modules/EnemyScanner.h"
#include "core_modules/ActorUtils.h"
#include "core_modules/ConsoleUtils.h"
#include "core_modules/FileLoaders/ConfigLoader.h"

#include <unordered_map>
#include <vector>
#include <random>
#include <algorithm>

static bool vangSwitch = false;
static RE::Actor* tempRanger;

namespace Puppeteer 
{
	struct survivalTime
	{
		RE::FormID formID;
		int seconds;
		bool isAttacked;
	};

	/*enum class CombatSignal
	{
		InstantKill,
		PlayerUsingMagic,
		PlayerUsingRanged,
		LeaderDown,
		RangerThreatened,
	};*/

	static std::vector<survivalTime> survivalTimes;

	void AssignRoles(
		const std::vector<RE::FormID>& npcIDs, 
		std::unordered_map<RE::FormID, char>& assignedNPCs);
	void rangerKeepDistance(const std::unordered_map<RE::FormID, char>& roles, RE::PlayerCharacter* player);
	void executeTactics(std::unordered_map<RE::FormID, char>& roles, RE::PlayerCharacter* a_player);
}