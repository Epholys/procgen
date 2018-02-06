#ifndef OBSERVER_H
#define OBSERVER_H


#include <memory>
#include <type_traits>
#include <utility>

#include "Observable.h"

template<typename T,
         typename = std::enable_if_t<std::is_base_of<Observable, T>::value>>
class Observer
{
public:
    Observer(const std::shared_ptr<T>& t)
        : target_(t)
        , id_ { -1, false }
        {
        }
    
    virtual ~Observer()
        {
            if (id_.second)
            {
                target_->remove_observer(id_.first);
            }
        }

    void add_callback(const Observable::callback& callback)
        {
            id_.first = target_->add_observer(callback);
            id_.second = true;
        }

protected:
    std::shared_ptr<T> target_ { nullptr };
    std::pair<int, bool> id_ { -1, false };
};


#endif // OBSERVER_H
