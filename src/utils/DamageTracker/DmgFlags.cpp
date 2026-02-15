#include "DmgFlags.h"

void FlagSet::Set(Flag f, bool value)
{
    data.set(static_cast<size_t>(f), value);
}

void FlagSet::Reset() { data.reset(); }
void FlagSet::Reset(Flag f) {   data.reset(static_cast<size_t>(f)); }

bool FlagSet::Any() const   { return data.any(); }
bool FlagSet::None() const  { return data.none(); }
bool FlagSet::All() const   { return data.all(); }

bool FlagSet::operator[](Flag f) const
{
    return data[static_cast<size_t>(f)];
}

//bool& dmgFlags::operator[](Flag f)
//{
//    return data[static_cast<size_t>(f)];
//}