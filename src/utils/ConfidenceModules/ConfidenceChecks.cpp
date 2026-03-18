#include "ConfidenceChecks.h"

void ConfidenceChecks::evaluate(
	const FlagSet& dmgFlags,
	std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord)
{
	if (!cacheCheck)	cacheConfidence(combatRecord);
	updateCFDDownChances();

	//For logging which flag is raised
	if (dmgFlags[Flag::IsInstantKilled] || dmgFlags[Flag::IsPlayerVampireLord] || dmgFlags[Flag::IsPlayerWerewolf])
	{
		CONSOLE_LOG("[Puppeteer][Confidence Manager] Confidence Down triggered.");
		CONSOLE_LOG(
			"[Flags] Instant-killed: {} | Vampire: {} | Werewolf: {}"
			, dmgFlags[Flag::IsInstantKilled]
			, dmgFlags[Flag::IsPlayerVampireLord]
			, dmgFlags[Flag::IsPlayerWerewolf]
		);

		//ConfidenceDown(combatRecord);
		//Multiple Confidence Events can stack Confidence Checks
		for (size_t i = 0; i < dmgFlags.data.count(); ++i)
		{
			ConfidenceDown(
				combatRecord
				, dmgFlags[Flag::IsPlayerWerewolf]
				, dmgFlags[Flag::IsPlayerVampireLord]
			);
		}
	}
}

void ConfidenceChecks::cacheConfidence
(std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord)
{
	if (combatRecord.empty())
		return;

	for (auto& npc : combatRecord)
	{
		auto av = ActorUtils::GetActorValue(npc.first);
		npc.second.originalConfidence = av->GetActorValue(RE::ActorValue::kConfidence);
		npc.second.modifiedConfidence = npc.second.originalConfidence;
	}

	cacheCheck = true;
}

void ConfidenceChecks::returnOriginalConfidence
(std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord)
{
	if (combatRecord.empty())
		return;

	for (auto& npc : combatRecord)
	{
		returnSingleOriginalConfidence
		(	npc.first
		,	npc.second.originalConfidence
		,	npc.second.modifiedConfidence
		);
	}

	cacheCheck = false;
}

void ConfidenceChecks::returnSingleOriginalConfidence
(const RE::FormID formID, float &originalConfidence, float &modifiedConfidence)
{
	auto av = ActorUtils::GetActorValue(formID);
	if (!av)	return;
	av->SetActorValue(RE::ActorValue::kConfidence, originalConfidence);
	originalConfidence = 0.0f;
	modifiedConfidence = 0.0f;
}

void ConfidenceChecks::ConfidenceDown(
	std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord
	, bool isPlayerWerewolf, bool isPlayerVampireLord
)
{

	if (combatRecord.empty())
		return;

	for (auto& npc : combatRecord)
	{
		//Skip if player is Vamp Lord and is fighting vampires
		if (isPlayerVampireLord)
			if (ActorUtils::isEnemyVampire(npc.first))
				continue;

		//Skip if the player is werewolf and is fighting Silver Hands
		if (isPlayerWerewolf)
			if (ActorUtils::isEnemySilverHand(npc.first))
				continue;

		auto av = ActorUtils::GetActorValue(npc.first);
		int chance = 0;
		switch (npc.second.role)
		{
		case 'L':
			chance = cfdDownChances.leader;
			break;
		case 'V':
			chance = cfdDownChances.vanguard;
			break;
		case 'R':
			chance = cfdDownChances.ranger;
			break;
		case 'C':
			chance = cfdDownChances.caster;
			break;
		default:
			break;
		}

		//CONSOLE_LOG("{}'s Chance: {}", RE::TESForm::LookupByID(npc.first)->As<RE::Actor>()->GetDisplayFullName(), chance);

		std::pair<int, bool> diceRoll = ActorUtils::diceRollOnChance(chance % 100);
		diceRoll.first = diceRoll.first > 100 ? diceRoll.first % 100 : diceRoll.first;

		//if the npc's rolled number falls below the chance, Confidence Down triggers
		if (diceRoll.second)
		{
			if (npc.second.modifiedConfidence > 0)
				npc.second.modifiedConfidence--;
			else
				continue;

			if (!av)	continue;
			av->SetActorValue(RE::ActorValue::kConfidence, npc.second.modifiedConfidence);
		}

		npc.second.confidenceCheckDiceRoll = diceRoll.first;
	}
}

void ConfidenceChecks::updateCFDDownChances()
{
	globalConfidenceDownChance = ConfigLoader::GetGlobalConfidenceDownChance();
	if (globalConfidenceDownChance > -1) //implies the user have defined a set number for this in the JSON
	{
		cfdDownChances.leader = globalConfidenceDownChance;
		cfdDownChances.ranger = globalConfidenceDownChance;
		cfdDownChances.vanguard = globalConfidenceDownChance;
		cfdDownChances.caster = globalConfidenceDownChance;

		return;
	}

	rolesInfo = ConfigLoader::GetRolesInfo();
	
	cfdDownChances.leader = rolesInfo["Leader"].value("ConfidenceDownChance", 30);
	cfdDownChances.ranger = rolesInfo["Ranger"].value("ConfidenceDownChance", 80);
	cfdDownChances.vanguard = rolesInfo["Vanguard"].value("ConfidenceDownChance", 50);
	cfdDownChances.caster = rolesInfo["Caster"].value("ConfidenceDownChance", 70);
}

