#include "Observable.h"

// Exception:
//  - Precondition: 'f' must not be a nullptr.
int Observable::add_observer(callback f)
{
    Expects(f);
    observers_[id_] = f;
    return id_++;
}

// Exception:
//  - Precondition: 'id' must be a previously given identifier.
void Observable::remove_observer(int id)
{
    Expects(observers_.count(id) > 0);
    observers_.erase(id);
}

void Observable::notify() const
{
    for(const auto& p : observers_)
    {
        p.second();
    }
}
