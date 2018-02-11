#ifndef HELPER_ALGORITHM
#define HELPER_ALGORITHM


// TODO
#include <iostream>

template<typename ForwardIt, typename Type>
ForwardIt find_existing(ForwardIt first, ForwardIt last, const Type& value)
{
    for (auto it = first; it != last; ++it)
    {
        if (*it == value)
        {
            return it;
        }
    }
    return last;
}

template<typename ForwardIt, typename Type, typename Predicate>
ForwardIt find_existing(ForwardIt first, ForwardIt last, const Type& value, Predicate p)
{
    for (auto it = first; it != last; ++it)
    {
        if (p(*it, value))
        {
            return it;
        }
    }
    return last;
}

template<typename ForwardIt>
ForwardIt find_duplicate(ForwardIt model, ForwardIt first, ForwardIt last)
{
    for (auto it = first; it != last; ++it)
    {
        if (it != model && *it == *model)
        {
            return it;
            break;
        }
    }
    return last;
}

template<typename ForwardIt, typename Predicate>
ForwardIt find_duplicate(ForwardIt model, ForwardIt first, ForwardIt last, Predicate p)
{
    for (auto it = first; it != last; ++it)
    {
        if (it != model && p(*model, *it))
        {
            return it;
            break;
        }
    }
    return last;
}


#endif // HELPER_ALGORITHM
