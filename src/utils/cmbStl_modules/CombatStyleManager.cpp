#include "CombatStyleManager.h"
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

void CombatStyleManager::profileFilterFromJSON(char type, combatStyleProf::mults& profile)
{
	switch (type)
	{
	case 'L':
		if (configData.contains("Leader"))
			combatStyleProf::setJSONToProfile(configData["Leader"], profile);
		break;
	case 'R':
		if (configData.contains("Ranger"))
			combatStyleProf::setJSONToProfile(configData["Ranger"], profile);
		break;
	case 'C':
		if (configData.contains("Caster"))
			combatStyleProf::setJSONToProfile(configData["Caster"], profile);
		break;
	default:
		if (configData.contains("Vanguard"))
			combatStyleProf::setJSONToProfile(configData["Vanguard"], profile);
		break;
	}
}

void CombatStyleManager::handleHesitation(combatStyleProf::mults& profile)
{
	profile.fallback += hesitationModifier;
	profile.avoidThreatChance += hesitationModifier;
	profile.grpOffensive -= hesitationModifier;
	profile.offensive -= hesitationModifier;
	profile.defensive += (hesitationModifier / 2);
}

combatStyleProf::mults CombatStyleManager::AssignCS(RE::TESNPC* npc, combatStyleProf::mults profile, char type)
{
	//Gets config data from JSON
	//failsafe if JSON ends up empty, that combat style will remain unchanged
	if (configData.empty())
	{
		CONSOLE_LOG("[Puppeteer] ConfigData is empty!");
		return profile;
	}

	//changes the profile base configData and through a filter using type
	profileFilterFromJSON(type, profile);

	//cloning new style from 
	tmpStyle = CloneCombatStyle(npc->GetCombatStyle());

	//Hesitation Logic
	if (hesitationModifier != 0)
		handleHesitation(profile);

	//converting profile to style
	setProfileToStyle(profile, tmpStyle);

	//setting new style
	npc->SetCombatStyle(tmpStyle);

	return profile;
}

void CombatStyleManager::AssignAndCache
(
	//const std::unordered_map<RE::FormID, char>& roleList,
	//CombatStyle::profileCollection &collection,
	std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord,
	const json& jsonStyleSettings
)
{
	if (combatRecord.size() < ConfigLoader::GetMinimumActors())
		return;

	configData = jsonStyleSettings;

	for (auto i = combatRecord.begin(); i != combatRecord.end(); i++)
	{
		auto npc = RE::TESForm::LookupByID<RE::Actor>(i->first);

#pragma region Safety Checks
		if (!npc) continue;
		if (npc && (npc->IsDeleted() || npc->IsDisabled() || !npc->Is3DLoaded())) continue;
		auto actorBase = npc->GetActorBase();
		if (!actorBase) continue;
		auto cmbStyle = actorBase->GetCombatStyle();
		if (!cmbStyle) continue;
#pragma endregion

		//Caching original styles (will not overwrite existing original style 
		// on the second cycle onwards
		if (i->second.original.initialized == false)
		{
			i->second.original = combatStyleProf::initializeGen(cmbStyle);
			i->second.original.initialized = true;

			//assigning and caching modified styles.
			// Will overwrite existing key's values no matter what
			i->second.modified = AssignCS(actorBase, i->second.original, i->second.role);
		}
	}

//	for (auto i = roleList.begin(); i != roleList.end(); i++)
//	{
//		auto npc = RE::TESForm::LookupByID<RE::Actor>(i->first);
//
//		#pragma region Safety Checks
//		if (!npc) continue;
//		if (npc && (npc->IsDeleted() || npc->IsDisabled() || !npc->Is3DLoaded())) continue;
//		auto actorBase = npc->GetActorBase();
//		if (!actorBase) continue;
//		auto cmbStyle = actorBase->GetCombatStyle();
//		if (!cmbStyle) continue;
//#pragma endregion
//
//		//Caching original styles (will not overwrite existing original style 
//		// on the second cycle onwards
//		if (!collection.original.contains(i->first))
//			collection.original[i->first] = combatStyleProf::initializeGen(cmbStyle);
//
//		//assigning and caching modified styles.
//		// Will overwrite existing key's values no matter what
//		collection.modified[i->first] = AssignCS(actorBase, collection.original[i->first], i->second);
//	}
}

void CombatStyleManager::ReturnCached(
	//const std::unordered_map<RE::FormID, char>& currentRoles,
	//CombatStyle::profileCollection& collection
	std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord
)
{
	//if (currentRoles.empty() || (collection.modified.empty() && collection.original.empty())) return;
	if (combatRecord.empty()) return;


	//for (auto i = currentRoles.begin(); i != currentRoles.end(); i++)
	for(auto i = combatRecord.begin(); i != combatRecord.end(); i++)
	{
		auto npc = RE::TESForm::LookupByID<RE::Actor>(i->first);
		if (!npc) continue;
		if (npc && (npc->IsDeleted() || npc->IsDisabled() || !npc->Is3DLoaded())) continue;
		auto npcBase = npc->GetActorBase();
		if (!npcBase)
		{
			CONSOLE_LOG("[Puppeteer] actor base is invalid");
			continue;
		}
		tmpStyle = CloneCombatStyle(npcBase->GetCombatStyle());

		if (!tmpStyle)
		{
			CONSOLE_LOG("[Puppeteer] style is invalid, not returning back to owner");
			continue;
		}

		//Only return the original style when it's initiallized
		//Otherwise, their original style will all be 0 on the next time this function is ran
		if (i->second.original.initialized)
		{
			combatStyleProf::setProfileToStyle(i->second.original,tmpStyle);

			npcBase->SetCombatStyle(tmpStyle);
		}

		combatRecord.erase(i);
	}
}

void CombatStyleManager::ReturnCachedSingle(
	//std::unordered_map<RE::FormID, combatStyleProf::mults> &cachedList, 
	std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& combatRecord,
	const RE::FormID deadForm)
{
	auto deadTarget = combatRecord.find(deadForm);
	if (deadTarget == combatRecord.end())
		return;

	auto npc = RE::TESForm::LookupByID<RE::Actor>(deadForm);
	if (!npc)
	{
		auto npcBase = npc->GetActorBase();
		if (npcBase)
		{
			tmpStyle = CloneCombatStyle(npcBase->GetCombatStyle());
			combatStyleProf::setProfileToStyle(deadTarget->second.original, tmpStyle);

			npcBase->SetCombatStyle(tmpStyle);
		}
	}
	
	combatRecord.erase(deadTarget);
	//cachedList.erase(deadTarget);
}

void CombatStyleManager::setHesitationValue(float a_value)
{
	hesitationModifier = a_value;
}


