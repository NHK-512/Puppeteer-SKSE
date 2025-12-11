#include "AIManager.h"

static void ResetSystem()
{
    CombatStyleManager::ReturnCached(currentRoles, profCollection);
    if (!currentRoles.empty())
        currentRoles.clear();
    if (!enemies.empty())
        enemies.clear();
    if (!previousEnemies.empty()) 
        previousEnemies.clear();
    if (leaderForm)             
        leaderForm = 0;
    if (leaderFormCache)        
        leaderFormCache = 0;
    if (maxSkipCycles < 0 && countSinceLeaderDeath < maxSkipCycles) 
        countSinceLeaderDeath = maxSkipCycles;
}

static void RoleAssignAndTracker()
{
    if (enemies.empty())
    {
        if (!previousEnemies.empty())
        {
            CombatStyleManager::ReturnCached(currentRoles, profCollection);
            if(consoleUtils::TriggerOnce("NO_ACTORS_DETECTED", !previousEnemies.empty()))
                CONSOLE_LOG("[Puppeteer] Still in combat. No enemies detected. Start cleaning list...");
        }

        if (consoleUtils::TriggerOnce("NO_ACTORS_VALID", enemies.empty()))
            CONSOLE_LOG("[Puppeteer] No eligible enemy is scanned");
        return;
    }

    ActorUtils::DeadActorsCleanup(currentRoles, profCollection, RE::PlayerCharacter::GetSingleton()->IsInCombat());

    if (enemies.size() < minimumActors)
    {
        if (consoleUtils::TriggerOnce("LESS_ACTORS", enemies.size() < minimumActors))
            CONSOLE_LOG(
                "[Puppeteer] Scanned enemies ({0:d}) is less than minimum required ({1:d}).",
                enemies.size(), minimumActors);
        return;
    }

    //currentRoles = 
    Puppeteer::AssignRoles(enemies, currentRoles);

    if (currentRoles.begin()->second == 'L')
    {
        leaderForm = currentRoles.begin()->first;
        //leader = RE::TESForm::LookupByID<RE::Actor>(currentRoles.begin()->first);

    }
    else
    {
        for (auto actor : currentRoles)
        {
            if (actor.second == 'L')
            {
                leaderForm = actor.first;
                //leader = RE::TESForm::LookupByID(actor.first)->As<RE::Actor>(); break;
            }
        }
    }

    if (countSinceLeaderDeath >= maxSkipCycles) {
        previousEnemies = enemies;
    }
}

static void RoleControl()
{
    //Always scans in every cycle
    //Scanner gets list of humanoid hostiles near player
    //enemies List constant gets overwritten and updated
    //enemies = 
    EnemyScanner::GetHostileNPCsNearPlayer(scanDistance, enemies);

    //Caching leader of previous cycle to keep track if they are dead or not
    //while keep assigning new leader
    //Only works from 2nd cycle onwards
    if (!previousEnemies.empty())
    {
        if(countSinceLeaderDeath > maxSkipCycles)
            leaderFormCache = leaderForm;

        if (consoleUtils::TriggerOnce("SAME_ENEMIES", previousEnemies == enemies))
            CONSOLE_LOG("[Puppeteer] List of enemies is the same");
    }

    if (previousEnemies.empty() || //true if 1st cycle
        previousEnemies != enemies)//true if dead actors, dead and added new, added new
    {
        //To make sure combat styles will be applied on 1st cycle
        if (previousEnemies.empty())
            countSinceLeaderDeath = maxSkipCycles; 

        auto leader = RE::TESForm::LookupByID<RE::Actor>(leaderForm);
        if(leader)
            if (leader->IsDead() && leader->Is3DLoaded())
                countSinceLeaderDeath = 0;

        if (previousEnemies.empty() && enemies.size() >= minimumActors)
            CONSOLE_LOG("[Puppeteer] Roles initialized for first cycle");
        else if (!previousEnemies.empty() && previousEnemies != enemies &&
                consoleUtils::TriggerOnce("DIFF_ENEMIES", previousEnemies != enemies))
            CONSOLE_LOG("[Puppeteer] List of enemies have changed");

        RoleAssignAndTracker();
    }
    
    countSinceLeaderDeath++;

    //Don't update combat styles for X amount of cycles if leader is dead 
    if (countSinceLeaderDeath > maxSkipCycles)
    {
        CombatStyleManager::AssignAndCache(currentRoles, profCollection, rolesMult);
        //CONSOLE_LOG("[Puppeteer] Combat Styles modified!");
    }
    else
    {
        //Revert all actor's roles back to normal
        CombatStyleManager::ReturnCached(currentRoles, profCollection);
        auto leaderCache = RE::TESForm::LookupByID<RE::Actor>(leaderFormCache);
        if (leaderCache && leaderCache->Is3DLoaded() &&
            consoleUtils::TriggerOnce("DEAD_LEADER", leaderCache->IsDead()))
            CONSOLE_LOG("[Puppeteer] Detected Leader's death. Skipping total cycles: {0:d}", maxSkipCycles);
        if(consoleUtils::TriggerOnce("COUNT_1", countSinceLeaderDeath == 1))
            CONSOLE_LOG("[Puppeteer] Combat Styles reverted & disabled for cycle No.{0:d}", countSinceLeaderDeath);
    }
}

