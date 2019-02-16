#ifndef INTERPRETATION_MAP_BUFFER_H
#define INTERPRETATION_MAP_BUFFER_H


#include "gsl/gsl"

#include "Turtle.h"
#include "RuleMapBuffer.h"

namespace procgui
{
    // Simple RuleMapBuffer
    using InterpretationMapBuffer = RuleMapBuffer<drawing::InterpretationMap>;
}

#endif // INTERPRETATION_MAP_BUFFER_H
