#include "ConsoleUtils.h"

#pragma region tFlipFlop
std::unordered_map<std::string, bool>& states()
{
    static std::unordered_map<std::string, bool> s;
    return s;
}

bool consoleUtils::TriggerOnce(const char* key, bool condition)
{
    bool& state = states()[key];

    if (condition) {
        if (!state) {
            state = true;     // Mark as triggered
            return true;      // Report first-time trigger
        }
        return false;         // Already triggered earlier
    }
    else {
        state = false;        // Reset when condition is false
        return false;
    }
}

#pragma endregion

void consoleUtils::Log(const std::string& msg)
{
    // Safety: check console exists
    if (auto console = RE::ConsoleLog::GetSingleton(); console) {
        console->Print(msg.c_str());
    }
}

void consoleUtils::inspectCBStyleOfSelected(std::unordered_map<RE::FormID, char> roleList)
{
    auto target = RE::Console::GetSelectedRefHandle();

    if (cachedTarget && cachedTarget == target)
        return;

    if (!target)
    {
        return;
    }

    auto targetRef = target.get().get();
    if (!targetRef)
    {
        if(TriggerOnce("INVALID_SELECT", !targetRef))
            Log("[Puppeteer] Target Reference is invalid");
        return;
    }

    RE::Actor* actor = targetRef->As<RE::Actor>();
    if (!actor)
    {
        if (TriggerOnce("INVALID_ACTOR", !actor))
            Log("[Puppeteer] Target Reference is invalid");
        return;
    }

    auto formID = actor->GetFormID();
    if (!roleList.empty())
        Log("[Puppeteer] {:x} | {} | {}"
            , formID
            , actor->GetDisplayFullName()
            , roleList.find(formID)->second);
    else
        Log("[Puppeteer] {:x} | {} "
            , formID
            , actor->GetDisplayFullName());

    auto style = actor->GetActorBase()->GetCombatStyle();
    Log("Offensive: {:.2f} | Defensive: {:.2f} | grpOffensive: {:.2f} | avoidThreatChance: {:.2f}"
        , style->generalData.offensiveMult
        , style->generalData.defensiveMult
        , style->generalData.groupOffensiveMult
        , style->generalData.avoidThreatChance
    );

    Log("Circle: {:.2f} | Flank: {:.2f} | Fallback: {:.2f} | Stalk: {:.2f} | Strafe: {:.2f}"
        , style->closeRangeData.circleMult
        , style->closeRangeData.flankDistanceMult
        , style->closeRangeData.fallbackMult
        , style->closeRangeData.stalkTimeMult
        , style->longRangeData.strafeMult
    );
    
    cachedTarget = target;
}
