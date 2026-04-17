#include "AIManager.h"

void AIManager::LoadSettings()
{
    using namespace ConfigLoader;

    if (!IfConfigChanged()) {
        return;
    }

    LoadConfig(); //needed for changes after first time loading

    minimumActors = GetMinimumActors();
    secondsPerCycle = GetSecondsPerCycle();
    maxSkipCycles = GetSkipCyclesPerCycle();
    scanDistance = GetScanDistance();
    rolesMult = GetRolesInfo();

    CONSOLE_LOG("[Puppeteer] Cycle Duration: {} | Scan Distance: {} | Minimum Actors: {}",
        secondsPerCycle, scanDistance, minimumActors);
}

void AIManager::Initialize()
{
    //seed rand
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::jthread([] {
        player = RE::PlayerCharacter::GetSingleton();
        // Wait until player is fully loaded
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            //auto* player = RE::PlayerCharacter::GetSingleton();
            if (player && player->Is3DLoaded())
            {
                CONSOLE_LOG("[Puppeteer] Player loaded");
                break;
            }
        }

        lastTick = std::chrono::steady_clock::now();

        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto console = RE::UI::GetSingleton();
            if (console)
            {
                if (console->IsMenuOpen("Console"))
                {
                    if (activeSession)
                        consoleUtils::inspectCBStyleOfSelected(activeSession->extractCurrentEnemies());
                    else
                        consoleUtils::inspectCBStyleOfSelected({});
                }
            }

            Update();
        }
    }).detach();
}

void AIManager::Update()
{
    //auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player || !player->Is3DLoaded())
        return;

    //End of combat/Last tick
    if (!player->IsInCombat())
    {
        if (activeSession)
        {
            CONSOLE_LOG("[Puppeteer] Combat ended");
            activeSession->recordDmgData(activeTracker->getTotalDmgData());
            activeTracker.reset();
            activeSession.reset();  
            //all_enemies.clear();
        }

        return;
    }

    SKSE::GetTaskInterface()->AddTask([]()
    {

        auto now = std::chrono::steady_clock::now();
        AIManager::LoadSettings(); // hot-reload config
        PuppeteerConfig cfg{
            scanDistance,
            secondsPerCycle,
            minimumActors,
            maxSkipCycles,
            &rolesMult
        };

        if (!activeSession)
        {
            CONSOLE_LOG("[Puppeteer] Combat started");
            activeSession = std::make_unique<CombatSession>(now);
            activeTracker = std::make_unique<dmgTracker>(player);
        }

        if (/*!all_enemies.empty() &&*/ activeTracker)
        {
            activeTracker->Tick(dmgFlags, player);
        }

#pragma region Per Cycle
        if (std::chrono::duration_cast<std::chrono::seconds>
            (now - lastTick).count() < secondsPerCycle)
        {
            return;
        }

        lastTick = now;

        if (dmgFlags[Flag::IsInstantKilled])
        {
            CONSOLE_LOG("[Puppeteer] Hesitation cycle start.");
            secondsPerCycle = ConfigLoader::GetDeathHesitationDuration();
        }
        else
        {
            secondsPerCycle = ConfigLoader::GetSecondsPerCycle();
        }
#pragma endregion
    
        if (activeSession)
        {   
            activeSession->Tick(player, cfg, dmgFlags);
            dmgFlags.Reset();
            activeTracker->UpdateList(activeSession->extractCurrentEnemies());
        }
            
    });
}




