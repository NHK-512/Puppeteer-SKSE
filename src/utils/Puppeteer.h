#pragma once

#include "roles/Leader.h"
#include "roles/Ranger.h"
#include "roles/Vanguard.h"
#include "roles/Striker.h"
#include "core_modules/EnemyScanner.h"
#include "core_modules/ActorUtils.h"
#include "core_modules/ConsoleUtils.h"

#include <unordered_map>
#include <vector>
#include <random>
#include <algorithm>

static bool vangSwitch = false;
static RE::Actor* tempRanger;


namespace Puppeteer 
{
	//std::unordered_map<RE::FormID, char> 
	void AssignRoles(
		const std::vector<RE::FormID>& npcIDs, 
		std::unordered_map<RE::FormID, char>& assignedNPCs);
	void rangerKeepDistance(const std::unordered_map<RE::FormID, char>& roles, RE::PlayerCharacter* player);
	void Listen(std::unordered_map<RE::FormID, char>& roles, int cycleTime);
}