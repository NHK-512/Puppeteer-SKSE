#include "Puppeteer.h"

std::unordered_map<RE::FormID, char> Puppeteer::AssignRoles(const std::vector<RE::FormID>& npcIDs)
{
    std::unordered_map<RE::FormID, char> roleList;
    std::vector<RE::Actor*> hostiles;

    for (auto formID : npcIDs) {
        auto* form = RE::TESForm::LookupByID(formID);
        if (form) {
            if (auto* actor = form->As<RE::Actor>()) {
                hostiles.push_back(actor);
            }
        }
    }

    if (hostiles.empty()) {
        return;
    }

    // --- Assign Leader ---
    auto comparator = [](RE::Actor* a, RE::Actor* b) 
    {
        const auto avA = a->AsActorValueOwner();
        const auto avB = b->AsActorValueOwner();

        float levelA = a->GetLevel();/*avA->GetActorValue(RE::ActorValue::kLevel);*/
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

    auto sorted = hostiles;
    std::sort(sorted.begin(), sorted.end(), comparator);

    RE::Actor* leader = sorted.front();
    //Leader::Assign(leader);
    roleList[leader->GetFormID()] = 'L';


    // --- Assign Ranger ---
    //std::vector<RE::Actor*> assignedRangers;

    for (auto* actor : hostiles) {
        if (actor == leader) continue;

        const auto inv = actor->GetInventory();
        for (const auto& [item, data] : inv) {
            if (!item || !data.second) continue;
            if (data.first <= 0) continue;

            if (const auto weap = item->As<RE::TESObjectWEAP>()) {
                if (weap->HasKeywordString("WeapTypeBow") || weap->HasKeywordString("WeapTypeCrossbow")) 
                {
                    //Ranger::Assign(actor);
                    //assignedRangers.push_back(actor);
                    roleList[actor->GetFormID()] = 'R';
                    break;
                }
            }
        }
    }




    // --- Assign Vanguard ---
    std::vector<RE::Actor*> assignedVang;
    assignedVang.push_back(leader);

    for (auto* actor : hostiles) {
        if (actor == leader) continue;

        const auto inv = actor->GetInventory();

        for (const auto& [item, data] : inv) {
            if (!item || !data.second) continue;
            if (data.first <= 0) continue;

            if (const auto armor = item->As<RE::TESObjectARMO>()) {
                if (armor->HasKeywordString("ArmorShield")) {
                    //Vanguard::Assign(actor);
                    assignedVang.push_back(actor);
                    roleList[actor->GetFormID()] = 'V';
                    break;
                }
            }
        }
    }

    if (std::count_if(assignedVang.begin(), assignedVang.end(), [&](auto* a) { return a != leader; }) == 0) {
        for (auto* actor : hostiles) {
            if (actor == leader) continue;

            const auto inv = actor->GetInventory();
            for (const auto& [item, data] : inv) {
                if (!item || !data.second) continue;
                if (data.first <= 0) continue;

                if (const auto weap = item->As<RE::TESObjectWEAP>()) {
                    if (weap->IsTwoHandedSword() || weap->IsTwoHandedAxe()) {
                        //Vanguard::Assign(actor);
                        assignedVang.push_back(actor);
                        roleList[actor->GetFormID()] = 'V';
                        break;
                    }
                }
            }
        }
    }

    // --- Assign Vanguard fallback ---
    for (auto* actor : hostiles) {
        if (std::find(assignedVang.begin(), assignedVang.end(), actor) == assignedVang.end()) {
            //Vanguard::Assign(actor);
            //assignedVang.push_back(actor);
            roleList[actor->GetFormID()] = 'V';
        }
    }

    auto* player = RE::PlayerCharacter::GetSingleton();
    //Ranger::AdjustRangerTactics(assignedRangers, assignedVang, player);

    // --- Assign Striker ---
    for (auto* actor : hostiles) {
        if (actor == leader) continue;

        const auto inv = actor->GetInventory();
        int oneHandedCount = 0;

        for (const auto& [item, data] : inv) {
            if (!item || !data.second) continue;
            if (data.first <= 0) continue;

            if (const auto weap = item->As<RE::TESObjectWEAP>()) {
                if (weap->IsOneHandedSword() ||
                    weap->IsOneHandedAxe() ||
                    weap->IsOneHandedDagger() ||
                    weap->IsOneHandedMace()) {
                    oneHandedCount++;
                }
            }
        }

        if (oneHandedCount >= 2) {
            roleList[actor->GetFormID()] = 'S';
            //Striker::Assign(actor);
        }
    }

    return roleList;
}