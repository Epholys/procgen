#ifndef OBSERVABLE_H
#define OBSERVABLE_H


#include <unordered_map>
#include <functional>

#include "gsl/gsl"

class Observable
{
public:
    using callback = std::function<void()>;
    
    Observable() = default;

    int add_observer(const callback& f);
    void remove_observer(int id);

protected:
    void notify() const;

private:
    int id_ { 0 };
    std::unordered_map<int, callback> observers_ { };
};


#endif // OBSERVABE_H
