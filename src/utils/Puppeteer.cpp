#include "Puppeteer.h"

bool isRangerOrVanguard(const std::unordered_map<RE::FormID, char>& roleList, RE::Actor* npc)
{
    //if the npc in the list a ranger (R), returns false = skip
    if (roleList.find(npc->GetFormID())->second == 'R' ||
        roleList.find(npc->GetFormID())->second == 'V')
        return 0;

    return 1;
}

void Puppeteer::AssignRoles(
    const std::vector<RE::FormID>& npcIDs, 
    std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& assignedNPCs
)
{
    if (npcIDs.empty())
        return;

    std::vector<RE::Actor*> hostiles;

    for (auto formID : npcIDs) {
        auto* form = RE::TESForm::LookupByID(formID);
        if (form) {
            auto actor = form->As<RE::Actor>(); 
            if(actor && (actor->Is3DLoaded() && !actor->IsDead()))
            {
                hostiles.push_back(actor);
            }
        }
    }

    if (hostiles.empty()) {
        return;//roleList;
    }

    RE::Actor* leader = nullptr;
    for (auto* actor : hostiles)
    {
        if (!leader && Leader::AssignRole(actor, assignedNPCs, leader)) continue;
        if (Ranger::AssignRole(actor, assignedNPCs)) continue;
        if (Caster::AssignRole(actor, assignedNPCs)) continue;
        if (Vanguard::AssignRole(actor, assignedNPCs)) continue;
        //fallback if NPC don't fit with any role
        if (assignedNPCs.find(actor->GetFormID()) == assignedNPCs.end())
            assignedNPCs[actor->GetFormID()].role = 'V';
    }

    //fallback if leader is not found
    if (!leader)
        leader = *hostiles.begin();
    assignedNPCs[leader->GetFormID()].role = 'L';
}

bool ifRoleIsDead(const std::vector<RE::Actor*>& actors)
{
    int deathCount = 0;

    if (actors.size() == 0)
        return 0;

    for (int i = 0; i < actors.size(); i++)
    {
        if (actors[i]->IsDead())
        {
            deathCount++;
        }
    }

    if (deathCount == actors.size())
        return 1;
    return 0;
}

#pragma region Ranger Utilities
void rangerKeepDistance(
    const std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& roles,
    float minDistance,
    RE::PlayerCharacter*& player
)
{
    for (const auto& npc : roles)
    {
        //Only include Rangers
        if (npc.second.role != 'R') continue;
        auto* form = RE::TESForm::LookupByID(npc.first);
        if (!form) continue;
        auto actor = form->As<RE::Actor>();
        if (!actor) continue;
        if (actor->IsDead())	continue;

        float dist = ActorUtils::GetDistanceBetweenTargets(actor, player);
        if (dist > minDistance) continue;

        actor->InitiateFlee(player, 1, 1, 1, nullptr, nullptr, minDistance, minDistance * 1.2);
    }
}

struct BowEntry {
    RE::TESObjectREFR* ref;
    float distanceSquared;
};

RE::TESObjectREFR* getClosestBowToActor(RE::Actor* npc)
{
    if (!npc) {
        return nullptr;
    }

    const auto npcPos = npc->GetPosition();
    std::vector<BowEntry> foundBows;

    auto [formMap, lock] = RE::TESForm::GetAllForms();
    if (!formMap) {
        return nullptr;
    }

    RE::BSReadWriteLock locker(lock.get());
    for (auto& [formID, form] : *formMap) {
        if (!form) {
            continue;
        }

        // We're only interested in placed object references
        auto* ref = skyrim_cast<RE::TESObjectREFR*>(form);
        if (!ref || !ref->Is3DLoaded() || ref->IsDisabled()) {
            continue;
        }

        auto* base = ref->GetBaseObject();
        auto* weapon = base ? base->As<RE::TESObjectWEAP>() : nullptr;
        if (!weapon || !weapon->IsBow()) {
            continue;
        }

        float distSq = ref->GetPosition().GetSquaredDistance(npcPos);
        foundBows.push_back({ ref, distSq });
    }

    if (foundBows.empty()) {
        return nullptr;
    }

    auto closest = std::min_element(
        foundBows.begin(),
        foundBows.end(),
        [](const BowEntry& a, const BowEntry& b) {
            return a.distanceSquared < b.distanceSquared;
        });

    return closest->ref;
}

RE::TESAmmo* getArrowRefFromRangerCorpse(RE::Actor* aliveActor, std::vector<RE::Actor*> rangers)
{
    RE::Actor* corpse = ActorUtils::getClosestActorToActor(aliveActor, rangers);

    const auto inv = corpse->GetInventory();
    for (const auto& [item, data] : inv) {
        if (!item || !data.second) continue;
        if (data.first <= 0) continue;

        if (const auto arrow = item->As<RE::TESAmmo>()) {
            //return arrow;
            return RE::TESForm::LookupByID(arrow->GetFormID())->As<RE::TESAmmo>();
        }
    }

    return nullptr;
}

