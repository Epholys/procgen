#include "VertexPainterComposite.h"

#include "VertexPainterLinear.h"
#include "VertexPainterSerializer.h"

#include <cmath>

namespace colors
{
namespace impl
{
    ColorGeneratorComposite::ColorGeneratorComposite()
        : painter_ {nullptr}
        , global_index_ {0}
    {
    }

    ColorGeneratorComposite::ColorGeneratorComposite(VertexPainterComposite* painter)
        : painter_ {painter}
        , global_index_ {0}
    {
        Expects(painter_);
    }

    sf::Color ColorGeneratorComposite::get(float f)
    {
        // OPTIMIZATION TODO
        // // Should never happen.
        Expects(painter_);
        Expects(!painter_->vertex_indices_pools_.empty());
        // END

        // We do not clamp to 1 as it would be an out-of-bound call. So we clamp
        // it to just before 1.
        f = std::clamp(f, 0.f, 1.f - std::numeric_limits<float>::epsilon());
        auto size = painter_->child_painters_.size();
        // Compute which child painter is concerned by this vertex.
        unsigned which_painter = static_cast<unsigned>(f * size);

        // OPTIMIZATION
        // painter_->vertex_indices_pools_.at(which_painter).push_back(global_index_++);
        painter_->vertex_indices_pools_[which_painter].push_back(global_index_++);
        // END

        // Dummy color (BUT NOT TRANSPARENT (Transparent is a special value
        // for save/load position))
        return sf::Color::White;
    }

    void ColorGeneratorComposite::reset_index()
    {
        global_index_ = 0;
    }

    std::shared_ptr<ColorGenerator> ColorGeneratorComposite::clone() const
    {
        return std::make_shared<ColorGeneratorComposite>();
    }

