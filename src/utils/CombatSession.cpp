#include "CombatSession.h"

CombatSession::CombatSession(
    const PuppeteerConfig& cfg, 
    std::vector<RE::FormID>& a_list
)
{
    EnemyScanner::GetHostileNPCsNearPlayer(cfg.scanDistance, a_list);

    CONSOLE_LOG("[CombatSession] Enemy list transferred");
}

CombatSession::~CombatSession()
{
    // Revert all modified combat styles
    CSManager.ReturnCached(
        currentRoles,
        profCollection
    );

    // Explicit cleanup (safe & intentional)
    currentRoles.clear();
    enemies.clear();
    previousEnemies.clear();

    currentLeaderID = 0;
    previousLeaderID = 0;
    cyclesSinceLeaderDeath = 0;

    CONSOLE_LOG("[Puppeteer] CombatSession destroyed. State cleaned up.");
}

bool CombatSession::IsValid() const
{
    return player;
}

std::vector<RE::FormID> CombatSession::extractRoles()
{
    return (!enemies.empty()) ? enemies : std::vector<RE::FormID>();
}

const std::unordered_map<RE::FormID, combatStyleProf::mults>& CombatSession::extractModifiedCmbs()
{
    return profCollection.modified;
}

void CombatSession::Tick(const PuppeteerConfig& cfg, const FlagSet& dmgFlags)
{
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

    if (cyclesSinceLeaderDeath > cfg.maxSkipCycles)
    {
        if (dmgFlags[Flag::IsInstantKilled])
            CSManager.setHesitationValue(1.0);

        CSManager.AssignAndCache(
            currentRoles,
            profCollection,
            *cfg.rolesMult
        );

        if (dmgFlags[Flag::IsInstantKilled])
            CSManager.setHesitationValue(0.0);

        Puppeteer::executeTactics(
            currentRoles,
            player
        );
    }
    else
    {
        CSManager.ReturnCached(
            currentRoles,
            profCollection
        );
    }

    previousEnemies = enemies;
}

void CombatSession::AssignRolesAndTrack(const PuppeteerConfig& cfg)
{
    if (enemies.empty())
    {
        if (!previousEnemies.empty())
        {
            CSManager.ReturnCached(
                currentRoles,
                profCollection
            );

            if (consoleUtils::TriggerOnce(
                "NO_ACTORS_DETECTED",
                !previousEnemies.empty()))
            {
                CONSOLE_LOG(
                    "[Puppeteer] Still in combat. No enemies detected. Start cleaning list...");
            }
        }

        if (consoleUtils::TriggerOnce(
            "NO_ACTORS_VALID",
            enemies.empty()))
        {
            CONSOLE_LOG(
                "[Puppeteer] No eligible enemy is scanned");
        }

        return;
    }

    ActorUtils::DeadActorsCleanup(
        currentRoles,
        profCollection,
        CSManager,
        IsPlayerInCombat()
    );

    if (enemies.size() < static_cast<std::size_t>(cfg.minimumActors))
    {
        if (consoleUtils::TriggerOnce(
            "LESS_ACTORS",
            enemies.size() < cfg.minimumActors))
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
        currentRoles
    );

    currentLeaderID = 0;
    for (const auto& [formID, role] : currentRoles)
    {
        if (role == 'L')
        {
            currentLeaderID = formID;
            break;
        }
    }

    if (cyclesSinceLeaderDeath >= cfg.maxSkipCycles)
    {
        previousEnemies = enemies;
    }
}
