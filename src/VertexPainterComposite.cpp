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
            auto it = painter_.vertices_index_groups_.begin();
            for (auto i=0u; i<index; ++i)
            {
                ++it;
            }
            it->push_back(index_++);
            
            return sf::Color::Transparent;
        }

        void ColorGeneratorComposite::reset_index()
        {
            index_ = 0;
        }        
        
        std::shared_ptr<ColorGenerator> ColorGeneratorComposite::clone_impl() const
        {
            return std::make_shared<ColorGeneratorComposite>(*this);
        }
    }
    
    VertexPainterComposite::VertexPainterComposite()
        : VertexPainter{}
        , vertices_copy_ {}
        , color_distributor_{std::make_shared<impl::ColorGeneratorComposite>(*this)}
        , main_painter_{std::make_shared<VertexPainterBuffer>(
                            std::make_shared<VertexPainterLinear>(color_distributor_))}
        , vertices_index_groups_{}
        , child_painters_{}
    {
    }

    VertexPainterComposite::VertexPainterComposite(const std::shared_ptr<ColorGenerator> gen)
        : VertexPainter{gen}
        , vertices_copy_ {}
        , color_distributor_{std::make_shared<impl::ColorGeneratorComposite>(*this)}
        , main_painter_{std::make_shared<VertexPainterBuffer>(
                            std::make_shared<VertexPainterLinear>(color_distributor_))}
        , vertices_index_groups_{}
        , child_painters_{}
    {
        auto c1 = std::make_shared<LinearGradient>(LinearGradient({{sf::Color::Red, 0}, {sf::Color::Yellow, 1}}));
        auto c2 = std::make_shared<LinearGradient>(LinearGradient({{sf::Color::Blue, 0}, {sf::Color::Magenta, 1}}));

        auto b1 = std::make_shared<VertexPainterLinear>(c1);
        auto b2 = std::make_shared<VertexPainterRadial>(c2);
        child_painters_.push_back(std::make_shared<VertexPainterBuffer>(b1));
        child_painters_.push_back(std::make_shared<VertexPainterBuffer>(b2));
    }
    
    VertexPainterComposite::VertexPainterComposite(const VertexPainterComposite& other)
        : VertexPainter{other}
        , vertices_copy_{other.vertices_copy_}
        , color_distributor_{other.color_distributor_}
        , main_painter_{other.main_painter_}
        , vertices_index_groups_{other.vertices_index_groups_}
        , child_painters_{other.child_painters_}
    {
        auto c1 = std::make_shared<LinearGradient>(LinearGradient({{sf::Color::Red, 0}, {sf::Color::Yellow, 1}}));
        auto c2 = std::make_shared<LinearGradient>(LinearGradient({{sf::Color::Blue, 0}, {sf::Color::Magenta, 1}}));

        auto b1 = std::make_shared<VertexPainterLinear>(c1);
        auto b2 = std::make_shared<VertexPainterRadial>(c2);
        child_painters_.push_back(std::make_shared<VertexPainterBuffer>(b1));
        child_painters_.push_back(std::make_shared<VertexPainterBuffer>(b2));
    }

    VertexPainterComposite::VertexPainterComposite(VertexPainterComposite&& other)
        : VertexPainter{std::move(other)}
        , vertices_copy_{std::move(other.vertices_copy_)}
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
            vertices_copy_ = other.vertices_copy_;
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
            vertices_copy_ = std::move(other.vertices_copy_);
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
    

    void VertexPainterComposite::paint_vertices(std::vector<sf::Vertex>& vertices,
                                    const std::vector<int>& iteration_of_vertices,
                                    int max_recursion,
                                    sf::FloatRect bounding_box)

    {
        vertices_copy_ = vertices;
        color_distributor_->reset_index();
        vertices_index_groups_.clear();

        main_painter_->get_painter()->paint_vertices(vertices_copy_,
                                                     iteration_of_vertices,
                                                     max_recursion,
                                                     bounding_box);

        std::vector<std::vector<sf::Vertex>> vertices_groups;
        for(const auto& v : vertices_index_groups_)
        {
            std::vector<sf::Vertex> part;
            for(auto idx : v)
            {
                part.push_back(vertices_copy_.at(idx));
            }
            vertices_groups.push_back(part);
        }

        auto idx = 0u;
        for(auto& painter_it : child_painters_)
        {
            painter_it->get_painter()->paint_vertices(vertices_groups.at(idx),
                                                      iteration_of_vertices,
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
                vertices_copy_.at(idx) = flattened_groups_.at(i++);
            }
        }

        vertices = vertices_copy_;
    }
}
