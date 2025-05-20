#pragma once

#include "roles/Leader.h"
#include "roles/Ranger.h"
#include "roles/Vanguard.h"
#include "roles/Striker.h"
#include "EnemyScanner.h"

#include <unordered_map>
#include <vector>
#include <random>

namespace Puppeteer 
{
	std::unordered_map<RE::FormID, char> AssignRoles(const std::vector<RE::FormID>& npcIDs);
}