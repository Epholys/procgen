#ifndef LSYSTEM_BUFFER_H
#define LSYSTEM_BUFFER_H


#include "RuleBuffer.h"

namespace procgui
{
    // ImGui needs a number for its fixed-size input buffer.
    // We arbitrarily choose 64 characters as the limit for the sucessor of a
    // production rule.
    constexpr int lsys_successor_size = 64;

    using LSystemBuffer = RuleBuffer<LSystem>;
}


#endif // LSYSTEM_BUFFER_H
