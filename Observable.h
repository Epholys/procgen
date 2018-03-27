#ifndef OBSERVABLE_H
#define OBSERVABLE_H


#include <unordered_map>
#include <functional>

#include "gsl/gsl"

// Simple Observable class used for the classic GoF Observer pattern.
//
// Any class which inherit from 'Observable' can be observed by 'Observer<>' or
// simple functions.
// In case of observers classes, they must be added with 'add_observer()' and
// removed with 'remove_observer()'. These method are classically called in a
// RAII fashion. See the 'Observer<>' class for more information.
// In case of simple functions, 'remove_observer()' is not necessary.
// Every time the child class is modified, 'notify()' must be called to inform
// the observers of the change.
class Observable
{
public:
    // The type of function called after each 'notify()' call.
    using callback = std::function<void()>;

    Observable() = default;

    // Add a callback and return its unique identifier.
    // Exception:
    //  - Precondition: 'f' must not be a nullptr.
    int add_observer(callback f);

    // Remove a callback by its identifier.
    // Exception:
    //  - Precondition: 'id' must be a previously given identifier.
    void remove_observer(int id);

protected:
    // Notify all the observers. Must be called after each modification in the
    // child class.
    void notify() const;


    // A counter for the identifier of the next observer.
    int id_ { 0 };

    // The map of all callbacks.
    std::unordered_map<int, callback> observers_ { };
};


#endif // OBSERVABE_H
