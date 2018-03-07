#include "InterpretationMapBuffer.h"

namespace procgui
{
    OrderEntry get_order_entry(const drawing::Order& order)
    {
        const auto order_entry = std::find_if(all_orders.begin(), all_orders.end(),
                                              [order](const auto& o){ return order.id == o.order.id; });
        Ensures(order_entry != all_orders.end());
        return *order_entry;
    }
}
