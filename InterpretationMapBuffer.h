#ifndef INTERPRETATION_MAP_VIEW_H
#define INTERPRETATION_MAP_VIEW_H


#include <list>
#include <tuple>
#include <array>

#include "gsl/gsl"

#include "Turtle.h"

namespace procgui
{
    // An OrderEntry is the simple association of an 'drawing::Order' to a
    // string identifier. This string is used in the GUI to select an order.
    struct OrderEntry
    {
        drawing::Order order;
        std::string name;
    };
    inline bool operator== (const OrderEntry& lhs, const OrderEntry& rhs)
    {
        return lhs.order == rhs.order;
    }

    // All the orders:
    const OrderEntry go_forward_entry { drawing::go_forward, "Go forward" };
    const OrderEntry turn_right_entry { drawing::turn_right, "Turn right" };
    const OrderEntry turn_left_entry  { drawing::turn_left, "Turn left"   };
    const OrderEntry save_position_entry { drawing::save_position, "Save position" };
    const OrderEntry load_position_entry { drawing::load_position, "Load position" };

    const std::vector<OrderEntry> all_orders { go_forward_entry, turn_right_entry, turn_left_entry,
                                               save_position_entry, load_position_entry };
    const std::vector<const char*> all_orders_name =
        [](){ std::vector<const char*> v;
              for(const auto& o : all_orders)
                  v.push_back(o.name.c_str());
              return v; }();

    // Find the OrderEntry associated to an Order
    // Exceptions:
    //   - Postcondition: an 'OrderEntry' associated to a 'drawing::Order' must
    //   exists.
    OrderEntry get_order_entry(const drawing::Order& order);


    
    // The interface between drawing::InterpretationMap and the ImGui GUI.
    //
    // ImGui is a immediate-mode GUI, so it can not retain information between
    // frame. This class hosts a interpretation (predecessor linked to an order)
    // buffer used in 'procgui::interact_with()' to manage:
    //   - duplicate interpretations
    //   - adding new interpretations
    //   - removing interpretations
    // We can still directly access the InterpretationMap for all trivial
    // attributes like the axiom.
    //
    // An InterpretationMapBuffer must be destructed before the destruction of
    // its InterpretationMap (non owning-reference). For the coherence of the
    // GUI, an InterpretationMap must have a unique InterpretationMapBuffer
    // associated.
    //
    // Note: This class has a lot in common with 'LSystemBuffer'. If a third
    // class has the same properties, all will be refactorized.
    struct InterpretationMapBuffer
    {
        using validity = bool; // If the interpretation is a duplicate, it is
                               // not valid.
        using predecessor = std::array<char, 2>;

        InterpretationMapBuffer(drawing::InterpretationMap& map);

        // Synchronize the interpretations buffer with the IntepretationMap
        void sync();

        drawing::InterpretationMap& map_; // non-owning reference
        std::list<std::tuple<validity, predecessor, OrderEntry>> interpretation_buffer_;
    };
}

#endif // INTERPRETATION_MAP_VIEW_H
