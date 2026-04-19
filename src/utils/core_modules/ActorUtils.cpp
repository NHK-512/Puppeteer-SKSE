#include "ActorUtils.h"


RE::Actor* ActorUtils::getClosestActorToActor(RE::Actor* targetActor, const std::vector<RE::Actor*>& otherActors)
{
	auto fromPos = targetActor->GetPosition();
	RE::NiPoint3 toPos;
	float dist = FLT_MAX;
	RE::Actor* closestActor = *otherActors.begin();

	for (int i = 0; i < otherActors.size(); i++)
	{
		if (!otherActors[i])
			continue;

		toPos = otherActors[i]->GetPosition();
		if (fromPos.GetDistance(toPos) < dist)
		{
			dist = fromPos.GetDistance(toPos);
			closestActor = otherActors[i];
		}
	}

	return closestActor;
}

std::vector<RE::Actor*> ActorUtils::extractActorsFromRoles(
	//const std::unordered_map<RE::FormID, char>& roles, 
	const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& roles,
	char roleType)
{
	std::vector<RE::Actor*> outVct;
	RE::Actor* actor;

	if (roles.size() <= 1)
		return outVct;

	for (auto i = roles.begin(); i != roles.end(); i++)
	{
		if (i->second.role == roleType)
		{
			actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
			if (actor &&
				actor->Is3DLoaded())
				outVct.push_back(actor);
		}
	}

	return outVct;
}

bool IsValidForDelete(RE::Actor* actor)
{
	if (actor)
	{
		auto base = actor->GetActorBase();
		auto state = actor->AsActorState();
		if (base) {
			if (base->IsEssential() ||	// Only allies can kill them
				base->IsProtected())	// Only player cannot kill them, but enemies can
			{
				return (state && state->IsBleedingOut()) || actor->IsInKillMove();
			}
		}
	}

	return false;
}

void ActorUtils::DeadActorsCleanup(
	std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord,
	ConfidenceChecks& CFDMananager,
	CombatStyleManager& CSManager,
	std::unordered_map<char, int>& deathCount,
	bool IsInCombat
)
{
	if (!IsInCombat )
	{
		if (!combatRecord.empty())
			combatRecord.clear();
		return;
	}

	if(combatRecord.empty()) //if still in combat but list is empty
		return;

	for(auto i = combatRecord.begin(); i != combatRecord.end(); i++)
	{
		auto actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
		if (actor &&
			(actor->IsDead() ||IsValidForDelete(actor))
		)
		{
			//Returns Confidence stat
			CFDMananager.returnSingleOriginalConfidence
			(	i->first
			,	i->second.originalConfidence
			,	i->second.modifiedConfidence
			);

			//Returns original Combat Styles
			CSManager.ReturnCachedSingle(combatRecord, i->first);

			//Logs role specific deaths
			switch (i->second.role)
			{
			case 'R':
				if (deathCount.contains('R'))
					deathCount['R']++;
				break;
			case 'V':
				if (deathCount.contains('V'))
					deathCount['V']++;
				break;
			case 'C':
				if (deathCount.contains('C'))
					deathCount['C']++;
				break;
			default:
				break;
			}

			//Failsafe check to ensure deletion from Record
			if(combatRecord.contains(i->first))
				combatRecord.erase(i);
		}
	}
}

int randomInt(int min, int max)
{
	return (std::rand() % (max - min + 1)) + min;
}

std::pair<int, bool> ActorUtils::diceRollOnChance(int percentChance)
{
	//If the random number falls below the percentChance -> success
	int randnum = (randomInt(0, 100) % 100);
	if (randnum < percentChance)
		return std::make_pair(randnum, true);

	return std::make_pair(randnum, false);
}

RE::ActorValueOwner* ActorUtils::GetActorValue(RE::FormID formID)
{
	auto* form = RE::TESForm::LookupByID(formID);
	if (!form) return nullptr;
	auto actor = form->As<RE::Actor>();
	if (!actor) return nullptr;
	if (!actor->Is3DLoaded() && actor->IsDead())	return nullptr;
	return actor->AsActorValueOwner();
}

bool ActorUtils::isEnemySilverHand(RE::FormID formID)
{
	auto* form = RE::TESForm::LookupByID(formID);
	if (!form) return false;
	auto actor = form->As<RE::Actor>();
	if (!actor) return false;
	if (!actor->Is3DLoaded() && actor->IsDead())	return false;

	if (std::strstr(actor->GetDisplayFullName(), "Silver Hand") == nullptr)
		return false;

	return true;
}

bool ActorUtils::isEnemyVampire(RE::FormID formID)
{
	auto* form = RE::TESForm::LookupByID(formID);
	if (!form) return false;
	auto actor = form->As<RE::Actor>();
	if (!actor) return false;
	if (!actor->Is3DLoaded() && actor->IsDead())	return false;

	if (std::strstr(actor->GetDisplayFullName(), "Vampire") == nullptr)
		return false;

	return true;
}

float ActorUtils::GetDistanceFurthestVanguard(
	const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& roles
,	RE::PlayerCharacter*& player
)
{
	float maxDistance = 0.0f;
	for (const auto& npc : roles)
	{
		//Only include Vanguards
		if (npc.second.role != 'V') continue;
		auto* form = RE::TESForm::LookupByID(npc.first);
		if (!form) continue;
		auto actor = form->As<RE::Actor>();
		if (!actor) continue;
		if (actor->IsDead())	continue;

		float dist = ActorUtils::GetDistanceBetweenTargets(actor, player);
		if (dist > maxDistance) maxDistance = dist;
	}

	return maxDistance;
}

RE::AIProcess* GetAIProcess(RE::Actor* a_actor)
{
	if (!a_actor)	return nullptr;

	auto& runtime = a_actor->GetActorRuntimeData();

	auto process = runtime.currentProcess;
	if (!process)	return nullptr;

	return process;
}

RE::HitData* ActorUtils::GetLastHitData(RE::Actor* a_actor)
{
	auto process = GetAIProcess(a_actor);

	auto middleHigh = process->middleHigh;
	if (!middleHigh)	return nullptr;

	auto hitData = middleHigh->lastHitData;
	if (!hitData)	return nullptr;

	return hitData;
}

RE::AttackData* ActorUtils::GetAttackData(RE::Actor* a_actor)
{
	auto process = GetAIProcess(a_actor);

	auto high = process->high;
	if (!high)	return nullptr;

	auto atkData = high->attackData.get();
	if (!atkData)	return nullptr;

	return &atkData->data;
}

float ActorUtils::GetDistanceBetweenTargets(RE::Actor* a_self, RE::Actor* a_target)
{
	if (!a_self) return 0.0f;
	if (!a_target) return 0.0f;
	auto selfPosition = a_self->GetPosition();
	
	return a_target->GetPosition().GetDistance(selfPosition);
}

bool ActorUtils::isWithinMeleeRange(RE::Actor* a_self, RE::Actor* a_target, const float& meleeDist)
{
	if (!a_self)	return false;
	if (!a_target)	return false;

	//if distance is greater than melee distance
	if (GetDistanceBetweenTargets(a_self, a_target) > meleeDist)
		return false;

	return true;
}

bool ActorUtils::isPlayerHPat0(RE::PlayerCharacter* a_player)
{
	if (!a_player)	return false;

	auto actorValue = GetActorValue(a_player->GetFormID());
	auto hp = actorValue->GetActorValue(RE::ActorValue::kHealth);
	if (hp == 0.0f)	return true;

	return false;
}
