#pragma once
#include <unordered_map>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "DmgFlags.h"
#include "../core_modules/FileLoaders/ConfigLoader.h"

#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

class dmgTracker
{
public:
	dmgTracker(const std::vector<RE::FormID>& enemies);
	~dmgTracker();

	void UpdateList(const std::vector<RE::FormID>& enemies);
	void Tick(
		FlagSet& outputFlags, 
		std::chrono::steady_clock::time_point& now,
		const std::unordered_map< RE::FormID, combatStyleProf::mults>& modifiedCmbs);

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

	//0 to 1 -> hesitation starts; 1 -> 0 hesitation ends
	bool hesitated = false; 
	int hesitateDuration = 0;
	std::chrono::steady_clock::time_point hesitationLastTick;
	//void hesitationReaction(bool toggle, const std::unordered_map< RE::FormID, combatStyleProf::mults>& modifiedCmbs);
};

