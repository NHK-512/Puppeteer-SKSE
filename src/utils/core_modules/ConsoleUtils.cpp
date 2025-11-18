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

