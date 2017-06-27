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


struct Turtle : public sf::Drawable
{
    
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

    void compute_vertices(int n_iter = 1);

    void add_order(char c, const std::function<void(Turtle*)>& fn);


    std::unordered_map<char, std::function<void()>> orders;
    
    const float delta_angle = math::degree_to_rad(60);
    const int step = 5;
    lsys::LSystem lsys {"F", { {'F', "G-F-G"}, {'G', "F+G+F"} }};


    float curr_angle = math::degree_to_rad(90);
    sf::Vector2f curr_pos  {600, 200};
    std::vector<sf::Vertex> vertices = { sf::Vertex(curr_pos) };
};


void go_forward(Turtle* turtle);
void turn_right(Turtle* turtle);
void turn_left(Turtle* turtle);

#endif
