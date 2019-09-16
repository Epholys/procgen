#include <cmath>
#include "VertexPainterComposite.h"
#include "VertexPainterLinear.h"
#include "VertexPainterSerializer.h"

namespace colors
{
    namespace impl
    {
        ColorGeneratorComposite::ColorGeneratorComposite()
            : painter_{nullptr}
            , global_index_{0}
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
            // Should never happen.
            Expects(painter_);
            Expects(!painter_->vertex_indices_pools_.empty());

            f = f < 0. ? 0. : f;
            // We do not clamp to 1 as it would be an out-of-bound call. So we clamp
            // it to just before 1.
            f = f >= 1. ? 1.-std::numeric_limits<float>::epsilon() : f;
            
            auto size = painter_->child_painters_observers_.size();
            // Compute which child painter is concerned by this vertex.
            unsigned which_painter = std::floor(f * size);

            // Get the correct pool
            auto it = painter_->vertex_indices_pools_.begin();
            for (auto i=0u; i<which_painter; ++i)
            {
                ++it;
            }

            // Add the index of the current vertex in the correct pool.
            it->push_back(global_index_++);

            // Dummy color (BUT NOT TRANSPARENT (Transparent is a special value
            // for save/load position))
            return sf::Color(0,0,0,1);
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
        

        //------------------------------------------------------------

        VertexPainterWrapperObserver::VertexPainterWrapperObserver(std::shared_ptr<VertexPainterWrapper> painter_wrapper,
                                                                   VertexPainterComposite* painter_composite)
            : OWrapper {painter_wrapper}
            , painter_ {painter_composite}
        {
            Expects(painter_);
            add_callback([this](){painter_->notify();});
        }

        std::shared_ptr<VertexPainterWrapper> VertexPainterWrapperObserver::get_painter_wrapper() const
        {
            return get_target();
        }
        void VertexPainterWrapperObserver::set_painter_wrapper(std::shared_ptr<VertexPainterWrapper> painter_wrapper)
        {
            Expects(painter_);
            set_target(painter_wrapper);
            add_callback([this](){painter_->notify();});
            painter_->notify();
        }

        void VertexPainterWrapperObserver::set_composite_painter(VertexPainterComposite* painter)
        {
            Expects(painter_);
            painter_ = painter;
            add_callback([this](){painter_->notify();});
            painter_->notify();
        }
    }

    //----------------------------------------------------------------------
    
    std::shared_ptr<VertexPainter> VertexPainterComposite::copied_painter_ {};

    VertexPainterComposite::VertexPainterComposite()
        : VertexPainter{} 
        , color_distributor_{std::make_shared<impl::ColorGeneratorComposite>(this)}
        , main_painter_observer_{
            std::make_shared<VertexPainterWrapper>(
                std::make_shared<VertexPainterLinear>(
                    std::make_shared<ColorGeneratorWrapper>(color_distributor_))),
                this}
        , vertex_indices_pools_{}
        , child_painters_observers_{}
    {
        child_painters_observers_.emplace_back(std::make_shared<VertexPainterWrapper>(), this);
    }

    VertexPainterComposite::VertexPainterComposite(const std::shared_ptr<ColorGeneratorWrapper>)
        : VertexPainterComposite{}
    {
    }
    
    std::shared_ptr<VertexPainter> VertexPainterComposite::clone() const
    {
        auto clone = std::make_shared<VertexPainterComposite>();

        auto composite_color_wrapper = std::make_shared<ColorGeneratorWrapper>(clone->color_distributor_);
        auto main_painter_clone = std::make_shared<VertexPainterWrapper>(*main_painter_observer_.get_painter_wrapper());
        main_painter_clone->unwrap()->set_generator_wrapper(composite_color_wrapper);
        clone->main_painter_observer_.set_painter_wrapper(main_painter_clone);


        clone->child_painters_observers_.clear();
        for (auto it = begin(child_painters_observers_); it != end(child_painters_observers_); ++it)
        {
            auto child_clone = std::make_shared<VertexPainterWrapper>(*it->get_painter_wrapper());
            clone->child_painters_observers_.emplace_back(child_clone, &(*clone));
        }

        clone->vertex_indices_pools_ = vertex_indices_pools_;
        
        return clone;
    }
    
    std::list<std::shared_ptr<VertexPainterWrapper>> VertexPainterComposite::get_child_painters() const
    {
        std::list<std::shared_ptr<VertexPainterWrapper>> list;
        for (const auto& observer : child_painters_observers_)
        {
            list.push_back(observer.get_target());
        }
        return list;
    }

    std::shared_ptr<VertexPainterWrapper> VertexPainterComposite::get_main_painter() const
    {
        return main_painter_observer_.get_painter_wrapper();
    }

