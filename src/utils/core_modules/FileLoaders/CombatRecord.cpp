#include "CombatRecord.h"

void CombatDataManager::CreateBlank()
{
    root["size"] = 20; //default size
    root["battles"] = json::array();

    Save();
}

//Load everytime in order to write to existing JSON
void CombatDataManager::Load()
{
    std::ifstream in(filename);
    if (in)
    {
        in >> root;
    }
    else
    {
        CreateBlank();
    }
}

void CombatDataManager::calculateRoleBias()
{
    //3 values needed for calculating bias
    //1. sum of EACH roles killed (ranger, vanguard, caster)
    //2. sum of ALL roles killed (ranger + vanguard + caster) = total
    //3. expected value = total / 3 (3 roles)
    
    int sumRanger = 0;
    int sumVanguard = 0;
    int sumCaster = 0;

    for (const auto& b : root["battles"])
    {
        //skips if groupData is not found = no records written
        if (!b.contains("groupData"))
            break;

        if (b["groupData"].contains("ranger"))
        {
            sumRanger   += b.value("ranger", 0);
        }
        if (b["groupData"].contains("Vanguard"))
        {
            sumVanguard += b.value("Vanguard", 0);
        }
        if (b["groupData"].contains("Caster"))
        {
            sumCaster   += b.value("Caster", 0);
        }
    }

    if (sumCaster == 0 && sumRanger == 0 && sumVanguard == 0)
        return;

    double total = sumCaster + sumRanger + sumVanguard;
    double expected = total / 3.0; //3 is the amount of roles

    rangerBias = (sumRanger - expected) / expected;
    vanguardBias = (sumVanguard - expected) / expected;
    casterBias = (sumCaster - expected) / expected;
}

void CombatDataManager::Save()
{
    std::ofstream out(filename);
    out << root.dump(4);
}

double CombatDataManager::GetAverage(const std::string& field)
{
    double total = 0;
    int count = 0;

    for (auto& b : root["battles"])
    {
        if (field == "duration")
            total += b["outcome"]["duration"];

        else if (field == "dead")
            total += b["groupData"]["dead"];

        else if (field == "alive")
            total += b["groupData"]["alive"];

        else if (field == "melee")
            total += b["playerCombatPattern"]["melee"];

        else if (field == "range")
            total += b["playerCombatPattern"]["range"];

        else if (field == "magic")
            total += b["playerCombatPattern"]["magic"];

        else if (field == "playerDmg")
            total += b["playerCombatPattern"]["dmgDealt"];

        count++;
    }

    if (count == 0) return 0;

    return total / count;
}

void CombatDataManager::CommitRecord()
{
    json entry;

    entry["outcome"] =
    {
        {"playerWins", pendingRecord.playerWins},
        {"duration", pendingRecord.duration}
    };

    entry["groupData"] =
    {
        {"dead", pendingRecord.dead},
        {"alive", pendingRecord.alive},
        {"dmgDealt", pendingRecord.groupDmg},
        {"ranger", pendingRecord.ranger },
        {"vanguard", pendingRecord.vanguard},
        {"caster", pendingRecord.caster}
    };

    entry["playerCombatPattern"] =
    {
        {"melee", pendingRecord.melee},
        {"range", pendingRecord.range},
        //{"magic", pendingRecord.magic},       //Add later when figured out how to know
        //{"physical", pendingRecord.physical}, // if an attack is physical or magic
        {"dmgDealt", pendingRecord.playerDmg}
    };

    if (root.empty())    CreateBlank();

    size_t maxSize = root["size"];

    // Remove oldest entries if exceeding limit
    while (root["battles"].size() >= maxSize && maxSize > 0)
    {
        root["battles"].erase(root["battles"].begin());
    }

    // Add new record
    root["battles"].push_back(entry);

    Save();
}

