#include "VertexPainter.h"
#include "procgui.h"
#include "geometry.h"

namespace colors
{
    VertexPainter::VertexPainter()
        : generator_{nullptr}
    {
        generator_ = std::make_shared<LinearGradient>(LinearGradient::keys({{sf::Color::Red, 0.}, {sf::Color::Green, 0.75}, {sf::Color::Blue, 1.}}));
    }
    VertexPainter::VertexPainter(std::shared_ptr<ColorGenerator> gen)
        : generator_{gen}
    {
    }
    
    void VertexPainter::paint_vertices(std::vector<sf::Vertex>& vertices, sf::FloatRect bounding_box)
    {
        if (!generator_)
        {
            return;
        }

        struct Line
        {
            sf::Vector2f point;
            sf::Vector2f direction;
        };
        enum bound { Upper=0, Rightmost, Bottom, Leftmost };
        std::vector<Line> bounds (4);
        bounds.at(Upper) = {{bounding_box.left, bounding_box.top}, {1,0}};
        bounds.at(Rightmost) = {{bounding_box.left+bounding_box.width, bounding_box.top}, {0,1}};
        bounds.at(Bottom) = {{bounding_box.left+bounding_box.width, bounding_box.top+bounding_box.height}, {-1,0}};
        bounds.at(Leftmost) = {{bounding_box.left, bounding_box.top+bounding_box.height}, {0,-1}};
                
        const float angle = 45.;
        std::pair<Line, Line> higher_bounds; // at the direction pointed by the angle
        if (angle >= 0 && angle < 180.)
        {
            higher_bounds.first = bounds.at(Upper);
        }
        else
        {
            higher_bounds.first = bounds.at(Bottom);
        }
        if (angle >= 90. && angle < 270.)
        {
            higher_bounds.second = bounds.at(Leftmost);
        }
        else
        {
            higher_bounds.second = bounds.at(Rightmost);
        }
        
        const sf::Vector2f middle {bounding_box.left+bounding_box.width/2.f,
                                   bounding_box.top+bounding_box.height/2.f};
        const sf::Vector2f direction {std::cos(math::degree_to_rad(angle)), std::sin(math::degree_to_rad(angle))};

        sf::Vector2f intersection {middle};
        geometry::intersection(middle, direction, higher_bounds.first.point, higher_bounds.first.direction, intersection);
        float distance_from_middle = geometry::distance(middle, intersection);
        
        sf::Vector2f intersection2 {middle};
        geometry::intersection(middle, direction, higher_bounds.second.point, higher_bounds.second.direction, intersection2);
        float distance_from_middle2 = geometry::distance(middle, intersection2);

        if (distance_from_middle2 < distance_from_middle)
        {
            intersection = intersection2;
        }
        
        sf::Vector2f opposite_intersection = middle - (intersection - middle);

        float distance = geometry::distance(opposite_intersection, intersection);
        
        sf::Vector2f projection;
        sf::Vector2f AB = opposite_intersection - intersection;
        float AB_squared = AB.x*AB.x + AB.y*AB.y;
        if (AB_squared == 0)
        {
            projection = intersection;
        }

        for (auto& v : vertices)
        {
            sf::Vector2f Ap = v.position - intersection;
            float t = (Ap.x*AB.x + Ap.y*AB.y) / AB_squared;
            projection = intersection + t*AB;

            float lerp = std::sqrt(std::pow(opposite_intersection.x-projection.x,2)+
                                   std::pow(opposite_intersection.y-projection.y,2)) / distance;

            sf::Color color = generator_->get(lerp);
            color.a = v.color.a;
            v.color = color;
        }
    }

    void VertexPainter::interact_with()
    {
        if(procgui::interact_with(generator_, ""))
        {
            notify();
        }
    }
}
