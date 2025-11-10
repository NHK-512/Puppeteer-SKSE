#include "AIManager.h"

static void RoleAssignAndTracker()
{
    currentRoles = Puppeteer::AssignRoles(enemies);
    //profCollection = CombatStyleManager::AssignAndCache(currentRoles);

    if (currentRoles.empty())
    {
        CONSOLE_LOG("List of roles is empty. Returning!");
        return;
    }

    for (auto actor : currentRoles)
    {
        if (actor.second == 'L')
        {
            currentLeaderID = actor.first; break;
        }
    }
        
    for (auto enemy : enemies)
    {
        previousEnemies.insert(enemy);
    }

    CONSOLE_LOG("Roles and combat styles initialized");
}

static void RoleControl()
{
    //Always scans in every cycle
    //Scanner gets list of humanoid hostiles near player
    scanDistance = ConfigLoader::GetScanDistance();
    //enemies List constant gets overwritten and updated
    enemies = EnemyScanner::GetHostileNPCsNearPlayer(scanDistance);

    //Case 1: If starting the fight for the first time
    if (previousEnemies.empty())
    {
        RoleAssignAndTracker();
        return;
    }

    //Case 2: If new enemies joins the fight
    else
    {
        for (auto& id : enemies) {
            if (!previousEnemies.contains(id)) {

                CombatStyleManager::ReturnCached(profCollection.original);
                RoleAssignAndTracker();

                CONSOLE_LOG("Roles reassigned due to new hostiles");
                return;
            }
        }
    }

    //Case 3: If the leader is dead
    if (currentLeaderID != 0) {
        leader = RE::TESForm::LookupByID(currentLeaderID)->As<RE::Actor>();
        if (!leader || leader->IsDead()) {
            CombatStyleManager::ReturnCached(profCollection.original);
            RoleAssignAndTracker();
            CONSOLE_LOG("Roles are reassigned due to lost of Leader");
            return;
        }
    }

    //Case 4: if there are dead NPCs in the list but the fight still goes on
    if (std::any_of(
        currentRoles.begin(), currentRoles.end(),
        [](const auto& pair) {
            return RE::TESForm::LookupByID<RE::Actor>(pair.first)->IsDead();
        }
    ))
    {
        ////revert dead actor's combat style
        //profCollection = CombatStyleManager::AssignAndCache(currentRoles);
        //flush dead actor from the list of roles
        ActorUtils::DeadActorsCleanup(currentRoles, profCollection.original, false);
    }

    //Case 5: List stays the same, combat style still updates regularly
    profCollection = CombatStyleManager::AssignAndCache(currentRoles);
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

            //shut off entire operation if there are too little enemies
            //if (minimumActors != 0 && !enemies.empty())
            //{
            //    if (enemies.size() < minimumActors && player->IsInCombat())
            //        shutoffAIManager = true;
            //}

            if (player->IsInCombat()) 
            {
                static auto lastCheck   = std::chrono::steady_clock::now();
                auto        now         = std::chrono::steady_clock::now();
                minimumActors = ConfigLoader::GetMinimumActors();
                secondsPerCycle = ConfigLoader::GetSecondsPerCycle();

                //Checks for 5 seconds since last check and call the function (runs every secondsPerCycle, adjust in header file)
                //Change 5 for another number if need to be more or less frequent
                if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() >= secondsPerCycle) 
                {
                    //cycleCount++;
                    CONSOLE_LOG("Cycle Duration: {:d} | Scan Distance: {:f} | Minimum Actors: {:d}", /*cycleCount*/ secondsPerCycle, scanDistance, minimumActors);

                    lastCheck = now;

                    SKSE::GetTaskInterface()->AddUITask
                    ([] 
                    {
                        auto* dataHandler = RE::TESDataHandler::GetSingleton();
                        auto* player = RE::PlayerCharacter::GetSingleton();

                        //Role control has to be called consistenly, even if Puppeteer is disabled
                        RoleControl();

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

                        //if there are less enemies than the set Minimum
                        if (!enemies.empty() && minimumActors != 0)
                        {
                            if (enemies.size() < minimumActors)
                                shutoffPuppeteer = true;
                        }
                        else
                            CONSOLE_LOG("Enemy list and/or minimum actor is empty for eval");

                        //if player is valid & Puppeteer is allowed to run
                        if (player && shutoffPuppeteer == false) 
                        {
                            Puppeteer::Listen(currentRoles, secondsPerCycle);

                            /*CONSOLE_LOG
                            (
                                "Leader: {:X} - {} - Type {}",
                                currentLeaderID,
                                RE::TESForm::LookupByID<RE::Actor>(currentLeaderID)->GetDisplayFullName(),
                                currentRoles.find(currentLeaderID)->second
                            );
                            CONSOLE_LOG(
                                "Offensive: {:F}; Defensive: {:F}; Circling: {:F}",
                                RE::TESForm::LookupByID<RE::Actor>(currentLeaderID)
                                ->GetActorBase()->GetCombatStyle()->generalData.offensiveMult,
                                RE::TESForm::LookupByID<RE::Actor>(currentLeaderID)
                                ->GetActorBase()->GetCombatStyle()->generalData.defensiveMult,
                                RE::TESForm::LookupByID<RE::Actor>(currentLeaderID)
                                ->GetActorBase()->GetCombatStyle()->closeRangeData.circleMult
                            );

                            std::ifstream config("Data/SKSE/Plugins/PuppeteerConfig.json");
                            json j = json::parse(config);

                            CONSOLE_LOG(    
                                "[JSON] Leader's off: {:F}, def: {:F}, circ: {:F}",
                                j["roles"]["Leader"].value("offensive", 0.0f),
                                j["roles"]["Leader"].value("defensive", 0.0f),
                                j["roles"]["Leader"].value("circle", 0.0f)
                            );*/
                        }

                        //Turns Puppeteer functions back on after skipping its operations 
                        //due to leader's death
                        if (shutoffPuppeteer == true)
                        {
                            CombatStyleManager::ReturnCached(profCollection.original);
                            shutoffPuppeteer = false;
                            CONSOLE_LOG("Puppeteer is turned back on for next cycle.");
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
                    //cycleCount = 0;
                    
                    ActorUtils::DeadActorsCleanup(currentRoles, profCollection.original, player->IsInCombat());
                    CombatStyleManager::ReturnCached(profCollection.original);
                }
            }

        }
    }).detach();
}




