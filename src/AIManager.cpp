#include "AIManager.h"
#include "utils/EnemyScanner.h"
#include "utils/Puppeteer.h"

void AIManager::Initialize()
{
    CONSOLE_LOG("AIManager Initialized.");

    // Start background task
    std::jthread([] 
    {
        while (true) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player)
                continue;

            if (player->IsInCombat()) 
            {
                CONSOLE_LOG("Player entered combat. Starting AI scan loop.");

                // Do AI scanning task every few seconds while in combat
                static auto lastCheck = std::chrono::steady_clock::now();
                auto now = std::chrono::steady_clock::now();

                //Checks for 5 seconds since last check and call the function (runs every 60 sec)
                //Change 5 for another number if need to be more or less frequent
                if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() >= secondsPerCycle) 
                {
                    lastCheck = now;

                    SKSE::GetTaskInterface()->AddUITask
                    ([] 
                    {
                        auto* player = RE::PlayerCharacter::GetSingleton();
                        if (player) 
                        {
                            AIManager::RoleControl(player);
                            CONSOLE_LOG("RoleControl was called (in combat)");
                        }
                    });
                }

            } else {
                CONSOLE_LOG("Player exited combat. Stopping scan loop.");
            }

        }
    }).detach();
}

void AIManager::RoleControl(RE::Actor* actor)
{
    //Always scans in every cycle
    //Scanner gets list of humanoid hostiles near player
    enemies = EnemyScanner::GetHostileNPCsNearPlayer(scanDistance);

    if (!enemies.empty())
    {
        for (auto& id : enemies) {
            if (!previousEnemies.contains(id)) {
                currentRoles = Puppeteer::AssignRoles(enemies);
                CONSOLE_LOG("Roles reassigned due to new hostiles");
                return;
            }
        }
    }
    else 
    {
        currentRoles = Puppeteer::AssignRoles(enemies);
        currentLeaderID = currentRoles.begin()->first;
        
        for (auto enemy : enemies)
        {
            previousEnemies.insert(enemy);
        }

        CONSOLE_LOG("Roles initialized");
        return;
    }

    //checks if the leader is dead
    if (currentLeaderID != 0) {
        auto leader = RE::TESForm::LookupByID(currentLeaderID)->As<RE::Actor>();
        if (!leader || leader->IsDead()) {
            currentRoles = Puppeteer::AssignRoles(enemies);
            CONSOLE_LOG("Roles are reassigned due to lost of Leader");
            return;
        }
    }
}