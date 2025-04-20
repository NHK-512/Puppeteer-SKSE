#pragma once
#define CONSOLE_LOG(...) RE::ConsoleLog::GetSingleton()->Print(fmt::format(__VA_ARGS__).c_str())

namespace Leader
{
	void Assign(RE::Actor* npc);
}