#include "procgui.h"
#include "LSystemView.h"

namespace procgui
{
    using namespace drawing;
    
    LSystemView::LSystemView(std::shared_ptr<LSystem> lsys,
                             std::shared_ptr<drawing::InterpretationMap> map,
                             drawing::DrawingParameters params)
        : Observer<LSystem> {lsys}
        , Observer<InterpretationMap> {map}
        , lsys_buff_ {lsys}
        , interpretation_buff_ {map}
        , params_ {params}
        , bounding_box_ {}
        , boxes_{}
    {
        Observer<LSystem>::add_callback([this](){compute_vertices();});
        Observer<InterpretationMap>::add_callback([this](){compute_vertices();});
        compute_vertices();
    }

    drawing::DrawingParameters& LSystemView::get_parameters()
    {
        return params_;
    }
    LSystemBuffer& LSystemView::get_lsystem_buffer()
    {
        return lsys_buff_;
    }
    InterpretationMapBuffer& LSystemView::get_interpretation_buffer()
    {
        return interpretation_buff_;
    }

    
    void LSystemView::compute_vertices()
    {
        vertices_ = drawing::compute_vertices(lsys_buff_.get_target(),
                                              interpretation_buff_.get_target(),
                                              params_);
        compute_bounding_box();
        compute_subdivisions();
    }

    void LSystemView::compute_bounding_box()
    {
        if (vertices_.size() == 0)
        {
            return;
        }
        const auto& first = vertices_.at(0);
        float top = first.position.y,  down = first.position.y;
        float left = first.position.x, right = first.position.x;
        for (const auto& v : vertices_)
        {
            if (v.position.y < top)
            {
                top = v.position.y;
            }
            else if (v.position.y > down)
            {
                down = v.position.y;
            }

            if (v.position.x > right)
            {
                right = v.position.x;
            }
            else if (v.position.x < left)
            {
                left = v.position.x;
            }
        }
        bounding_box_ = {left, top, right - left, down - top};;
    }

    void LSystemView::compute_subdivisions()
    {
        if (vertices_.size() == 0)
        {
            return;
        }

        init_subdivisions();

        for (auto v : vertices_)
        {
            int index = find_index(v);
            
            auto& box = boxes_.at(index);
            if (std::isnan(box.left))
            {
                box = {v.position.x, v.position.y, 0, 0};
            }
            else
            {
                if (v.position.x < box.left)
                {
                    box.width += box.left - v.position.x;
                    box.left = v.position.x;
                }
                else if (v.position.x > box.left + box.width)
                {
                    box.width += v.position.x - box.left - box.width;
                }

                if (v.position.y < box.top)
                {
                    box.height += box.top - v.position.y;
                    box.top = v.position.y;
                }
                else if (v.position.y > box.top + box.height)
                {
                    box.height += v.position.y - box.top - box.height;
                }
            }
        }
    }

    void LSystemView::init_subdivisions()
    {
        for (unsigned index = 0; index < boxes_.size(); ++index)
        {
            boxes_.at(index) = sf::FloatRect(NAN, NAN, NAN, NAN);
        }
    }

    int LSystemView::find_index(const sf::Vertex& v) const
    {
        static_assert(n_subdivision * 2 < sizeof(int) * 8, "Too many subdivisions for the index");
        int index = 0;
        int middleX = bounding_box_.left + bounding_box_.width / 2.;
        int middleY = bounding_box_.top + bounding_box_.height / 2.;
        for (unsigned int i=1; i <= n_subdivision; ++i)
        {
            if (v.position.x < middleX)
            {
                index |= 1;
                middleX -= bounding_box_.width / (2. * (i+1));
            }
            else
            {
                middleX += bounding_box_.width / (2. * (i+1));
            }
            index <<= 1;
            
            if (v.position.y < middleY)
            {
                index |= 1;
                middleY -= bounding_box_.height / (2. * (i+1));
            }
            else
            {
                middleY += bounding_box_.height / (2. * (i+1));
            }

            if (i != n_subdivision)
            {
                index <<= 1;
            }
        }
        return index;
    }


    void LSystemView::draw(sf::RenderTarget &target)
    {
        if (interact_with(*this, ""))
        {
            compute_vertices();
        };
        target.draw(vertices_.data(), vertices_.size(), sf::LineStrip);

        std::array<sf::Vertex, 5> box =
            {{ {{ bounding_box_.left, bounding_box_.top}},
               {{ bounding_box_.left, bounding_box_.top + bounding_box_.height}},
               {{ bounding_box_.left + bounding_box_.width, bounding_box_.top + bounding_box_.height}},
               {{ bounding_box_.left + bounding_box_.width, bounding_box_.top}},
               {{ bounding_box_.left, bounding_box_.top}}}};
        target.draw(box.data(), box.size(), sf::LineStrip);

        std::vector<sf::Vertex> boxes;
        for (const auto& b : boxes_)
        {
            boxes.push_back({{ b.left, b.top}, sf::Color(255,0,0,50)});
            boxes.push_back({{ b.left, b.top + b.height}, sf::Color(255,0,0,50)});
            boxes.push_back({{ b.left + b.width, b.top + b.height}, sf::Color(255,0,0,50)});
            boxes.push_back({{ b.left + b.width, b.top}, sf::Color(255,0,0,50)});
        }
        target.draw(boxes.data(), boxes.size(), sf::Quads);
                    
    }
}
