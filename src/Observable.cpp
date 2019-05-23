#include "Observable.h"

Observable::Observable(const Observable&)
{
    id_ = {0};
    observers_ = {};
}
Observable& Observable::operator=(const Observable& other)
{
    if (this != &other)
    {
        id_ = {0};
        observers_ = {};
    }
    return *this;
}

Observable::Observable(Observable&& other)
    : Observable{}
{
    id_ = {0};
    observers_ = {};
    other.id_ = {0};
    other.observers_ = {};
}

Observable& Observable::operator=(Observable&& other)
{
    if (this != &other)
    {
        id_ = {0};
        observers_ = {};
        other.id_ = {0};
        other.observers_ = {};
    }
    return *this;
}
    

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
