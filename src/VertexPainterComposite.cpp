#include "helper_math.h"
#include "ColorsGenerator.h"
#include "VertexPainterComposite.h"
#include "VertexPainterRadial.h"

namespace colors
{
    namespace impl
    {
        ColorGeneratorComposite::ColorGeneratorComposite(VertexPainterComposite& painter)
            : painter_ {painter}
            , index_ {0}
        {
        }

        sf::Color ColorGeneratorComposite::get(float f)
        {
            f = f < 0. ? 0. : f;
            // We do not clamp to 1 as it would be a out-of-bound call. So we clamp
            // it to just before 1.
            f = f >= 1. ? 1.-std::numeric_limits<float>::epsilon() : f;

            auto size = painter_.child_painters_.size();
            unsigned index = std::floor(f * size);
            Expects(index < size);
            Expects(!painter_.vertices_index_groups_.empty());
            auto it = painter_.vertices_index_groups_.begin();
            for (auto i=0u; i<index; ++i)
            {
                ++it;
            }
            it->push_back(index_++); // ULGY index_ vs index
            
            return sf::Color::Transparent;
        }

        void ColorGeneratorComposite::reset_index()
        {
            index_ = 0;
        }        
        
        std::shared_ptr<ColorGenerator> ColorGeneratorComposite::clone_impl() const
        {
            return painter_.color_distributor_;
        }

        //------------------------------------------------------------

        VertexPainterBufferObserver::VertexPainterBufferObserver(std::shared_ptr<VertexPainterBuffer> painter_buffer,
                                                                 VertexPainterComposite& painter_composite)
            : OBuffer {painter_buffer}
            , painter_ {painter_composite}
        {
            add_callback([this](){painter_.notify();});
        }
        // Shallow rule-of-five constructors.
        VertexPainterBufferObserver::VertexPainterBufferObserver(const VertexPainterBufferObserver& other)
            : OBuffer{other.get_target()}
            , painter_{other.painter_}
        {
            add_callback([this](){painter_.notify();});
        }
        VertexPainterBufferObserver::VertexPainterBufferObserver(VertexPainterBufferObserver&& other)
            : OBuffer{std::move(other.get_target())}
            , painter_{other.painter_}
        {
            add_callback([this](){painter_.notify();});
        }
        VertexPainterBufferObserver& VertexPainterBufferObserver::operator=(const VertexPainterBufferObserver& other)
        {
            if (this != &other)
            {
                set_target(other.get_target());
                add_callback([this](){painter_.notify();});
            }
            return *this;
        }
        VertexPainterBufferObserver& VertexPainterBufferObserver::operator=(VertexPainterBufferObserver&& other)
        {
            if (this != &other)
            {
                set_target(std::move(other.get_target()));
                add_callback([this](){painter_.notify();});
            }
            return *this;
        }

        std::shared_ptr<VertexPainterBuffer> VertexPainterBufferObserver::get_painter_buffer() const
        {
            return get_target();
        }
        void VertexPainterBufferObserver::set_painter_buffer(std::shared_ptr<VertexPainterBuffer> painter_buff)
        {
            set_target(painter_buff);
            add_callback([this](){painter_.notify();});
            painter_.notify();
        }

        
    }

    //----------------------------------------------------------------------
    
    std::shared_ptr<VertexPainter> VertexPainterComposite::copied_painter_ {};

    VertexPainterComposite::VertexPainterComposite()
        : VertexPainter{}
        , color_distributor_{std::make_shared<impl::ColorGeneratorComposite>(*this)}
        , main_painter_{std::make_shared<VertexPainterBuffer>(
            std::make_shared<VertexPainterLinear>(color_distributor_)), *this}
        , vertices_index_groups_{}
        , child_painters_{{impl::VertexPainterBufferObserver(std::make_shared<VertexPainterBuffer>(), *this)}}
    {
    }

    VertexPainterComposite::VertexPainterComposite(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
        , color_distributor_{std::make_shared<impl::ColorGeneratorComposite>(*this)}
        , main_painter_{std::make_shared<VertexPainterBuffer>(
                            std::make_shared<VertexPainterLinear>(color_distributor_)), *this}
        , vertices_index_groups_{}
        , child_painters_{{impl::VertexPainterBufferObserver(std::make_shared<VertexPainterBuffer>(), *this)}}

    {
        // TODO: 'gen' unused
    }
    
    VertexPainterComposite::VertexPainterComposite(const VertexPainterComposite& other)
        : VertexPainter{other}
        , color_distributor_{other.color_distributor_}
        , main_painter_{other.main_painter_}
        , vertices_index_groups_{other.vertices_index_groups_}
        , child_painters_{other.child_painters_}
    {
    }

    VertexPainterComposite::VertexPainterComposite(VertexPainterComposite&& other)
        : VertexPainter{std::move(other)}
        , color_distributor_{std::move(other.color_distributor_)}
        , main_painter_{std::move(other.main_painter_)}
        , vertices_index_groups_{std::move(other.vertices_index_groups_)}
        , child_painters_{std::move(other.child_painters_)}
    {
    }

    VertexPainterComposite& VertexPainterComposite::operator=(const VertexPainterComposite& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            color_distributor_ = other.color_distributor_;
            main_painter_ = other.main_painter_;
            child_painters_ = other.child_painters_;
            vertices_index_groups_ = other.vertices_index_groups_;
        }
        return *this;
    }

