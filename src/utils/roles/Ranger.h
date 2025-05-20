#pragma once
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(fmt::format(__VA_ARGS__).c_str())

namespace Ranger
{
	void AdjustRangerTactics(const std::vector<RE::Actor*>& rangers, const std::vector<RE::Actor*>& vanguards, RE::Actor* player);
	void PrioritizeDistance(RE::Actor* ranger, RE::Actor* player, float desiredDistance);
	void Assign(RE::Actor* npc);
}