    void VertexPainterComposite::set_main_painter(std::shared_ptr<VertexPainterWrapper> painter_wrapper)
    {
        // Hack, as 'color_distributor_' will be called by the 'notify()' of the
        // 'wrap()' method of 'painter_wrapper'.
        // Moving this in 'color_distributor_.get()' does not work.
        vertex_indices_pools_.clear();
        for(auto i=0u; i<child_painters_observers_.size(); ++i)
        {
            vertex_indices_pools_.push_back({});
        } 

        painter_wrapper->unwrap()->get_generator_wrapper()->wrap(color_distributor_);
        main_painter_observer_.set_painter_wrapper(painter_wrapper);
        notify();
    }

                 
    void VertexPainterComposite::set_child_painters(const std::list<std::shared_ptr<VertexPainterWrapper>> painters)
    {
        child_painters_observers_.clear();
        for (const auto& painter : painters)
        {
            child_painters_observers_.emplace_back(painter, this);
        }
        notify();
    }

    void VertexPainterComposite::paint_vertices(std::vector<sf::Vertex>& vertices,
                                                const std::vector<int>& iteration_of_vertices,
                                                int max_recursion,
                                                sf::FloatRect bounding_box)

    {
        auto vertices_copy = vertices;
        auto iteration_of_vertices_copy = iteration_of_vertices;
        
        // Prepare the variable for ColorGeneratorComposite
        color_distributor_->reset_index();
        vertex_indices_pools_.clear();
        for(auto i=0u; i<child_painters_observers_.size(); ++i)
        {
            vertex_indices_pools_.push_back({});
        } 

        // Fill the pools by making paint the main painter through 'color_distributor_'.
        main_painter_observer_.get_painter_wrapper()->unwrap()->paint_vertices(vertices_copy,
                                                                               iteration_of_vertices,
                                                                               max_recursion,
                                                                               bounding_box);

        // -- Get the vertices and their iteration from the indices in
        // 'vertex_indices_pools_'. 
        std::vector<std::vector<sf::Vertex>> vertices_pools;
        std::vector<std::vector<int>> iteration_of_vertices_pools;

        for(const auto& v : vertex_indices_pools_)
        {
            // For each indices pools...
            std::vector<sf::Vertex> vertices_part;
            std::vector<int> iteration_of_vertices_part;
            for(auto idx : v)
            {
                // ... get each index and get from the '*_copy' the vertex and
                // its iteration.
                vertices_part.push_back(vertices_copy.at(idx));
                iteration_of_vertices_part.push_back(iteration_of_vertices_copy.at(idx));
            }
            vertices_pools.push_back(vertices_part);
            iteration_of_vertices_pools.push_back(iteration_of_vertices_part);
        }

        auto idx = 0u;
        for(auto& painter_it : child_painters_observers_)
        {
            // Call each child painter with its part of vertices with their index.
            painter_it.get_painter_wrapper()->unwrap()->paint_vertices(vertices_pools.at(idx),
                                                                       iteration_of_vertices_pools.at(idx),
                                                                       max_recursion,
                                                                       bounding_box);
            ++idx;
        }

        // Flatten 'vertices_pools' into a single vertex std::vector.
        std::vector<sf::Vertex> flattened_pool;
        std::size_t total_size = 0;
        for (const auto& sub : vertices_pools)
        {
            total_size += sub.size();
        }
        flattened_pool.reserve(total_size);
        for (const auto& sub : vertices_pools)
        {
            flattened_pool.insert(flattened_pool.end(), sub.begin(), sub.end());
        }

        // By construction, the n-th vertex in 'flatten_pool' correspond to the
        // n-th index in the flattened 'vertex_indices_pools_'. This index is
        // the position in the global 'vertices' vector. So, this code put in
        // order the painted vertices in 'vertices'.
        auto i = 0u;
        for (const auto& v : vertex_indices_pools_)
        {
            for (auto idx : v)
            {
                vertices_copy.at(idx) = flattened_pool.at(i++);
            }
        }

        vertices = vertices_copy;
    }

    void VertexPainterComposite::supplementary_drawing(sf::FloatRect bounding_box) const
    {
        main_painter_observer_.get_target()->unwrap()->supplementary_drawing(bounding_box);
        for (const auto& painter : child_painters_observers_)
        {
            painter.get_target()->unwrap()->supplementary_drawing(bounding_box);
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
    void VertexPainterComposite::save_painter(std::shared_ptr<VertexPainter> painter)
    {
        copied_painter_ = painter->clone();
    }

    std::string VertexPainterComposite::type_name() const
    {
        return "VertexPainterComposite";
    }
}
