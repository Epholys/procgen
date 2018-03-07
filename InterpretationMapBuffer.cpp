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

    
    InterpretationMapBuffer::InterpretationMapBuffer(drawing::InterpretationMap& map)
        : map_ {map}
        , interpretation_buffer_ {}
    {
        // Initialize the buffer with the InterpretationMap's interpretations.
        for (const auto& interpretation : map.get_rules())
        {
            predecessor pred  = { interpretation.first, '\0' };
            interpretation_buffer_.push_back({true, pred, get_order_entry(interpretation.second)});
        }
    }

    void InterpretationMapBuffer::sync()
    {
        map_.clear_rules();
        
        for (const auto& interpretation : interpretation_buffer_)
        {
            char pred = std::get<predecessor>(interpretation).at(0);
            if(std::get<validity>(interpretation) &&
               pred != '\0') // An empty interpretation is not synchronized.
            {
                map_.get_rule(pred).second = std::get<OrderEntry>(interpretation).order;
            }
        }
    }
}
