#include "CombatStyleProfiles.h"
using namespace combatStyleProf;

mults combatStyleProf::initializeGen(RE::TESCombatStyle* original)
{
	mults profile;

	profile.offensive = original->generalData.offensiveMult;
	profile.defensive = original->generalData.defensiveMult;
	profile.grpOffensive = original->generalData.groupOffensiveMult;
	profile.avoidThreatChance = original->generalData.avoidThreatChance;
	profile.circle = original->closeRangeData.circleMult;
	profile.flank = original->closeRangeData.flankDistanceMult;
	profile.fallback = original->closeRangeData.fallbackMult;
	profile.stalk = original->closeRangeData.stalkTimeMult;
	profile.strafe = original->longRangeData.strafeMult;
	profile.atkStaggered = original->meleeData.attackIncapacitatedMult;
	profile.bash = original->meleeData.bashMult;
	profile.bashAtk = original->meleeData.bashAttackMult;
	profile.bashPatk = original->meleeData.bashPowerAttackMult;
	profile.bashRecoil = original->meleeData.bashRecoilMult;
	profile.patkBlocking = original->meleeData.powerAttackBlockingMult;
	profile.patkStaggered = original->meleeData.powerAttackIncapacitatedMult;

	return profile;
}

//RE::TESCombatStyle* 
void
combatStyleProf::setProfileToStyle(mults profile, RE::TESCombatStyle*& style)
{
	//RE::TESCombatStyle* style;

	style->generalData.offensiveMult = profile.offensive;
	style->generalData.defensiveMult = profile.defensive;
	style->generalData.groupOffensiveMult = profile.grpOffensive;
	style->generalData.avoidThreatChance = profile.avoidThreatChance;
	style->closeRangeData.circleMult = profile.circle;
	style->closeRangeData.flankDistanceMult = profile.flank;
	style->closeRangeData.fallbackMult = profile.fallback;
	style->closeRangeData.stalkTimeMult = profile.stalk;
	style->longRangeData.strafeMult = profile.strafe;
	style->meleeData.attackIncapacitatedMult = profile.atkStaggered;
	style->meleeData.bashMult = profile.bash;
	style->meleeData.bashAttackMult = profile.bashAtk;
	style->meleeData.bashPowerAttackMult = profile.bashPatk;
	style->meleeData.bashRecoilMult = profile.bashRecoil;
	style->meleeData.powerAttackBlockingMult = profile.patkBlocking;
	style->meleeData.powerAttackIncapacitatedMult = profile.patkStaggered;

	//return style;
}

void combatStyleProf::setJSONToProfile(nlohmann::json cfg, mults& profile)
{
	profile.offensive = cfg.value("offensive", profile.offensive);
	profile.defensive = cfg.value("defensive", profile.defensive);
	profile.grpOffensive = cfg.value("grpOffensive", profile.grpOffensive);
	profile.avoidThreatChance = cfg.value("avoidThreatChance", profile.avoidThreatChance);
	profile.circle = cfg.value("circle", profile.circle);
	profile.flank = cfg.value("flank", profile.flank);
	profile.fallback = cfg.value("fallback", profile.fallback);
	profile.stalk = cfg.value("stalk", profile.stalk);
	profile.strafe = cfg.value("strafe", profile.strafe);
	profile.atkStaggered = cfg.value("atkStaggered", profile.atkStaggered);
	profile.bash = cfg.value("bash", profile.bash);
	profile.bashAtk = cfg.value("bashAtk", profile.bashAtk);
	profile.bashPatk = cfg.value("bashPatk", profile.bashPatk);
	profile.bashRecoil = cfg.value("bashRecoil", profile.bashRecoil);
	profile.patkBlocking = cfg.value("patkBlocking", profile.patkBlocking);
	profile.patkStaggered = cfg.value("patkStaggered", profile.patkStaggered);
}