    VertexPainterComposite& VertexPainterComposite::operator=(VertexPainterComposite&& other)
    {
        if (this != &other)
        {
            VertexPainter::operator=(other);
            color_distributor_ = std::move(other.color_distributor_);
            main_painter_ = std::move(other.main_painter_);
            child_painters_ = std::move(other.child_painters_);
            vertices_index_groups_ = std::move(other.vertices_index_groups_);
        }
        return *this;
    }

    std::shared_ptr<VertexPainter> VertexPainterComposite::clone_impl() const
    {
        return std::make_shared<VertexPainterComposite>(get_target()->get_generator()->clone());
    }
    
    std::list<std::shared_ptr<VertexPainterBuffer>> VertexPainterComposite::get_child_painters() const
    {
        std::list<std::shared_ptr<VertexPainterBuffer>> list;
        for (const auto& observer : child_painters_)
        {
            list.push_back(observer.get_target());
        }
        return list;
    }

    std::shared_ptr<VertexPainterBuffer> VertexPainterComposite::get_main_painter() const
    {
        return main_painter_.get_painter_buffer();
    }

    void VertexPainterComposite::set_main_painter(std::shared_ptr<VertexPainterBuffer> painter_buff)
    {
        // hack
        vertices_index_groups_.clear();
        for(auto i=0u; i<child_painters_.size(); ++i)
        {
            vertices_index_groups_.push_back({});
        }


        painter_buff->get_painter()->get_generator_buffer()->set_generator(color_distributor_);
        main_painter_.set_painter_buffer(painter_buff);
        notify(); // double call?
    }

                 
    void VertexPainterComposite::set_child_painters(const std::list<std::shared_ptr<VertexPainterBuffer>> painters)
    {
        std::list<impl::VertexPainterBufferObserver> list;
        for (const auto& painter : painters)
        {
            list.push_back(impl::VertexPainterBufferObserver(painter, *this));
        }
        child_painters_ = list;
        notify();
    }

    void VertexPainterComposite::paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box)

    {
        auto vertices_copy = vertices;
        auto iteration_of_vertices_copy = iteration_of_vertices;
        color_distributor_->reset_index();
        vertices_index_groups_.clear();
        for(auto i=0u; i<child_painters_.size(); ++i)
        {
            vertices_index_groups_.push_back({});
        }

        main_painter_.get_painter_buffer()->get_painter()->paint_vertices(vertices_copy,
                                                                          iteration_of_vertices,
                                                                          max_recursion,
                                                                          bounding_box);

        std::vector<std::vector<sf::Vertex>> vertices_groups;
        std::vector<std::vector<int>> iteration_of_vertices_groups;
        for(const auto& v : vertices_index_groups_)
        {
            std::vector<sf::Vertex> vertices_part;
            std::vector<int> iteration_of_vertices_part;
            for(auto idx : v)
            {
                vertices_part.push_back(vertices_copy.at(idx));
                iteration_of_vertices_part.push_back(iteration_of_vertices_copy.at(idx));
            }
            vertices_groups.push_back(vertices_part);
            iteration_of_vertices_groups.push_back(iteration_of_vertices_part);
        }

        auto idx = 0u;
        for(auto& painter_it : child_painters_)
        {
            painter_it.get_painter_buffer()->get_painter()->paint_vertices(vertices_groups.at(idx),
                                                                           iteration_of_vertices_groups.at(idx),
                                                                           max_recursion,
                                                                           bounding_box);
            ++idx;
        }

        std::vector<sf::Vertex> flattened_groups_;
        std::size_t total_size = 0;
        for (const auto& sub : vertices_groups)
        {
            total_size += sub.size();
        }
        flattened_groups_.reserve(total_size);
        for (const auto& sub : vertices_groups)
        {
            flattened_groups_.insert(flattened_groups_.end(), sub.begin(), sub.end());
        }

        auto i = 0u;
        for (const auto& v : vertices_index_groups_)
        {
            for (auto idx : v)
            {
                vertices_copy.at(idx) = flattened_groups_.at(i++);
            }
        }

        vertices = vertices_copy;
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
}
