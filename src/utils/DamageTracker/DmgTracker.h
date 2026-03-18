#pragma once
#include <unordered_map>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DmgFlags.h"
#include "../core_modules/CombatData.h"
#include "../core_modules/FileLoaders/ConfigLoader.h"

#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

class dmgTracker
{
public:
	dmgTracker();
	~dmgTracker();

	void UpdateList(const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& enemies);
	void Tick(FlagSet& outputFlags, RE::PlayerCharacter*& player);

private:
	struct HPperNPC
	{
		float lastHealth = -1.0f;
		bool hasTakenDamage = false;
		//bool isDead = false;

		std::optional<std::chrono::steady_clock::time_point> firstHitTime;
	};

	std::unordered_map<RE::FormID, HPperNPC> healthPool;
	int instantKillTime = 0;
	bool instantKillDetection(RE::Actor* actor, HPperNPC& HPrecord); 

	int hesitateDuration = 0;
	std::chrono::steady_clock::time_point hesitationLastTick;

	bool isPlayerWerewolfOnce = false;
	bool isPlayerVampireLordOnce = false;
	void isPlayerVampireLordOrWerewolf(FlagSet& outputFlags, RE::PlayerCharacter*& player);
};

