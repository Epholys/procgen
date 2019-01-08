#include <gsl/gsl>
#include <algorithm>
#include "UniqueId.h"

// sf::Id UniqueId::register_id(int id)
// {
//     // Assert precondition : 'id' must be positive and not already registered.
//     Expects(id >= 0);
//     auto alread_exists = std::find_if(begin(map_), end(map_),
//                                       [id](const auto& p){return p.second == id;});
//     Expects(alread_exists == map_.end());

//     // Find if there is an available id.
//     auto empty_slot = std::find_if(begin(map_), end(map_),
//                                    [](const auto& p){return p.second == -1;});
//     if (empty_slot != map_.end())
//     {
//         // If so, associate 'id' to it.
//         empty_slot->second = id;
//         return empty_slot->first;
//     }
//     else
//     {
//         // Otherwise, generate a new id and create the link.
//         auto id = new_id();
//         map_.push_back({id, id});
//         return id;
//     }
// }

int UniqueId::get_id()
{
    auto empty_slot = std::find_if(begin(map_), end(map_),
                                   [](const auto& p)
                                   {return std::get<IsEmpty>(p);});
    if (empty_slot != end(map_))
    {
        std::get<IsEmpty>(*empty_slot) = false;
        return std::get<Id>(*empty_slot);
    }
    else
    {
        map_.push_back({current_id_, false});
        return current_id_++;
    }
}
    
void UniqueId::remove_id(int id)
{
    // Assert precondition: 'id' must have been registered.
    auto it = std::find_if(begin(map_), end(map_),
                           [id](const auto& p)
                           {return std::get<Id>(p) == id &&
                            !std::get<IsEmpty>(p);});
    Expects(it != end(map_));

    // Mark the id as available.
    std::get<IsEmpty>(*it) = true;;
}

