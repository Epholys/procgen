#ifndef OBSERVER_H
#define OBSERVER_H


#include <memory>
#include <type_traits>
#include <utility>

#include "gsl/gsl"

#include "Observable.h"

// Simple Observer class used for classic GoF Observer pattern.
//
// Any class which inherit from 'Observer<>' can observe an 'Observable'
// class. This observable class is the first template parameter of the observer
// and is statically verified by the second template parameter using
// metaprogramming.
// The main focus of this class is to automatically manage the removal of the
// callback in the 'Observable' class using RAII.
template<typename T>
class Observer
{
public:
    static_assert(std::is_base_of<Observable, T>::value, "An observer must watch an Observable");
        

    // An Observer must have a observable target.
    // Exception:
    //  - Precondition: 't' must not be a nullptr.
    Observer() = delete;
    Observer(const std::shared_ptr<T>& t)
        : target_(t)
        , id_ { -1, false }
        {
            Expects(t);
        }

    // An Observer can not be copied trivially: no callback would be send to the
    // 'target_' but the id would be saved, making the destructor removing twice
    // for the same id. Each child class must define these function taking care
    // of calling 'add_callback()'.
    Observer(const Observer& other) = delete;
    Observer& operator=(const Observer& other) = delete;
    Observer(const Observer&& other) = delete;
    Observer& operator=(const Observer&& other) = delete;


    // If the callback exists, remove it from the target observable. Else, do
    // nothing. 
    virtual ~Observer()
        {
            if (id_.second)
            {
                target_->remove_observer(id_.first);
            }
        }

    // Add a callback to the target observable.
    // Usually, the callback is a method from the child class. The destructor
    // assures that the method of a destructed object is never used. However,
    // due to the construction of 'add_callback()', any suitable function could
    // be used of the callback, which is not recommended unless you know what
    // you are doing.
    void add_callback(const Observable::callback& callback)
        {
            if (id_.second)
            {
                target_->remove_observer(id_.first);
                id_.first = target_->add_observer(callback);
            }
            else
            {
                id_.first = target_->add_observer(callback);
                id_.second = true;
            }
        }
    
    void set_target(const std::shared_ptr<T>& t)
        {
            if (id_.second)
            {
                target_->remove_observer(id_.first);
                id_.first = -1;
                id_.second = false;
            }
            target_ = t;
        }

    const std::shared_ptr<T>& get_target() const
        {
            return target_;
        }
    
private:
    // The target observable.
    std::shared_ptr<T> target_;

    // The identifier of the registered callback.
    // If the callback is not defined, the boolean will be false.
    std::pair<int, bool> id_ { -1, false };
};


#endif // OBSERVER_H