RE::TESObjectWEAP* getBowFromRangerBody(RE::Actor* aliveActor, std::vector<RE::Actor*> rangers)
{
    RE::Actor* corpse = ActorUtils::getClosestActorToActor(aliveActor, rangers);

    const auto inv = corpse->GetInventory();
    for (const auto& [item, data] : inv) {
        if (!item || !data.second) continue;
        if (data.first <= 0) continue;

        if (const auto weap = item->As<RE::TESObjectWEAP>()) {
            if (weap->HasKeywordString("WeapTypeBow") || weap->HasKeywordString("WeapTypeCrossbow"))
                //return weap;
                return RE::TESForm::LookupByID(weap->GetFormID())->As<RE::TESObjectWEAP>();
        }
    }

    return nullptr;
}

RE::TESAmmo* getArrowRefFromRangerCorpse_2(RE::Actor* aliveActor, std::vector<RE::Actor*> rangers)
{
    RE::Actor* corpse = ActorUtils::getClosestActorToActor(aliveActor, rangers);

    return corpse->GetCurrentAmmo();
}

void RangerCheckAndReplace(std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& roles)
{
    auto vangList = ActorUtils::extractActorsFromRoles(roles, 'V');
    auto rangList = ActorUtils::extractActorsFromRoles(roles, 'R');

    //checking if all Rangers are dead or not
    if (!ifRoleIsDead(rangList))
    {
        return;
    }
    consoleUtils::Log("[Puppeteer] All Rangers are dead. Attempting replace with Vanguard");

    //checking if any Vanguards are alive
    if (vangList.empty() ||
        ifRoleIsDead(vangList)
    )
    {
        if(consoleUtils::TriggerOnce("NO_VANGS", (vangList.empty() || ifRoleIsDead(vangList))))
            consoleUtils::Log("[Puppeteer] No Vanguards available. Skipping replacement!");
        return;
    }

#pragma region declarations
    auto vang = vangList.front();
    auto equipManager = RE::ActorEquipManager::GetSingleton();
    auto* bowWeap = getBowFromRangerBody(vang, rangList);
    auto* bowRef = getClosestBowToActor(vang);
#pragma endregion
    //CONSOLE_LOG("Declaration complete.");

#pragma region reassign to ranger
    auto vangID = vang->GetFormID();
    if (roles.find(vangID) != roles.end())
    {
        roles.find(vangID)->second.role = 'R';
        //CONSOLE_LOG("Reassignment complete");
    }
    else
    {
        //CONSOLE_LOG("Couldn't find vanguard with FormID:{:X}, exiting reassignment.", vangID);
        return;
    }
#pragma endregion


#pragma region Bow equip
    if (bowRef)
    {
        vang->PickUpObject(bowRef, 1, 0, 1);
    }
    else if (bowWeap)
    {
        //Add bow to inv
        vang->AddObjectToContainer(bowWeap, nullptr, 1, nullptr);

        //Unequip main weap
        auto melee = vang->GetEquippedObject(true); // true = right hand
        if (melee) {
            equipManager->UnequipObject(vang,
                RE::TESForm::LookupByID(melee->GetFormID())->As<RE::TESObjectWEAP>());
        }

        //Equip Bow
        equipManager->EquipObject(vang, bowWeap);
        //CONSOLE_LOG("Bow equip successful");
    }
#pragma endregion


#pragma region Arrow equip

    auto* arrowRef = getArrowRefFromRangerCorpse(vang, rangList);

    if (!arrowRef)
    {
        arrowRef = getArrowRefFromRangerCorpse_2(vang, rangList);
        if (!arrowRef)
            arrowRef = RE::TESForm::LookupByID<RE::TESAmmo>(0x1397D);
    }

    if (arrowRef)
    {
        vang->AddObjectToContainer(arrowRef, nullptr, 1, nullptr);
        equipManager->EquipObject(vang, arrowRef);
        //CONSOLE_LOG("Arrow equip successful");
    }
    //else
        //CONSOLE_LOG("Arrow equip unsuccessful...");

#pragma endregion
}

#pragma endregion

void Puppeteer::executeTactics
(   std::unordered_map<RE::FormID, CombatData::npcCombatInfo>& roles
,   RE::PlayerCharacter*& a_player
)
{
    if (ConfigLoader::GetRangTakeCoverFeature())
    {
        //Calc the max vanguard distance once a cycle here
        
        rangerKeepDistance
        (   roles
        ,   ActorUtils::GetDistanceFurthestVanguard(roles, a_player)
        ,   a_player
        );
        //CONSOLE_LOG("[Puppeteer] Ranger role is taking cover!");
    }
    if (ConfigLoader::GetVangReplaceRang())
    {
        RangerCheckAndReplace(roles);
        //CONSOLE_LOG("[Puppeteer] Attempting Ranger replacement from Vanguard!");
    }
}