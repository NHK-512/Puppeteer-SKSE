#pragma once
#include <bitset>
#include <cstddef>

enum class Flag {
    IsTakingMagic,
    IsAttackedLongRange,
    IsAttackedShortRange,
    IsInstantKilled,
    IsPlayerWerewolf,
    IsPlayerVampireLord,

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
};