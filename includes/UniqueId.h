#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H


#include <tuple>
#include <vector>

// Generate unique identifiers and recycle them.
// 
// This class is usually used as a static attribute. Each time an object needs a
// unique id, it can call 'get_id()' to get a unique identifier. If an object
// is destructed, it can call 'remove_id()' in its destructor to free its
// identifier for a future object.
//
// Invariant:
//   - Each new identifier is unique.
//   - No new identifier are generated as long as freed old ones exists.
class UniqueId
{
public:
    // Get a unique identifier. It can be a completely new identifier or a old
    // one that was marked as free.
    int get_id();
        
    // Free 'id': a call to 'get_id()' could returns this new id.
    //
    // Exceptions;
    //   - Precondition: 'id' must exists in 'map_'
    void free_id(int id);

private:
    struct Entry
    {
        int id {-1};
        bool is_free {true};
    };
    
    // All the existing identifiers. The first element of the tuple is the id,
    // and the second element is a boolean indicating if this id is free.
    std::vector<Entry> map_ {};

    // The next new identifier.
    int next_id_ {0};
};

#endif // UNIQUE_ID_HPP
