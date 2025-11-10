// roles/Vanguard.cpp
#include "Vanguard.h"
#include <RE/Skyrim.h>

void Vanguard::GetCombatProfile(combatStyleProf::mults& prof)
{
	prof.offensive = 0.08f;
	prof.defensive = 0.85f;
	prof.grpOffensive = 0.15f;
	prof.circle = 0.65f;
	prof.stalk = 0.05f;
}

void Vanguard::WriteDefaultProfileToJSON(nlohmann::json& j)
{
	j["roles"]["Vanguard"]["offensive"] = 0.08f;
	j["roles"]["Vanguard"]["defensive"] = 0.85f;
	j["roles"]["Vanguard"]["grpOffensive"] = 0.15f;
	j["roles"]["Vanguard"]["circle"] = 0.65f;
	j["roles"]["Vanguard"]["stalk"] = 0.05f;
}