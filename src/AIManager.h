#pragma once

#include "PCH.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "utils/core_modules/EnemyScanner.h"
#include "utils/core_modules/ConsoleUtils.h"
#include "utils/core_modules/FileLoaders/ConfigLoader.h"
#include "utils/Puppeteer.h"
#include "utils/CombatSession.h"
#include "utils/DamageTracker/DmgFlags.h"
#include "utils/DamageTracker/DmgTracker.h"
#include "utils/cmbStl_modules/CombatStyleManager.h"

#include <algorithm>
#include <format>

#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

class AIManager
{
public:
    static void Initialize();
    static void LoadSettings();
private:
    static void Update();

    static inline RE::PlayerCharacter* player;

    // Active combat session (nullptr if not in combat)
    static inline std::unique_ptr<CombatSession> activeSession;
    static inline std::unique_ptr<dmgTracker> activeTracker;
    static inline std::vector<RE::FormID> all_enemies;
    static inline FlagSet dmgFlags;

    // CONFIG (non-combat)
    static inline float scanDistance = 0.f;
    static inline int secondsPerCycle = 1;
    static inline int minimumActors = 0;
    static inline int maxSkipCycles = 0;
    static inline json rolesMult = json::object();

    static inline std::chrono::steady_clock::time_point lastTick;
};
