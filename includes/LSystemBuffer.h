#ifndef LSYSTEM_BUFFER_H
#define LSYSTEM_BUFFER_H


#include "LSystem.h"
#include "RuleMapBuffer.h"


namespace procgui
{
    // ImGui needs a number for its fixed-size input buffer.
    // We arbitrarily choose 128 characters as the limit for the sucessor of a
    // production rule.
    constexpr int lsys_successor_size = 128;

    // Simple implementation of the LSystemBuffer.
    using LSystemBuffer = RuleMapBuffer<LSystem>;
}


#endif // LSYSTEM_BUFFER_H
