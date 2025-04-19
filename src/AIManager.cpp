#include "AIManager.h"
#include "utils/EnemyScanner.h"

void AIManager::Initialize()
{
    SKSE::log::info("AI Manager Initialized.");
    CONSOLE_LOG("AI Manager Initialized.");

    // Start background task
    std::jthread([] 
    {
        bool wasInCombat = false;

        while (true) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player)
                continue;

            if (player->IsInCombat()) 
            {
                if (!wasInCombat) 
                {
                    CONSOLE_LOG("Player entered combat. Starting AI scan loop.");
                    wasInCombat = true;
                }

                // Do AI scanning task every few seconds while in combat
                static auto lastCheck = std::chrono::steady_clock::now();
                auto now = std::chrono::steady_clock::now();

                //Checks for 5 seconds since last check and call the function (runs every 5 sec)
                //Change 5 for another number if need to be more or less frequent
                if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count() >= 5) 
                {
                    lastCheck = now;

                    SKSE::GetTaskInterface()->AddUITask
                    ([] 
                    {
                        auto* player = RE::PlayerCharacter::GetSingleton();
                        if (player) 
                        {
                            AIManager::ModifyAI(player);
                            CONSOLE_LOG("ModifyAI was called (in combat)");
                        }
                    });
                }

                /*SKSE::GetTaskInterface()->AddUITask
                ([] 
                {
                    auto* player = RE::PlayerCharacter::GetSingleton();
                    if (player) 
                    {
                        AIManager::ModifyAI(player);
                        CONSOLE_LOG("ModifyAI was called");
                    }
                });*/

            } else if (wasInCombat) 
            {
                wasInCombat = false;
                CONSOLE_LOG("Player exited combat. Stopping AI scan loop.");
            }

        }
    }).detach();

    //ISSUE: THE LOOP ABOVE WILL CALL EVERY FEW SECONDS AND LOGS IN THE CONSOLE.
    //FIX: MAKE IT SO THE FUNCTION IS ONLY CALLED WHENEVER THE PLAYER IS INVOLVED IN COMBAT
}

void AIManager::ModifyAI(RE::Actor* actor)
{
    //if the actor is invalid or not the player -> breaks
    if (!actor || !actor->IsPlayerRef())
        return;

    //gets list of humanoid hostiles near player
    //input number is max distance as radius of the scanning area
    //IMPORTANT: MAX DISTANCE AS TO BE HIGH (MINIMUM ~500)
    const auto enemies = EnemyScanner::GetHostileNPCsNearPlayer(5000);

    //logs hostiles with their form ID to console
    for (auto formID : enemies) {
        CONSOLE_LOG("Hostile nearby NPC FormID: {:X}", formID);
        SKSE::log::info("Hostile nearby NPC FormID: {:X}", formID);
    }
}

//if (!actor || !actor->Is3DLoaded())
//    return;

//// Just as an example, print the actor's name
//const auto name = actor->GetDisplayFullName();
//SKSE::log::info("AI ModifyAI called on actor: {}", name);

//// Example: Detect allies nearby within a radius (simplified)
//constexpr float radius = 1000.0f;
//auto* cell = actor->GetParentCell();
//if (!cell)
//    return;

//for (auto& ref : cell->GetReferences())
//{
//    auto* other = ref.get().As<RE::Actor>();
//    if (!other || other == actor || !other->IsHostileToActor(actor))
//        continue;

//    //auto message = ;
//    float dist = actor->GetPosition().GetDistance(other->GetPosition());
//    if (dist < radius)
//    {
//        SKSE::log::info("{} sees nearby enemy : {}", name, other->GetDisplayFullName());
//        // Here you'd add actual coordination logic
//    }
//}