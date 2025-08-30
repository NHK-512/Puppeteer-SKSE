#pragma once
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(fmt::format(__VA_ARGS__).c_str())
#include "../ActorUtils.h"

namespace Ranger
{
	//Core function for rangers to prioritize distance
	void KeepDistanceAwayPlayer(RE::Actor* ranger, const std::vector<RE::Actor*>& vanguards, RE::Actor* player);
	void Assign(RE::Actor* npc);

	//void PrioritizeDistance(RE::Actor* ranger, RE::Actor* player, float desiredDistance);
}