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
			CFDMananager.returnSingleOriginalConfidence
			(	i->first
			,	i->second.originalConfidence
			,	i->second.modifiedConfidence
			);
			CSManager.ReturnCachedSingle(combatRecord, i->first);

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
