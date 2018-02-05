#ifndef OBSERVER_H
#define OBSERVER_H


#include <memory>
#include <type_traits>

#include "Observable.h"

template<typename T,
         typename = std::enable_if_t<std::is_base_of<Observable, T>::value>>
class Observer
{
public:
    Observer(const std::shared_ptr<T>& t)
        {
            target_ = t;
            id_ = target_->add_observer([this](){this->handle_notification();});
        }
    
    virtual ~Observer()
        {
            target_->remove_observer(id_);
        }

    virtual void handle_notification() const = 0;

protected:
    std::shared_ptr<T> target_ { nullptr };
    int id_ { 0 };
};


#endif // OBSERVER_H
