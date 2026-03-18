#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "../DamageTracker/DmgFlags.h"
#include "../core_modules/ActorUtils.h"
#include "../core_modules/CombatData.h"

using namespace nlohmann;

class ConfidenceChecks
{
public:
	//ConfidenceChecks();
	//~ConfidenceChecks();
	void returnOriginalConfidence(std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord);
	void returnSingleOriginalConfidence
	(const RE::FormID formID, float& originalConfidence, float& modifiedConfidence);
	void evaluate(
		const FlagSet& dmgFlags, 
		std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord);

private:
	void cacheConfidence(std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord);
	bool cacheCheck = false;
	void ConfidenceDown(
		std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord
		, bool isPlayerWerewolf, bool isPlayerVampireLord
	);
	void updateCFDDownChances();

	struct confidenceDownChance
	{
		int leader = 0;
		int vanguard = 0;
		int ranger = 0;
		int caster = 0;
	} cfdDownChances;
	int globalConfidenceDownChance = -1;
	json rolesInfo = json::object();
	//std::unordered_map<RE::FormID, CombatData::npcCombatInfo> combatRecord;
};