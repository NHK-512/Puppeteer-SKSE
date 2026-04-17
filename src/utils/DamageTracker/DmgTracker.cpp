#include "DmgTracker.h"

dmgTracker::dmgTracker(RE::PlayerCharacter*& player)
{
	playerLastHP = ActorUtils::GetActorValue(player->GetFormID())->GetActorValue(RE::ActorValue::kHealth);
	instantKillTime = ConfigLoader::GetInstantKillTime();
	hesitateDuration = ConfigLoader::GetDeathHesitationDuration();
	hesitationLastTick = std::chrono::steady_clock::now();
}

dmgTracker::~dmgTracker()
{
	healthPool.clear();
	instantKillTime = 0;
	hesitateDuration = 0;
	playerLastHP = 0.0f;
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
	this->player = player;
	auto playerCurrentHP = ActorUtils::GetActorValue(player->GetFormID())->GetActorValue(RE::ActorValue::kHealth);
	//if player's health is less than last time
	if (playerCurrentHP < playerLastHP && !healthPool.empty())
	{
		auto hitData = ActorUtils::GetLastHitData(player);
		auto aggressor = hitData->aggressor.get().get();
		if (healthPool.contains(aggressor->GetFormID()))
		{
			damageData.totalDmgDealt += (playerLastHP - playerCurrentHP);
			//CONSOLE_LOG("[DmgTracker] Player has taken {} DMG from {}", playerLastHP - playerCurrentHP, aggressor->GetFormID());
		}
		playerLastHP = playerCurrentHP;
	}
	

	if (healthPool.empty())
		return;

	//Running for ever NPC per TICK
	for (auto i = healthPool.begin(); i != healthPool.end(); i++)
	{
		auto actor = RE::TESForm::LookupByID<RE::Actor>(i->first);
		if (!actor || !actor->Is3DLoaded())
			return;
		
		if (instantKillDetection(actor, i->second))
		{
			outputFlags.Set(Flag::IsInstantKilled);
		}

		isPlayerVampireLordOrWerewolf(outputFlags);
	}
}

CombatData::dmgData dmgTracker::getTotalDmgData()
{
	return damageData;
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

	//Tracks damage taken
	if (currentHealth < HPrecord.lastHealth) 
	{
		if (auto hitData = ActorUtils::GetLastHitData(actor);
			hitData && hitData->aggressor.get().get() == player
		)
		{
			auto dmgTaken = HPrecord.lastHealth - currentHealth;

			damageData.totalDmgTaken += dmgTaken;
			CONSOLE_LOG("[DmgTracker] Player dealt {} damage!", dmgTaken);

			if (ActorUtils::isWithinMeleeRange(actor, player, 250.0f))
			{
				damageData.melee += dmgTaken;
				CONSOLE_LOG("[DmgTracker] Player is dealing MELEE attacks!");
			}
			else
			{
				damageData.range += dmgTaken;
				CONSOLE_LOG("[DmgTracker] Player is dealing RANGED attacks!");
			}
			
			/*
			CONSOLE_LOG("[DmgTracker] Total damage: {}", hitData->totalDamage);
			CONSOLE_LOG("[DmgTracker] Player dealt {} PHYSICAL damage!", hitData->physicalDamage);
			if (auto hitDataSpell = hitData->attackDataSpell)
				CONSOLE_LOG("[DmgTracker] I. Hit Data Spell: {}", hitDataSpell->GetFullName());
			if (auto weap = hitData->weapon)
				CONSOLE_LOG("[DmgTracker] Weapon name: {}", weap->GetFullName());
			if (auto atkData = hitData->attackData)
			{
				if (auto atkDataSpell = atkData->data.attackSpell)
					CONSOLE_LOG("[DmgTracker] II. Atk Data Spell: {}", atkDataSpell->GetFullName());
			}

			if (auto atkData = ActorUtils::GetAttackData(actor))
			{
				if (auto atkSpell = atkData->attackSpell)
					CONSOLE_LOG("[DmgTracker] III. Atk Data Spell: {}", atkSpell->GetFullName());
				if (auto atkType = atkData->attackType)
					CONSOLE_LOG("[DmgTracker] Atk Type: {}", atkType->GetName());
			}

			if (hitData->attackDataSpell)
				CONSOLE_LOG("[DmgTracker] Player's attack is a spell");
			else
				CONSOLE_LOG("[DmgTracker] Player's attack is physical");
			auto atkType = hitData->attackData->data.attackType;
			if (atkType)
				CONSOLE_LOG("[DmgTracker] Attack type: {}", atkType->GetName());*/
		}

		
		HPrecord.lastHealth = currentHealth;
		if (!HPrecord.hasTakenDamage)
		{
			HPrecord.firstHitTime = now;
			HPrecord.hasTakenDamage = true;
		}
	}

	return false;
}

void dmgTracker::isPlayerVampireLordOrWerewolf(FlagSet& outputFlags)//, RE::PlayerCharacter*& player)
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





