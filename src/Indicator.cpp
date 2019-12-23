#include "Indicator.h"

bool Indicator::poll_modification()
{
    if (is_modified_)
    {
        is_modified_ = false;
        return true;
    }
    return false;
}

void Indicator::indicate_modification()
{
    is_modified_ = true;
}
