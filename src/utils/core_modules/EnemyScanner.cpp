#include "EnemyScanner.h"

void EnemyScanner::GetHostileNPCsNearPlayer(float radius, std::vector<RE::FormID>& enemies)
{

    auto* player = RE::PlayerCharacter::GetSingleton();
    if (!player)
        return;

    const auto playerPos = player->GetPosition();

    // Iterate over all loaded actors (high, low, middle processes)
    for (auto* actor : GetNearbyAggroedActors(radius))
    {
        if (!actor || actor == player)
            continue;

        if (!actor->GetRace()->GetPlayable())
            continue;

        if (!actor->IsHostileToActor(player))
            continue;

        if (actor->IsDead())
            continue;

        if (actor->AsActorState()->IsBleedingOut())
            continue;

        if (actor->IsInKillMove())
            continue;

        const auto dist = playerPos.GetDistance(actor->GetPosition());
        if (dist <= radius &&
            std::find(enemies.begin(), enemies.end(), actor->GetFormID()) == enemies.end())
        {
            enemies.push_back(actor->GetFormID());
        }
    }
}

bool EnemyScanner::isOneEnemyInstantKilled(std::vector<Puppeteer::survivalTime>& survivalTimes)
{
    //extra note: timer of each enemy is reset every Puppeteer's cycle. May or may not be intentional
    static RE::Actor* actor;
    static int killTime = ConfigLoader::GetInstantKillTime();
    for (auto npc = survivalTimes.begin(); npc != survivalTimes.end(); npc++)
    {
        actor = RE::TESForm::LookupByID<RE::Actor>(npc->formID);
        //move on if actor is invalid
        if (!actor) continue;
        //enable counter if actor is attacked
        if (ActorUtils::dmgTaken(actor))
        {
            CONSOLE_LOG("[Puppeteer] Enemy is attacked for the first time");
            npc->isAttacked = true;
        }
        //true if instantly killed
        if (npc->seconds < killTime && actor->IsDead())
        {
            CONSOLE_LOG("[Puppeteer] Survival Time: {:d}", npc->seconds);
            survivalTimes.erase(npc);
            return true;
        }

        if(npc->isAttacked) npc->seconds++;
    }

    return false;
}


//    
