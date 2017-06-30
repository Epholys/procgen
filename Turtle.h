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

// A Turtle is a computer graphics concept from the language
// logo. Imagine a pen attached to a turtle on the screen. The
// turtle can go forward or pivot. As it moves around, the pen
// draws straight lines. So, with some simple instructions, it is
// possible to create complex drawings.
//
// In this project, the Turtle is implemented as a graphic
// interpretation of the result of L-systems. Each character of
// L-system vocabulary is associated to an order like "move
// forward". The string is intepreted linearly, character by
// character, to form a complete drawing.
namespace logo
{
    // Forward declarations:
    struct Turtle;
    struct Walk;

    // This data structure contains all constant informations
    // necessary to interpret the result of a L-system. Each attribute
    // can be freely initialized and modified, there are not
    // invariant.
    // During an interpretation, this structure will not be
    // modified. Some attributes like 'delta_angle' or 'step' could
    // be, in the future, removed from this struct and moved to the
    // 'Walk' struct to allow more flexibility.
    struct Turtle
    {
        // An 'order' is a function modifying a 'Walk' according to
        // the datas of a 'Turtle'. Simply put, it is an instruction
        // like "move forward" or "turn left".
        using order = std::function<void(const Turtle& turtle, Walk& walk)>;

        // A 'lsys_interpretation' is a map linking a symbol of the
        // vocabulary of a L-system to an order. During the
        // interpretation, if the character is encountered, the
        // associated order will be executed.
        using lsys_interpretation = std::unordered_map<char, order>;

        
        // The starting position and angle of the Turtle.
        sf::Vector2f starting_pos { 0, 0 };
        double starting_angle { 0 };

        // When 'turn_left' or 'turn_right' orders are executed, the
        // Turtle pivot at a 'delta_angle' angle (in
        // radian). Initialized at an arbitrary value.
        double delta_angle { math::pi / 2 };

        // When 'move_forward' order is executed, the Turtle will move
        // forward 'step' pixels (at default zoom level). Initialized
        // at an arbitrary value. 
        int step { 5 };

        // The Turtle's L-system and its interpretation.
        lsys::LSystem lsys { };
        lsys_interpretation interpretations { };
    };


    // This data structure contains all informations concerning the
    // current state of the interpretation. It could be enriched later
    // by some attributes of Turtle to allow more flexibility.
    struct Walk
    {
        // The current position and angle of the turtle.
        sf::Vector2f curr_pos { 0, 0 };
        float curr_angle { 0 };

        // Each time the turtle changes its position, the new one is
        // saved in a vertex. 'vertices' represent the whole path
        // walked by the turtle.
        std::vector<sf::Vertex> vertices { };
    };

    
    // Compute all vertices of a turtle interpretation of a L-system.
    // This function iterates first 'n_iter' times the L-system
    // 'turtle.lsys' then interprates the result.
    std::vector<sf::Vertex> compute_vertices(const Turtle& turtle, int n_iter = 1);

    // All the orders currently defined.
    void go_forward(const Turtle& turtle, Walk& walk);

    // "Turn right" means "turn clockwise" AS SEEN ON THE SCREEN.
    void turn_right(const Turtle& turtle, Walk& walk);
    void turn_left(const Turtle& turtle, Walk& walk);
}

#endif
