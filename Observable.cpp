#include "Observable.h"

int Observable::add_observer(const callback& f)
{
    observers_[id_] = f;
    return id_++;
}

void Observable::remove_observer(int id)
{
    Expects(observers_.count(id) > 0);
    observers_.erase(id);
}

void Observable::notify() const
{
    for(const auto& p : observers_)
    {
        if (p.second)
        {
            p.second();
        }
    }
}
