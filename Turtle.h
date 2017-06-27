#ifndef TURTLE_H
#define TURTLE_H


#include <vector>
#include <string>
#include <cmath>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "math.h"
#include "LSystem.h"

namespace logo
{

    struct Walk
    {
        sf::Vector2f curr_pos { 0, 0 };
        float curr_angle { 0 };
        std::vector<sf::Vertex> vertices { };
    };

    struct Turtle;
    
    std::vector<sf::Vertex> compute_vertices(const Turtle& turtle, int n_iter = 1);
    void go_forward(const Turtle& turtle, Walk& walk);
    void turn_right(const Turtle& turtle, Walk& walk);
    void turn_left(const Turtle& turtle, Walk& walk);
    
    struct Turtle
    {
        using order = std::function<void(const Turtle& turtle, Walk& walk)>;
        
        const sf::Vector2f starting_pos { 600, 200 };
        const double starting_angle { 0 };
    
        // May become non-const when creating more complex systems and by
        // consequence migrated to TurtleWalk
        const lsys::LSystem lsys { "F", { {'F', "G-F-G"}, {'G', "F+G+F"} } };
        const std::unordered_map<char, order> orders
            {
                { 'F', go_forward },
                { 'G', go_forward },
                { '+', turn_right },
                { '-', turn_left  },
            };
        const double delta_angle { math::degree_to_rad(60) };
        const int step { 3 };
    };
}

#endif
