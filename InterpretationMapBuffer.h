#ifndef INTERPRETATION_MAP_BUFFER_H
#define INTERPRETATION_MAP_BUFFER_H


#include <list>
#include <tuple>
#include <array>

#include "gsl/gsl"

#include "Turtle.h"
#include "RuleMapBuffer.h"

namespace procgui
{
    // // An OrderEntry is the simple association of an 'drawing::Order' to a
    // // string identifier. This string is used in the GUI to select an order.
    // // OrderEntry is necessary beyond 'drawing::Order' to interact in the GUI.
    // struct OrderEntry
    // {
    //     drawing::Order order;
    //     std::string name;
    // };
    // inline bool operator== (const OrderEntry& lhs, const OrderEntry& rhs)
    // {
    //     return lhs.order == rhs.order;
    // }

    // // All the orders:
    // const OrderEntry go_forward_entry { drawing::go_forward, "Go forward" };
    // const OrderEntry turn_right_entry { drawing::turn_right, "Turn right" };
    // const OrderEntry turn_left_entry  { drawing::turn_left, "Turn left"   };
    // const OrderEntry save_position_entry { drawing::save_position, "Save position" };
    // const OrderEntry load_position_entry { drawing::load_position, "Load position" };

    // const std::vector<OrderEntry> all_orders { go_forward_entry, turn_right_entry, turn_left_entry,
    //                                            save_position_entry, load_position_entry };
    // const std::vector<const char*> all_orders_name =
    //     [](){ std::vector<const char*> v;
    //           for(const auto& o : all_orders)
    //               v.push_back(o.name.c_str());
    //           return v; }();

    // // Find the OrderEntry associated to an Order
    // // Exceptions:
    // //   - Postcondition: an 'OrderEntry' associated to a 'drawing::Order' must
    // //   exists.
    // OrderEntry get_order_entry(const drawing::Order& order);

    // Simple RuleMapBuffer
    using InterpretationMapBuffer = RuleMapBuffer<drawing::InterpretationMap>;
}

#endif // INTERPRETATION_MAP_BUFFER_H
