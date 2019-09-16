#include "SupplementaryRendering.h"

namespace procgui
{
    std::vector<SupplementaryRendering::DrawCall> SupplementaryRendering::draw_calls_ {};

    void SupplementaryRendering::add_draw_call(const DrawCall& call)
    {
        draw_calls_.emplace_back(call);
    }

    void SupplementaryRendering::clear_draw_calls()
    {
        draw_calls_.clear();
    }

    void SupplementaryRendering::draw(sf::RenderTarget& target)
    {
        for (const auto& call : draw_calls_)
        {
            target.draw(call.vertices.data(), call.vertices.size(), call.type, call.states);
        }
    }
}