void LoadSettings()
{
    using namespace ConfigLoader;

    /*if (minimumActors   != GetMinimumActors() ||
        secondsPerCycle != GetSecondsPerCycle() ||
        maxSkipCycles   != GetSkipCyclesPerCycle()||
        scanDistance    != GetScanDistance())
    {
        minimumActors = GetMinimumActors();
        secondsPerCycle = GetSecondsPerCycle();
        maxSkipCycles = GetSkipCyclesPerCycle();
        scanDistance = GetScanDistance();

        CONSOLE_LOG("[Puppeteer] Cycle Duration: {0:d} | Scan Distance: {1:.1f} | Minimum Actors: {2:d}"
            , secondsPerCycle
            , scanDistance
            , minimumActors);
    }*/

    if (!IfConfigChanged()) {
        return;
    }

    LoadConfig(); //needed for changes after first time loading

    minimumActors = GetMinimumActors();
    secondsPerCycle = GetSecondsPerCycle();
    maxSkipCycles = GetSkipCyclesPerCycle();
    scanDistance = GetScanDistance();
    rolesMult = GetStyleMults();

    CONSOLE_LOG("[Puppeteer] Cycle Duration: {} | Scan Distance: {} | Minimum Actors: {}",
        secondsPerCycle, scanDistance, minimumActors);
}

void AIManager::Initialize()
{
    // Start background task
    std::jthread([] 
    {
        // Wait until the player is fully loaded into the world
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto* player = RE::PlayerCharacter::GetSingleton();
            if (player && player->Is3DLoaded()) {
                
                break;
            }
        }

        while (true) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player)
                continue;
            if (player && !player->Is3DLoaded())
                continue;

            auto console = RE::UI::GetSingleton();
            if (console)
            {
                if (console->IsMenuOpen("Console"))
                {
                    consoleUtils::inspectCBStyleOfSelected(currentRoles);
                }
            }
            
            //using PlayerFlags = RE::PlayerCharacter::PlayerFlags;
            if (player->IsInCombat() && player->Is3DLoaded())
            {
                static auto lastCheck   = std::chrono::steady_clock::now();
                auto        now         = std::chrono::steady_clock::now();
                LoadSettings();

                if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() >= secondsPerCycle) 
                {
                    lastCheck = now;

                    SKSE::GetTaskInterface()->AddUITask
                    ([] 
                    {
                        auto* dataHandler = RE::TESDataHandler::GetSingleton();
                        auto* player = RE::PlayerCharacter::GetSingleton();

                        //Role control has to be called consistenly, even if Puppeteer is disabled
                        RoleControl();
                        /*
                        CONSOLE_LOG("[Puppeteer] roles: {:d}, ogProfile: {:d}, modProfile: {:d}", currentRoles.size(), profCollection.original.size(), profCollection.modified.size());
                        CONSOLE_LOG("[Puppeteer] previous enemies: {:d}, enemies: {:d}, countSinceLeaderDead: {:d}", previousEnemies.size(), enemies.size(), countSinceLeaderDeath);
                        
                        CONSOLE_LOG("Cycle Duration: {0:d} | Scan Distance: {1:f} | Minimum Actors: {2:d} | Available actors: {3:d}"
                            , secondsPerCycle
                            , scanDistance
                            , minimumActors
                            , enemies.size());*/
                        auto leader = RE::TESForm::LookupByID<RE::Actor>(leaderForm);
                        if (!currentRoles.empty() ||            //list of enemies is not empty
                             enemies.size() > minimumActors  //Enemies is more than minimum
                             //(leader && !leader->IsDead() && leader->Is3DLoaded()      //leader is valid not dead (alive)
                        )
                        {
                            if (leader && !leader->IsDead() && leader->Is3DLoaded())
                                Puppeteer::Listen(currentRoles, secondsPerCycle);
                        }   
                    });
                }
                wasInCombat = true;
            } 
            else {
                if (wasInCombat)
                {
                    wasInCombat = false;
                    CONSOLE_LOG("Player exited combat. Stopping scan loop.");
                    //CONSOLE_LOG("[Puppeteer] BEFORE cleanup. roles: {:d}, ogProfile: {:d}, modProfile: {:d}", currentRoles.size(), profCollection.original.size(), profCollection.modified.size());
                    ResetSystem();
                    //CombatStyleManager::ReturnCached(currentRoles, profCollection);
                    //if (!currentRoles.empty())
                    //    currentRoles.clear();
                    //if (leaderForm) leaderForm = 0;
                    //if (leaderFormCache) leaderFormCache = 0;
                    //ActorUtils::DeadActorsCleanup(currentRoles, profCollection, player->IsInCombat());
                    //CONSOLE_LOG("[Puppeteer] AFTER cleanup. roles: {:d}, ogProfile: {:d}, modProfile: {:d}", currentRoles.size(), profCollection.original.size(), profCollection.modified.size());
                }
            }

        }
    }).detach();
}

//void AIManager::Update(RE::PlayerCharacter, float secondsSinceUpdate)
//{
//    if(static_cast<int>(secondsSinceUpdate) % 10 == 0)
//        CONSOLE_LOG("[DELTA] second: {:.2f}");
//}




