#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "RE/Skyrim.h"
#include "Puppeteer.h"
#include "core_modules/EnemyScanner.h"
#include "core_modules/FileLoaders/ConfigLoader.h"
//#include "core_modules/FileLoaders/BattleRecord.h"
#include "core_modules/ConsoleUtils.h"
#include "cmbStl_modules/CombatStyleManager.h"
#include "DamageTracker/DmgFlags.h"
#include "ConfidenceModules/ConfidenceChecks.h"

#define CONSOLE_LOG(...) consoleUtils::Log(__VA_ARGS__)

class CombatSession
{
public:
    explicit CombatSession();
    ~CombatSession();

    // Called every AIManager cycle while combat is active
    void Tick(RE::PlayerCharacter*& player, const PuppeteerConfig& cfg, const FlagSet& dmgFlags);
    void AssignRolesAndTrack(const PuppeteerConfig& cfg);

    // True if the session should continue running
    bool IsValid() const;
    bool IsPlayerInCombat() const
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        return player && player->IsInCombat();
    }

    const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& extractCurrentEnemies();

private:
    //Core references
    RE::PlayerCharacter* player;
    CombatStyleManager CSManager;
    ConfidenceChecks CFDManager;

    //Enemy tracking
    std::vector<RE::FormID> enemies;
    std::vector<RE::FormID> previousEnemies;

    //Role & leader state
    std::unordered_map<RE::FormID, char> currentRoles;
    RE::FormID currentLeaderID{ 0 };
    RE::FormID previousLeaderID{ 0 };

    //Leader death handling
    int cyclesSinceLeaderDeath{ 0 };

    // Combat style cache
    CombatStyle::profileCollection profCollection;
    std::unordered_map<RE::FormID, CombatData::npcCombatInfo> combatRecord;

    //killRecord roleKills;
};
