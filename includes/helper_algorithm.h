#ifndef HELPER_ALGORITHM
#define HELPER_ALGORITHM


// TODO: when 'RuleMapBuffer' will be removed, removes these too.

// Find a duplicate of 'model' between [first, last)
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

// Find a duplicate of 'model' satisfying condition 'p' between [first, last)
template<typename ForwardIt, typename Predicate>
ForwardIt find_duplicate_if(ForwardIt model, ForwardIt first, ForwardIt last, Predicate p)
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
