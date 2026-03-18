#include "DmgTracker.h"

dmgTracker::dmgTracker()
{
	//UpdateList(enemies);
	instantKillTime = ConfigLoader::GetInstantKillTime();
	hesitateDuration = ConfigLoader::GetDeathHesitationDuration();
	hesitationLastTick = std::chrono::steady_clock::now();
}

dmgTracker::~dmgTracker()
{
	healthPool.clear();
	instantKillTime = 0;
	hesitateDuration = 0;
	isPlayerVampireLordOnce = false;
	isPlayerWerewolfOnce = false;
}

void dmgTracker::UpdateList(const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& enemies)
{
	//For adding in any new enemies
	for (const auto& key : enemies)
	{
		healthPool.try_emplace(key.first, [&]
		{
			HPperNPC hp;
			auto actor = RE::TESForm::LookupByID<RE::Actor>(key.first);
			if (actor && actor->Is3DLoaded())
			{
				hp.lastHealth = actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
			}
			return hp;
		}());
	}

	//For removing any dead enemies and not re-triggering instant-kill flags
	for (auto& npc : healthPool)
	{
		auto actor = RE::TESForm::LookupByID<RE::Actor>(npc.first);
		if (actor && actor->IsDead())
		{
			healthPool.erase(npc.first);
		}
	}
}

//Is triggered per second
void dmgTracker::Tick(FlagSet& outputFlags, RE::PlayerCharacter*& player)
{
	if (healthPool.empty())
		return;

	for (auto i = healthPool.begin(); i != healthPool.end(); i++)
	{
		auto actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
		if (!actor || !actor->Is3DLoaded())
			return;
		
		if (instantKillDetection(actor, i->second))
		{
			outputFlags.Set(Flag::IsInstantKilled);
		}

		isPlayerVampireLordOrWerewolf(outputFlags, player);
	}
}

bool dmgTracker::instantKillDetection(RE::Actor* actor, HPperNPC& HPrecord)
{
	auto now = std::chrono::steady_clock::now();

	//Case 1: When an actor is dead -> evaluates kill time
	if (actor->IsDead())
	{
		if (!HPrecord.firstHitTime.has_value())
		{
			return true; //if dead before timestamped (failsafe)
		}

		auto survivalTime = std::chrono::duration_cast<std::chrono::seconds>
			(now - *HPrecord.firstHitTime).count();


		if (survivalTime <= instantKillTime)
		{
			CONSOLE_LOG("[Dmg Tracker] {}'s survival time: {}, Max kill time: {}", actor->GetDisplayFullName(), survivalTime, instantKillTime);
			return true; //if dead within kill time
		}

		//if dead longer than kill time
		return false;
	}

	//Case 2: When an actor is instantly kill via killmove even at full health
	if (actor->IsInKillMove() && HPrecord.hasTakenDamage == false)
	{
		CONSOLE_LOG("[Puppeteer][DmgTracker] {} is in kill move and instantly killed!", actor->GetDisplayFullName());
		return true;
	}

	float currentHealth = actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);

	if (currentHealth < HPrecord.lastHealth) 
	{
		HPrecord.lastHealth = currentHealth;
		if (!HPrecord.hasTakenDamage)
		{
			HPrecord.firstHitTime = now;
			HPrecord.hasTakenDamage = true;
		}
	}

	return false;
}

void dmgTracker::isPlayerVampireLordOrWerewolf(FlagSet& outputFlags, RE::PlayerCharacter*& player)
{
	//Limit vampire lord and werewolf transformation flagging to only once per battle
	if (isPlayerVampireLordOnce && isPlayerWerewolfOnce)
	{
		return;
	}

	if (!player)	return;
	auto raceName = player->GetRace()->GetFullName();

	if (!isPlayerVampireLordOnce)
	{
		if (std::strcmp(raceName, "Vampire Lord") == 0)
		{
			isPlayerVampireLordOnce = true;
			outputFlags.Set(Flag::IsPlayerVampireLord);
			return;
		}
	}

	if (!isPlayerWerewolfOnce)
	{
		if (std::strcmp(raceName, "Werewolf") == 0)
		{
			isPlayerWerewolfOnce = true;
			outputFlags.Set(Flag::IsPlayerWerewolf);
			return;
		}
	}
}





