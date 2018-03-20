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

        expand_to_neighbors();
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
                middleX -= bounding_box_.width / (2. * (i+1));
            }
            else
            {
                index |= 1;
                middleX += bounding_box_.width / (2. * (i+1));
            }
            index <<= 1;
            
            if (v.position.y < middleY)
            {
                middleY -= bounding_box_.height / (2. * (i+1));
            }
            else
            {
                index |= 1;
                middleY += bounding_box_.height / (2. * (i+1));
            }

            if (i != n_subdivision)
            {
                index <<= 1;
            }
        }
        return index;
    }

    void LSystemView::expand_to_neighbors()
    {
        for (unsigned int index=0; index<boxes_.size(); ++index)
        {
            auto around = neighbors(index);
            std::vector<float> farest_right_neighbors;
            std::vector<float> farest_up_neighbors;
            std::vector<float> farest_down_neighbors;
            if (around.right != -1)
            {
                auto& right = boxes_.at(around.right);
                if (!std::isnan(right.left))
                {
                    farest_right_neighbors.push_back(right.left);
                }

                // if (around.updiag != -1 &&
                //     !std::isnan(boxes_.at(around.updiag).left))
                // {
                //     farest_right_neighbors.push_back(boxes_.at(around.updiag).left);
                //     farest_up_neighbors.push_back(boxes_.at(around.updiag).top);
                // }
                // if (around.downdiag != -1 &&
                //     !std::isnan(boxes_.at(around.downdiag).left))
                // {
                //     farest_right_neighbors.push_back(boxes_.at(around.downdiag).left);
                //     farest_down_neighbors.push_back(boxes_.at(around.downdiag).top);
                // }
            }
            if(around.down != -1 &&
               !std::isnan(boxes_.at(around.down).left))
            {
                farest_down_neighbors.push_back(boxes_.at(around.down).top);
            }

            if (farest_right_neighbors.size() != 0)
            {
                auto max = std::max_element(farest_right_neighbors.begin(),
                                            farest_right_neighbors.end());
                auto& b = boxes_.at(index);
                if (b.left + b.width < *max)
                {
                    b.width = *max - b.left;
                }
            }
            
            if (farest_up_neighbors.size() != 0)
            {
                auto max = std::max_element(farest_up_neighbors.begin(),
                                            farest_up_neighbors.end());
                auto& b = boxes_.at(index);
                if (b.top < *max)
                {
                    b.height += b.top - *max;
                    b.top = *max;
                }
            }

            if (farest_down_neighbors.size() != 0)
            {
                auto max = std::max_element(farest_down_neighbors.begin(),
                                            farest_down_neighbors.end());
                auto& b = boxes_.at(index);
                if (b.top + b.height < *max)
                {
                    b.height = *max - b.top;
                }
            }
        }
    }

    LSystemView::Neighbors LSystemView::neighbors(int index) const
    {
        int right = right_neighbor(index);
        int down = down_neighbor(index);
        if (right != -1)
        {
            return {up_neighbor(right), right, down_neighbor(right), down };
        }
        else
        {
            return {-1, right, -1, down };
        }
    }
    int LSystemView::up_neighbor (int index) const
    {
        for (unsigned int i=0; i<n_subdivision; ++i)
        {
            if ((index & (1 << (i*2))) == 1)
            {
                index ^= 1 << (i*2);
                break;
            }
            else
            {
                index |= 1 << (i*2);
            }
            if (i == n_subdivision-1)
            {
                index = -1;
            }
        }
        return index;
    }
    int LSystemView::right_neighbor (int index) const
    {
        for (unsigned int i=0; i<n_subdivision; ++i)
        {
            if ((index & (1 << (i*2+1))) == 0)
            {
                index |= 1 << (i*2+1);
                break;
            }
            else
            {
                index ^= 1 << (i*2+1);
            }
            if (i == n_subdivision-1)
            {
                index = -1;
            }
        }
        return index;
    }
    int LSystemView::down_neighbor (int index) const
    {
        for (unsigned int i=0; i<n_subdivision; ++i)
        {
            if ((index & (1 << (i*2))) == 0)
            {
                index |= 1 << (i*2);
                break;
            }
            else
            {
                index ^= 1 << (i*2);
            }
            if (i == n_subdivision-1)
            {
                index = -1;
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
