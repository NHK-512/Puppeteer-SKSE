#pragma once
#include <fmt/core.h>
#include <RE/Skyrim.h>

inline void ConsoleLogF(const std::string& msg)
{
    if (auto console = RE::ConsoleLog::GetSingleton(); console) {
        console->Print(msg.c_str());
    }
}

template <typename... Args>
inline void ConsoleLogF(fmt::format_string<Args...> fmt, Args&&... args)
{
    ConsoleLogF(fmt::format(fmt, std::forward<Args>(args)...));
}
