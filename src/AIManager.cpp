#include "AIManager.h"
#include "utils/EnemyScanner.h"
#include "utils/Puppeteer.h"

static void RoleAssignAndTracker()
{
    currentRoles = Puppeteer::AssignRoles(enemies);

    if (currentRoles.empty())
    {
        CONSOLE_LOG("List of roles is empty. Returning!");
        return;
    }

    currentLeaderID = currentRoles.begin()->first;

    for (auto enemy : enemies)
    {
        previousEnemies.insert(enemy);
    }

    CONSOLE_LOG("Roles initialized");
}

static void RoleControl()
{
    //Always scans in every cycle
    //Scanner gets list of humanoid hostiles near player
    enemies = EnemyScanner::GetHostileNPCsNearPlayer(scanDistance);

    //If starting the fight for the first time
    if (previousEnemies.empty())
    {
        RoleAssignAndTracker();
        return;
    }
    //Check for new enemies who joins the fight
    else
    {
        for (auto& id : enemies) {
            if (!previousEnemies.contains(id)) {

                RoleAssignAndTracker();

                CONSOLE_LOG("Roles reassigned due to new hostiles");
                return;
            }
        }
    }


    //checks if the leader is dead
    if (currentLeaderID != 0) {
        leader = RE::TESForm::LookupByID(currentLeaderID)->As<RE::Actor>();
        if (!leader || leader->IsDead()) {
            RoleAssignAndTracker();
            CONSOLE_LOG("Roles are reassigned due to lost of Leader");
            return;
        }
    }
}

void AIManager::Initialize()
{
    CONSOLE_LOG("AIManager Initialized.");

    //wasInCombat = false;

    // Start background task
    std::jthread([] 
    {
        // Wait until the player is fully loaded into the world
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            auto* player = RE::PlayerCharacter::GetSingleton();
            if (player && player->Is3DLoaded()) {
                CONSOLE_LOG("Player is fully loaded. Starting AI manager loop.");
                break;
            }
        }

        while (true) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player || !player->Is3DLoaded())
                continue;

            if (player->IsInCombat()) 
            {
                // Do AI scanning task every few seconds while in combat
                static auto lastCheck   = std::chrono::steady_clock::now();
                auto        now         = std::chrono::steady_clock::now();

                //Checks for 5 seconds since last check and call the function (runs every secondsPerCycle, adjust in header file)
                //Change 5 for another number if need to be more or less frequent
                if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() >= secondsPerCycle) 
                {
                    cycleCount++;
                    CONSOLE_LOG("Player entered combat. Starting NPC scan loop. {:x} Cycle. Cycle Duration: {:d}", cycleCount, secondsPerCycle);

                    lastCheck = now;

                    SKSE::GetTaskInterface()->AddUITask
                    ([] 
                    {
                        auto* dataHandler = RE::TESDataHandler::GetSingleton();

                        //if a leader has already been assigned
                        if (currentLeaderID != 0)
                        {
                            leader = RE::TESForm::LookupByID(currentLeaderID)->As<RE::Actor>();

                            //checks if the leader exists or is dead
                            if (!leader || leader->IsDead())
                            {
                                shutoffPuppeteer = true;
                                CONSOLE_LOG("Leader is confirmed dead. Puppeteer disabled for this cycle");
                            }
                        }

                        auto* player = RE::PlayerCharacter::GetSingleton();

                        //Role control has to be called consistenly, even if Puppeteer is disabled
                        RoleControl();
                        ActorUtils::FlushDeadActorsFromRoles(currentRoles);

                        if (player && shutoffPuppeteer == false) //if player is valid & Puppeteer is allowed to run
                        {

                            //start Vanguard formation for when more than 1 vangs are present

                            Puppeteer::Listen(currentRoles, secondsPerCycle);

                            //CONSOLE_LOG("Puppeteer is fully functional");
                        }

                        //Turns Puppeteer functions back on after skipping its operations 
                        //due to leader's death
                        if (shutoffPuppeteer == true)
                        {
                            shutoffPuppeteer = false;
                            CONSOLE_LOG("Puppeteer is turned back on for next cycle.");
                        }
                    });


                }

                wasInCombat = true;

            } else {
                if (wasInCombat)
                {
                    CONSOLE_LOG("Player exited combat. Stopping scan loop.");
                    wasInCombat = false;
                    cycleCount = 0;
                    ActorUtils::FlushDeadActorsFromRoles(currentRoles);
                }
            }

        }
    }).detach();
}




