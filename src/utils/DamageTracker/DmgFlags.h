#pragma once
//#include <array>
#include <bitset>
#include <cstddef>

enum class Flag {
    IsTakingMagic,
    IsAttackedLongRange,
    IsAttackedShortRange,
    IsInstantKilled,

    Count
};

struct FlagSet {
    std::bitset<static_cast<size_t>(Flag::Count)> data;

    bool operator[](Flag f) const;
    void Set(Flag f, bool value = true);
    void Reset();              // reset all flags
    void Reset(Flag f);        // reset one flag
    bool Any() const;
    bool None() const;
    bool All() const;

    //std::array<bool, static_cast<size_t>(Flag::Count)> data{};

    //bool& operator[](Flag f);
    //bool  operator[](Flag f) const;
};