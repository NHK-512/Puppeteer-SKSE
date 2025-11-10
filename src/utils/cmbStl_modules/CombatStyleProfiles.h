#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

using namespace nlohmann;

namespace combatStyleProf
{
	struct mults
	{
		float offensive = 0;
		float defensive = 0;
		float grpOffensive = 0;
		float avoidThreatChance = 0;
		float circle = 0;
		float fallback = 0;
		float flank = 0;
		float stalk = 0;
		float strafe = 0;
		float atkStaggered = 0;
		float patkStaggered = 0;
		float patkBlocking = 0;
		float bash = 0;
		float bashRecoil = 0;
		float bashAtk = 0;
		float bashPatk = 0;
		float specialAtk = 0;
	};

	//Initializes and save a copy of each value into a profile
	mults initializeGen(RE::TESCombatStyle* original);
	//Converts the profile back to Combat Style
	RE::TESCombatStyle* setProfileToStyle(mults profile, RE::TESCombatStyle* style);

	//Converts the config JSON to a Profile depending on the role mentioned
	void setJSONToProfile(nlohmann::json config, mults& profile);
}