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

void CombatStyleManager::AssignAndCache
(
	std::unordered_map<RE::FormID, char> roleList,
	profileCollection &collection
)
{
	/*profileCollection collection;
	std::unordered_map<RE::FormID, combatStyleProf::mults> original;
	std::unordered_map<RE::FormID, combatStyleProf::mults> modified;*/

	if (roleList.size() < ConfigLoader::GetMinimumActors())
		return;

	for (std::unordered_map<RE::FormID, char>::iterator i = roleList.begin(); i != roleList.end(); i++)
	{
		auto npc = RE::TESForm::LookupByID<RE::Actor>(i->first);
		if (!npc || npc->IsDeleted() || npc->IsDisabled() || !npc->Is3DLoaded()) continue;
		auto actorBase = npc->GetActorBase();
		if (!actorBase) continue;
		auto cmbStyle = actorBase->GetCombatStyle();
		if (!cmbStyle) continue;

		//Caching original styles (will not overwrite existing original style 
		// on the second cycle onwards

		//if(!original.empty() && original.contains(i->first))
		if (!collection.original.contains(i->first))
			collection.original[i->first] = combatStyleProf::initializeGen(cmbStyle);

		//assigning and caching modified styles.
		// Will overwrite existing key's values no matter what
		collection.modified[i->first] = AssignCS(actorBase, collection.original[i->first], i->second);
	}

	/*if (collection.original.size() > currentRoles.size() &&
		collection.modified.size() > currentRoles.size())
	{
		for (auto j = collection.original.begin(); j != collection.original.end(); j++)
		{
			if (currentRoles.find(j->first) == currentRoles.end())
			{
				collection.original.erase(j);
				collection.modified.erase(j);
			}
		}
	}*/
}

void CombatStyleManager::ReturnCached(
	std::unordered_map<RE::FormID, char>& currentRoles,
	profileCollection& collection
)
{
	if (currentRoles.empty() || (collection.modified.empty() && collection.original.empty())) return;

	for (std::unordered_map<RE::FormID, char>::iterator
		i = currentRoles.begin(); i != currentRoles.end(); i++)
	{
		auto npc = RE::TESForm::LookupByID<RE::Actor>(i->first);
		if (!npc) continue;
		if (npc->IsDeleted() || npc->IsDisabled() || !npc->Is3DLoaded()) continue;
		auto npcBase = npc->GetActorBase();
		if (!npcBase)
		{
			CONSOLE_LOG("[Puppeteer] actor base is invalid");
			continue;
		}
		auto style = CloneCombatStyle(npcBase->GetCombatStyle());

		if (!style)
		{
			CONSOLE_LOG("[Puppeteer] style is invalid, not returning back to owner");
			continue;
		}
		style = combatStyleProf::setProfileToStyle(collection.original.find(i->first)->second, style);

		npcBase->SetCombatStyle(style);

		collection.modified.erase(i->first);
		collection.original.erase(i->first);
		currentRoles.erase(i);
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