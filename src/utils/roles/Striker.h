#pragma once
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(std::format(__VA_ARGS__).c_str())

namespace Striker
{
	void Assign(RE::Actor* npc);
}