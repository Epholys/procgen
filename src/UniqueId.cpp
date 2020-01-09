#include "UniqueId.h"

#include <algorithm>
#include <gsl/gsl>

int UniqueId::get_id()
{
    // Search if there is a free slot in 'map_'.
    auto free_slot = std::find_if(begin(map_), end(map_), [](const auto& e) { return e.is_free; });
    if (free_slot != end(map_))
    {
        // There is, return the associated identifier
        free_slot->is_free = false;
        return free_slot->id;
    }


    // There is not, add a new id to 'map_' and return it.
    map_.push_back({next_id_, false});
    return next_id_++;
}

void UniqueId::free_id(int id)
{
    // Assert precondition: 'id' must have been registered.
    auto it = std::find_if(begin(map_), end(map_), [id](const auto& e) {
        return e.id == id && !e.is_free;
    });
    Expects(it != end(map_));

    // Mark the id as available.
    it->is_free = true;
    ;
}
