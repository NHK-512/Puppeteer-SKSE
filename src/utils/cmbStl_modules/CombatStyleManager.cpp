#include "CombatStyleManager.h"
using namespace CombatStyleManager;
using namespace nlohmann;

RE::TESCombatStyle* CloneCombatStyle(RE::TESCombatStyle* original)
{
	if (!original)
	{
		return nullptr;
	}

	auto duppedForm = original->CreateDuplicateForm(false, nullptr);
	if (!duppedForm)
	{
		return nullptr;
	}

	auto clonedCmbStl = duppedForm->As<RE::TESCombatStyle>();

	return clonedCmbStl;
}

void profileFilter(char type, combatStyleProf::mults& profile)
{
	switch (type)
	{
	case 'L':
		Leader::GetCombatProfile(profile);
		break;
	case 'R':
		Ranger::GetCombatProfile(profile);
		break;
	default:
		Vanguard::GetCombatProfile(profile);
		break;
	}
}

void profileFilterFromJSON(json cfg, char type, combatStyleProf::mults& profile)
{
	switch (type)
	{
	case 'L':
		if (cfg.contains("roles") && cfg["roles"].contains("Leader"))
			combatStyleProf::setJSONToProfile(cfg["roles"]["Leader"], profile);
		break;
	case 'R':
		if (cfg.contains("roles") && cfg["roles"].contains("Ranger"))
			combatStyleProf::setJSONToProfile(cfg["roles"]["Ranger"], profile);
		break;
	default:
		if (cfg.contains("roles") && cfg["roles"].contains("Vanguard"))
			combatStyleProf::setJSONToProfile(cfg["roles"]["Vanguard"], profile);
		break;
	}
}

combatStyleProf::mults AssignCS(RE::TESNPC* npc, combatStyleProf::mults profile, char type)
{
	//Gets config data from JSON
	auto configData = ConfigLoader::LoadConfigIfChanged();

	//failsafe if JSON ends up empty, that combat style will remain unchanged
	if (configData.empty())
	{
		CONSOLE_LOG("[Puppeteer] ConfigData is empty!");
		return profile;
	}

	//changes the profile base configData and through a filter using type
	profileFilterFromJSON(configData, type, profile);

	//cloning new style from 
	auto customStyle = CloneCombatStyle(npc->GetCombatStyle());

	//converting profile to style
	combatStyleProf::setProfileToStyle(profile, customStyle);

	//setting new style
	npc->SetCombatStyle(customStyle);

	return profile;
}

profileCollection CombatStyleManager::AssignAndCache(std::unordered_map<RE::FormID, char> roleList)
{
	profileCollection collection;
	std::unordered_map<RE::FormID, combatStyleProf::mults> original;
	std::unordered_map<RE::FormID, combatStyleProf::mults> modified;

	if (roleList.size() <= 1)
		return collection;

	for (std::unordered_map<RE::FormID, char>::iterator i = roleList.begin(); i != roleList.end(); i++)
	{
		auto actorBase = RE::TESForm::LookupByID<RE::Actor>(i->first)->GetActorBase();

		auto cmbStyle = actorBase->GetCombatStyle();

		//Caching original styles (will not overwrite existing original style 
		// on the second cycle onwards
		if(!original.empty() && original.contains(i->first))
			original[i->first] = combatStyleProf::initializeGen(cmbStyle);

		//assigning and caching modified styles.
		// Will overwrite existing key's values no matter what
		modified[i->first] = AssignCS(actorBase, original[i->first], i->second);
	}

	collection.original = original;
	collection.modified = modified;

	return collection;
}

void CombatStyleManager::ReturnCached(std::unordered_map<RE::FormID, combatStyleProf::mults> cache)
{
	for (std::unordered_map<RE::FormID, combatStyleProf::mults>::iterator
		i = cache.begin(); i != cache.end(); i++)
	{
		auto npc = RE::TESForm::LookupByID<RE::Actor>(i->first)->GetActorBase();
		auto ogStyle = CloneCombatStyle(npc->GetCombatStyle());
		ogStyle = combatStyleProf::setProfileToStyle(i->second, ogStyle);

		npc->SetCombatStyle(ogStyle);
	}
}

void CombatStyleManager::ReturnCachedSingle(std::unordered_map<RE::FormID, combatStyleProf::mults> &cachedList, RE::FormID deadForm)
{
	auto deadTarget = cachedList.find(deadForm);
	if (deadTarget == cachedList.end())
		return;

	auto npc = RE::TESForm::LookupByID<RE::Actor>(deadForm)->GetActorBase();
	auto ogStyle = CloneCombatStyle(npc->GetCombatStyle());
	ogStyle = combatStyleProf::setProfileToStyle(deadTarget->second, ogStyle);

	npc->SetCombatStyle(ogStyle);

	cachedList.erase(deadTarget);
}