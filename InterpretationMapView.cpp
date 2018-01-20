#include "InterpretationMapView.h"

// TODO Change comments
namespace procgui
{
    InterpretationMapView::InterpretationMapView(drawing::interpretation_map& map)
        : map_ {map}
        , interpretation_buffer_ {}
    {
        // Initialize the buffer with the InterpretationMap's rules.
        for (const auto& instruction : map)
        {
            predecessor pred  = { instruction.first, '\0' };
            interpretation_buffer_.push_back({true, pred, instruction.second});
        }
    }

    void InterpretationMapView::sync()
    {
        map_.clear();
        
        for (const auto& interpretation : interpretation_buffer_)
        {
            char pred = std::get<predecessor>(interpretation).at(0);
            if(std::get<validity>(interpretation) &&
               pred != '\0') // An empty rule is not synchronized.
            {
                map_[pred] = std::get<drawing::Order>(interpretation);
            }
        }
    }
}
