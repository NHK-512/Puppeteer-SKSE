#pragma once
#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

namespace Striker
{
	void Assign(RE::Actor* npc);
}