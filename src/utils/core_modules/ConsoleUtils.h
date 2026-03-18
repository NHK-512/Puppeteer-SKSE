#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "CombatData.h"
#include "FileLoaders/ConfigLoader.h"
#include <unordered_map>

//#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

static RE::ObjectRefHandle cachedTarget;
namespace consoleUtils
{
	bool TriggerOnce(const char* key, bool condition);
	void inspectCBStyleOfSelected(const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& roleList);
    void Log(const std::string& msg);

    template <typename... Args>
    void Log(const std::string& fmt, Args&&... args)
    {
        if (!ConfigLoader::GetEnabledLogs()) {
            return;  // Logging disabled
        }

        try {
            std::string formatted = std::vformat(fmt, std::make_format_args(args...));
            Log(formatted);
        }
        catch (const std::format_error& e) {
            // Fail-safe log to avoid crashes from bad format strings
            Log(std::string("[Formatting error] ") + e.what());
        }
    }
}