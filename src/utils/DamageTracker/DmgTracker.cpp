#include "DmgTracker.h"

dmgTracker::dmgTracker(const std::vector<RE::FormID>& enemies)
{
	UpdateList(enemies);
	instantKillTime = ConfigLoader::GetInstantKillTime();
	hesitateDuration = ConfigLoader::GetDeathHesitationDuration();
	hesitationLastTick = std::chrono::steady_clock::now();
}

dmgTracker::~dmgTracker()
{
	healthPool.clear();
	instantKillTime = 0;
	hesitateDuration = 0;
}

void dmgTracker::UpdateList(const std::vector<RE::FormID>& enemies)
{
	for (const auto& key : enemies)
	{
		healthPool.try_emplace(key, [&]
		{
			HPperNPC hp;
			auto actor = RE::TESForm::LookupByID<RE::Actor>(key);
			if (actor && actor->Is3DLoaded())
			{
				hp.lastHealth = actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth);
			}
			return hp;
		}());
	}
}

void dmgTracker::Tick(
	FlagSet& outputFlags, 
	std::chrono::steady_clock::time_point& now,
	const std::unordered_map< RE::FormID, combatStyleProf::mults>& modifiedCmbs
)
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
	}
}

bool dmgTracker::instantKillDetection(RE::Actor* actor, HPperNPC& HPrecord)
{
	auto now = std::chrono::steady_clock::now();
	if (actor->IsDead())
	{
		if (!HPrecord.firstHitTime.has_value())
		{
			return true; //if dead before timestamped
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





