#include "CombatSession.h"

CombatSession::CombatSession()
{

}

CombatSession::~CombatSession()
{
    // Revert all modified combat styles
    CSManager.ReturnCached(
        combatRecord
        //currentRoles,
        //profCollection
    );

    CFDManager.returnOriginalConfidence(combatRecord);

    // Explicit cleanup (safe & intentional)
    //currentRoles.clear();
    enemies.clear();
    previousEnemies.clear();
    combatRecord.clear();

    currentLeaderID = 0;
    previousLeaderID = 0;
    cyclesSinceLeaderDeath = 0;

    CONSOLE_LOG("[Puppeteer] CombatSession destroyed. State cleaned up.");
}

bool CombatSession::IsValid() const
{
    return player;
}

const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& CombatSession::extractCurrentEnemies()
{
    return combatRecord;    
}

void CombatSession::Tick(
    RE::PlayerCharacter*& player
    , const PuppeteerConfig& cfg
    , const FlagSet& dmgFlags
)
{
    this->player = player;

    //Gets list of human hostiles
    EnemyScanner::GetHostileNPCsNearPlayer(cfg.scanDistance, enemies);

    //Checks if the current tick is the second one onwards
    //and if true, caches the leader
    if (!previousEnemies.empty())
    {
        if (cyclesSinceLeaderDeath > cfg.maxSkipCycles)
            previousLeaderID = currentLeaderID;

        if (consoleUtils::TriggerOnce("SAME_ENEMIES", previousEnemies == enemies))
            CONSOLE_LOG("[Puppeteer] List of enemies is the same");
    }

    //CONSOLE_LOG("[Previous En] {}   |   [Current En] {}", previousEnemies.size(), enemies.size());
    ////Checks if list of enemies is initiated for the first time
    ////Or the list changes from new enemies or old enemies dies off
    if(previousEnemies.empty() || previousEnemies != enemies)
    {
        //Starting cycle count will be the max-skip-cycles onwards
        if (previousEnemies.empty())
            cyclesSinceLeaderDeath = cfg.maxSkipCycles;

        //Checks for Leader's death
        //If true then reset the cycle count
        auto leader = RE::TESForm::LookupByID<RE::Actor>(currentLeaderID);
        if (leader && leader->IsDead() && leader->Is3DLoaded())
            cyclesSinceLeaderDeath = 0;

        AssignRolesAndTrack(cfg);
    }

    //Cycle count
    ++cyclesSinceLeaderDeath;

    //Confidence Checks works outside of Puppeteer's leader death restraint
    CFDManager.evaluate(dmgFlags, combatRecord);

    if (cyclesSinceLeaderDeath > cfg.maxSkipCycles)
    {
        /*CONSOLE_LOG("[Puppeteer][CombatSession] Normal Cycle count No.{} | Size: {}"
            , cyclesSinceLeaderDeath
            , combatRecord.size()
        );*/

        //Sets the Hesitation multiplier in CSManager class obj
        if (dmgFlags[Flag::IsInstantKilled])
            CSManager.setHesitationValue(1.0);

        //If Hesitation mult is successfully set, combat style mults will be added on last
        //before setting in the Actor itself
        CSManager.AssignAndCache(combatRecord, *cfg.rolesMult);

        //Hesitation mult is reset afterwards in case the flag is not triggered anymore
        if (dmgFlags[Flag::IsInstantKilled])
            CSManager.setHesitationValue(0.0);

        Puppeteer::executeTactics(combatRecord, player);
    }
    else
    {
        CONSOLE_LOG(
            "[Puppeteer][CombatSession] Currently skipping cycle No.{}", 
            cyclesSinceLeaderDeath
        );
        // Will no longer resets the CombatRecord if the leader is dead
        // This used to make Confidence levels reset -> inconsistent
        // Passes this job to DeadActorsCleanup in AssignRolesAndTrack above
        //CSManager.ReturnCached(combatRecord);
    }
}

void CombatSession::AssignRolesAndTrack(const PuppeteerConfig& cfg)
{
    if (enemies.empty())
    {
        if (consoleUtils::TriggerOnce("NO_ACTORS_VALID",enemies.empty()))
        {
            CONSOLE_LOG("[Puppeteer] No eligible enemy is scanned");
        }

        if (!previousEnemies.empty())
        {
            if (consoleUtils::TriggerOnce(
                "NO_ACTORS_DETECTED",
                !previousEnemies.empty()))
            {
                CONSOLE_LOG(
                    "[Puppeteer] Still in combat. No enemies detected. Start cleaning list...");
            }

            CSManager.ReturnCached(
                combatRecord
                //currentRoles,
                //profCollection
            );
        }

        return;
    }

    ActorUtils::DeadActorsCleanup
    (   combatRecord
    ,   CFDManager
    ,   CSManager
    ,   IsPlayerInCombat()
    );

    if (enemies.size() < static_cast<std::size_t>(cfg.minimumActors))
    {
        if (consoleUtils::TriggerOnce("LESS_ACTORS",enemies.size() < cfg.minimumActors))
        {
            CONSOLE_LOG(
                "[Puppeteer] Scanned enemies ({0:d}) is less than minimum required ({1:d}).",
                enemies.size(),
                cfg.minimumActors);
        }

        return;
    }

    Puppeteer::AssignRoles(
        enemies,
        combatRecord//currentRoles
    );

    currentLeaderID = 0;

    for (const auto& npc : combatRecord)
    {
        if (npc.second.role == 'L')
        {
            currentLeaderID = npc.first;
            break;
        }
    }

    if (cyclesSinceLeaderDeath >= cfg.maxSkipCycles)
    {
        previousEnemies = enemies;
    }
}
