#include "Puppeteer.h"

bool skipAssignedRanger(const std::unordered_map<RE::FormID, char>& roleList, RE::Actor* npc)
{
    //if the npc in the list a ranger (R), returns false = skip
    if (roleList.find(npc->GetFormID())->second == 'R')
        return 0;

    return 1;
}

//std::unordered_map<RE::FormID, char> 
void Puppeteer::AssignRoles(const std::vector<RE::FormID>& npcIDs, std::unordered_map<RE::FormID, char>& assignedNPCs)
{
    if (npcIDs.empty())
        return;
    //std::unordered_map<RE::FormID, char> roleList;
    std::vector<RE::Actor*> hostiles;

    for (auto formID : npcIDs) {
        auto* form = RE::TESForm::LookupByID(formID);
        if (form) {
            auto actor = form->As<RE::Actor>();
            //if (auto* actor = form->As<RE::Actor>() && actor-) 
            if(actor && (actor->Is3DLoaded() && !actor->IsDead()))
            {
                hostiles.push_back(actor);
            }
        }
    }

    if (hostiles.empty()) {
        return;//roleList;
    }



    // --- Assign Leader ---
    auto comparator = [](RE::Actor* a, RE::Actor* b) 
    {
        const auto avA = a->AsActorValueOwner();
        const auto avB = b->AsActorValueOwner();

        float levelA = a->GetLevel();
        float levelB = b->GetLevel();
        if (levelA != levelB)
            return levelA > levelB;

        float armorA = avA->GetActorValue(RE::ActorValue::kDamageResist);
        float armorB = avB->GetActorValue(RE::ActorValue::kDamageResist);
        if (armorA != armorB)
            return armorA > armorB;

        auto weaponA = a->GetEquippedObject(true);
        auto weaponB = b->GetEquippedObject(true);

        float dmgA = 0.0f;
        float dmgB = 0.0f;
        if (weaponA) 
        {
            if (auto weap = weaponA->As<RE::TESObjectWEAP>()) 
            {
                dmgA = weap->attackDamage;
            }
        }
        if (weaponB) 
        {
            if (auto weap = weaponB->As<RE::TESObjectWEAP>()) 
            {
                dmgB = weap->attackDamage;
            }
        }
        return dmgA > dmgB;
    };

    /*auto sorted = hostiles;
    std::sort(sorted.begin(), sorted.end(), comparator);

    RE::Actor* leader = sorted.front();
    Leader::Assign(leader);*/

    RE::Actor* leader = nullptr;
    for (auto* actor : hostiles) {
        if (!leader || comparator(actor, leader)) {
            leader = actor;
        }
    } //thanks for the suggested code, DavidJCobb
    if (!leader)
        leader = *hostiles.begin();
    assignedNPCs[leader->GetFormID()] = 'L';

    // --- Assign Ranger ---
    for (auto* actor : hostiles) {
        if (actor == leader) continue;

        const auto inv = actor->GetInventory();
        for (const auto& [item, data] : inv) {
            if (!item || !data.second) continue;
            if (data.first <= 0) continue;

            if (const auto weap = item->As<RE::TESObjectWEAP>()) {
                if (weap->HasKeywordString("WeapTypeBow") || weap->HasKeywordString("WeapTypeCrossbow")) 
                {
                    assignedNPCs[actor->GetFormID()] = 'R';
                    break;
                }
            }
        }
    }

    // --- Assign Vanguard ---
    std::vector<RE::Actor*> assignedVang;
    assignedVang.push_back(leader);

    //pick only actors inside hostiles with a shield 
    // and push inside assignedVang vector to exclude later
    for (auto* actor : hostiles) {
        if (actor == leader) continue;

        const auto inv = actor->GetInventory();

        for (const auto& [item, data] : inv) {
            if (!item || !data.second) continue;
            if (data.first <= 0) continue;

            if (const auto armor = item->As<RE::TESObjectARMO>()) {
                if (armor->HasKeywordString("ArmorShield")) {
                    assignedVang.push_back(actor);
                    assignedNPCs[actor->GetFormID()] = 'V';
                    break;
                }
            }
        }
    }

    if (std::count_if(assignedVang.begin(), 
        assignedVang.end(), 
        [&](auto* a) 
        { return a != leader; }) == 0) 
    {
        for (auto* actor : hostiles) {
            if (actor == leader) continue;

            const auto inv = actor->GetInventory();
            for (const auto& [item, data] : inv) {
                if (!item || !data.second) continue;
                if (data.first <= 0) continue;

                if (const auto weap = item->As<RE::TESObjectWEAP>()) {
                    if (weap->IsTwoHandedSword() || weap->IsTwoHandedAxe()) {
                        assignedVang.push_back(actor);
                        assignedNPCs[actor->GetFormID()] = 'V';
                        break;
                    }
                }
            }
        }
    }

    // --- Assign Vanguard fallback ---
    for (auto* actor : hostiles) {
        if (std::find(assignedVang.begin(), assignedVang.end(), actor) == assignedVang.end() &&
            skipAssignedRanger(assignedNPCs, actor))
        {
            assignedNPCs[actor->GetFormID()] = 'V';
        }
    }
    //return roleList;
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
void Puppeteer::rangerKeepDistance(const std::unordered_map<RE::FormID, char>& roles, RE::PlayerCharacter* player)
{
    std::vector<RE::Actor*> rangers = ActorUtils::extractActorsFromRoles(roles, 'R');
    if (rangers.empty() ||
        ifRoleIsDead(rangers))
    {
        return;
    }

    for (int iR = 0; iR < rangers.size(); iR++)
    {
        Ranger::KeepDistanceAwayPlayer(
            rangers[iR],
            ActorUtils::extractActorsFromRoles(roles, 'V'),
            player
        );
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

void RangerCheckAndReplace(std::unordered_map<RE::FormID, char>& roles)
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
        roles.find(vangID)->second = 'R';
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

void Puppeteer::Listen(std::unordered_map<RE::FormID, char> &roles, int cycleTime)
{  
    std::jthread([cycleTime, &roles]() {
        using namespace std::chrono_literals;

        for (int i = 0; i < (cycleTime-1) * 2; ++i) {  // check for up to 10 seconds
            std::this_thread::sleep_for(500ms);

            SKSE::GetTaskInterface()->AddUITask([i, cycleTime, &roles]()
            {
                auto* player = RE::PlayerCharacter::GetSingleton();
                if(roles.size() > 1)
                {
                    Puppeteer::rangerKeepDistance(roles, player);
                    RangerCheckAndReplace(roles);
                }
            });
            
        }
    }).detach();
}