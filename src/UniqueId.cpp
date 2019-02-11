#include <gsl/gsl>
#include <algorithm>
#include "UniqueId.h"

int UniqueId::get_id()
{
    //Search if there is a free slot in 'map_'.
    auto free_slot = std::find_if(begin(map_), end(map_),
                                   [](const auto& p)
                                   {return std::get<IsFree>(p);});
    if (free_slot != end(map_))
    {
        // There is, return the associated identifier
        std::get<IsFree>(*free_slot) = false;
        return std::get<Id>(*free_slot);
    }
    else
    {
        // There is not, add a new id to 'map_' and return it.
        map_.push_back({next_id_, false});
        return next_id_++;
    }
}
    
void UniqueId::free_id(int id)
{
    // Assert precondition: 'id' must have been registered.
    auto it = std::find_if(begin(map_), end(map_),
                           [id](const auto& p)
                           {return std::get<Id>(p) == id &&
                            !std::get<IsFree>(p);});
    Expects(it != end(map_));

    // Mark the id as available.
    std::get<IsFree>(*it) = true;;
}