    std::string ColorGeneratorComposite::type_name() const
    {
        return "ColorGeneratorComposite";
    }

} // namespace impl

std::shared_ptr<VertexPainter> VertexPainterComposite::copied_painter_ {};

VertexPainterComposite::VertexPainterComposite()
    : VertexPainter {}
    , color_distributor_ {std::make_shared<impl::ColorGeneratorComposite>(this)}
    , main_painter_ {std::make_shared<VertexPainterLinear>(
          ColorGeneratorWrapper(color_distributor_))}
    , vertex_indices_pools_ {}
    , child_painters_ {}
{
    child_painters_.emplace_back(VertexPainterWrapper());
}

VertexPainterComposite::VertexPainterComposite(const ColorGeneratorWrapper&)
    : VertexPainterComposite {}
{
}

std::shared_ptr<VertexPainter> VertexPainterComposite::clone() const
{
    auto clone = std::make_shared<VertexPainterComposite>();

    auto main_painter_clone = main_painter_;
    clone->set_main_painter(main_painter_clone);


    clone->child_painters_.clear();
    for (const auto& child : child_painters_)
    {
        auto child_clone = child;
        clone->child_painters_.emplace_back(std::move(child_clone));
    }

    clone->vertex_indices_pools_ = vertex_indices_pools_;

    clone->poll_modification();

    return clone;
}

const std::vector<VertexPainterWrapper>& VertexPainterComposite::get_child_painters() const
{
    return child_painters_;
}
std::vector<VertexPainterWrapper>& VertexPainterComposite::ref_child_painters()
{
    return child_painters_;
}

const VertexPainterWrapper& VertexPainterComposite::get_main_painter() const
{
    return main_painter_;
}
VertexPainterWrapper& VertexPainterComposite::ref_main_painter()
{
    return main_painter_;
}

void VertexPainterComposite::set_main_painter(const VertexPainterWrapper& painter_wrapper)
{
    // Hack, as 'color_distributor_' will be called by the 'notify()' of the
    // 'wrap()' method of 'painter_wrapper'.
    // Moving this in 'color_distributor_.get()' does not work.
    vertex_indices_pools_.clear();
    for (auto i = 0u; i < child_painters_.size(); ++i)
    {
        vertex_indices_pools_.push_back({});
    }

    auto painter = painter_wrapper.unwrap();
    painter->ref_generator_wrapper().wrap(color_distributor_);
    main_painter_.wrap(painter);
    indicate_modification();
}


void VertexPainterComposite::set_child_painters(const std::vector<VertexPainterWrapper>& painters)
{
    child_painters_.clear();
    for (const auto& painter : painters)
    {
        child_painters_.emplace_back(painter);
    }
    indicate_modification();
}

void VertexPainterComposite::paint_vertices(std::vector<sf::Vertex>& vertices,
                                            const std::vector<u8>& iteration_of_vertices,
                                            const std::vector<bool>& transparent,
                                            int max_recursion,
                                            sf::FloatRect bounding_box)

{
    // Prepare the variable for ColorGeneratorComposite
    const auto n_vertices = vertices.size();
    const auto n_child = child_painters_.size();
    const auto approx_size = n_vertices / n_child;
    color_distributor_->reset_index();
    vertex_indices_pools_.clear();
    for (auto i = 0u; i < n_child; ++i)
    {
        vertex_indices_pools_.push_back({});
        vertex_indices_pools_.back().reserve(approx_size);
    }

    // Fill the pools by making paint the main painter through 'color_distributor_'.
    main_painter_.unwrap()->paint_vertices(vertices,
                                           iteration_of_vertices,
                                           transparent,
                                           max_recursion,
                                           bounding_box);

#ifdef DEBUG_CHECKS
    for (auto i = 0ull; i < child_painters_.size(); ++i)
    {
        // For each indices pools...
        std::vector<sf::Vertex> vertices_part;
        std::vector<u8> iteration_of_vertices_part;
        std::vector<bool> transparent_part;
        const auto pool_size = vertex_indices_pools_.at(i).size();
        vertices_part.reserve(pool_size);
        iteration_of_vertices_part.reserve(pool_size);
        transparent_part.reserve(pool_size);
        for (auto idx : vertex_indices_pools_.at(i))
        {
            // ... get each index and get from the '*_copy' the vertex and
            // its iteration.
            vertices_part.push_back(vertices.at(idx));
            iteration_of_vertices_part.push_back(iteration_of_vertices.at(idx));
            transparent_part.push_back(transparent.at(idx));
        }
        child_painters_.at(i).unwrap()->paint_vertices(vertices_part,
                                                       iteration_of_vertices_part,
                                                       transparent_part,
                                                       max_recursion,
                                                       bounding_box);
        for (auto j = 0ull; j < vertices_part.size(); ++j)
        {
            vertices.at(vertex_indices_pools_.at(i).at(j)) = vertices_part.at(j);
        }
    }
#else
    for (auto i = 0ull; i < child_painters_.size(); ++i)
    {
        // For each indices pools...
        std::vector<sf::Vertex> vertices_part;
        std::vector<u8> iteration_of_vertices_part;
        std::vector<bool> transparent_part;
        const auto pool_size = vertex_indices_pools_[i].size();
        vertices_part.reserve(pool_size);
        iteration_of_vertices_part.reserve(pool_size);
        transparent_part.reserve(pool_size);
        for (auto idx : vertex_indices_pools_[i])
        {
            // ... get each index and get from the '*_copy' the vertex and
            // its iteration.
            vertices_part.push_back(vertices[idx]);
            iteration_of_vertices_part.push_back(iteration_of_vertices[idx]);
            transparent_part.push_back(transparent[idx]);
        }
        child_painters_[i].unwrap()->paint_vertices(vertices_part,
                                                    iteration_of_vertices_part,
                                                    transparent_part,
                                                    max_recursion,
                                                    bounding_box);
        for (auto j = 0ull; j < vertices_part.size(); ++j)
        {
            vertices[vertex_indices_pools_[i][j]] = vertices_part[j];
        }
    }
#endif
}

void VertexPainterComposite::supplementary_drawing(sf::FloatRect bounding_box) const
{
    main_painter_.unwrap()->supplementary_drawing(bounding_box);
    for (const auto& painter : child_painters_)
    {
        painter.unwrap()->supplementary_drawing(bounding_box);
    }
}


bool VertexPainterComposite::has_copied_painter()
{
    return bool(copied_painter_);
}
std::shared_ptr<VertexPainter> VertexPainterComposite::get_copied_painter()
{
    Expects(has_copied_painter());
    return copied_painter_->clone();
}
void VertexPainterComposite::save_painter(const std::shared_ptr<VertexPainter>& painter)
{
    copied_painter_ = painter->clone();
}

std::string VertexPainterComposite::type_name() const
{
    return "VertexPainterComposite";
}

bool VertexPainterComposite::poll_modification()
{
    bool modified = VertexPainter::poll_modification();
    modified |= main_painter_.poll_modification();
    for (auto& painter : child_painters_)
    {
        modified |= painter.poll_modification();
    }
    return modified;
}
} // namespace colors
