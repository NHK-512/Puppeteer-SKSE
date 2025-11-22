#include "ActorUtils.h"


bool ActorUtils::dmgTaken(RE::PlayerCharacter* player, RE::Actor* npc)
{
	if (!player)
		player = RE::PlayerCharacter::GetSingleton();

	if (npc->IsDead())
		return 0;

	if (npc->HasBeenAttacked())
	{
		CONSOLE_LOG("NPC {} has been attacked", npc->GetFormID());
		return 1;
	}
		

	return 0;
}

RE::Actor* ActorUtils::getClosestActorToActor(RE::Actor* targetActor, std::vector<RE::Actor*> otherActors)
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

std::vector<RE::Actor*> ActorUtils::extractActorsFromRoles(std::unordered_map<RE::FormID, char> roles, char roleType)
{
	std::vector<RE::Actor*> outVct;
	RE::Actor* actor;

	if (roles.size() <= 1)
		return outVct;

	for (std::unordered_map<RE::FormID, char>::iterator i = roles.begin(); i != roles.end(); i++)
	{
		if (i->second == roleType)
		{
			actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
			outVct.push_back(actor);
		}
	}

	return outVct;
}

bool IsEssentialAndOrProtected(RE::Actor* actor)
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

	return false;
}

void ActorUtils::DeadActorsCleanup(
	std::unordered_map<RE::FormID, char>& roles, 
	CombatStyleManager::profileCollection& collection,
	bool IsInCombat
)
{
	if (!IsInCombat )
	{
		if(!roles.empty())
			roles.clear();
		if(!collection.original.empty())
			collection.original.clear();
		if(!collection.modified.empty())
			collection.modified.clear();
		return;
	}

	if (roles.empty())
		return;

	if (collection.original.empty() || collection.modified.empty())
		return;

	for (std::unordered_map<RE::FormID, char>::iterator i = roles.begin(); i != roles.end(); i++)
	{
		auto actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
		if (actor->IsDead() ||
			IsEssentialAndOrProtected(actor))
		{
			CombatStyleManager::ReturnCachedSingle(collection.original, i->first);
			if (collection.modified.contains(i->first))
				collection.modified.erase(i->first);
			if (roles.contains(i->first))
				roles.erase(i->first);
		}
	}
}

void ActorUtils::checkGroupCombatStyle(std::unordered_map<RE::FormID, char> roles)
{
	RE::Actor* actor;

	if (roles.size() <= 1)
		return;

	for (std::unordered_map<RE::FormID, char>::iterator i = roles.begin(); i != roles.end(); i++)
	{
		actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
		auto cmbStyle = actor->GetActorBase()->GetCombatStyle();

		CONSOLE_LOG("Actor {} has style: {} , with ID {:X}", 
					 actor->GetDisplayFullName(), cmbStyle->GetFormEditorID(), cmbStyle->GetFormID());
	}
}

void ActorUtils::csGetModSet(RE::Actor* actor, int type, float newVal)
{

}