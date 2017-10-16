#ifndef TURTLE_H
#define TURTLE_H


#include <vector>
#include <string>
#include <cmath>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "helper_math.h"
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
namespace drawing
{
    // Forward declarations:
    struct DrawingParameters;

    // This data structure contains all informations concerning the
    // current state of the interpretation. It could be enriched later
    // by some attributes of DrawingParameters to allow more
    // flexibility.
    // Note: Turtle is placed into an implementation namespace as it
    // is only instanciated and used in 'compute_vertices()' to
    // generate the vertices.
    namespace impl
    {
        struct Turtle
        {
            explicit Turtle(const DrawingParameters& parameters);
            
            // All the parameters necessary to compute the vertices.
            // Note: This is a non-owning reference. As Turtle is only
            // used as a temporary object in the 'drawing' namespace,
            // the danger of lifetime management should be
            // lower. Every modification should be done with extra
            // care.
            const DrawingParameters& parameters;

            // The current position and angle of the turtle.
            sf::Vector2f position { 0, 0 };
            float angle { 0 };

            // Each time the Turtle changes its position, the new one is
            // saved in a vertex. 'vertices' represent the whole path
            // walked by the Turtle.
            std::vector<sf::Vertex> vertices { };
        };
    }

    // This data structure contains all constant informations
    // necessary to interpret the result of a L-system. Each attribute
    // can be freely initialized and modified, there are no invariant.
    // During an interpretation, this structure will not be
    // modified. Some attributes like 'delta_angle' or 'step' could
    // be, in the future, removed from this struct and moved to the
    // 'Turtle' struct to allow more flexibility.
    struct DrawingParameters
    {
        // The starting position and angle of the Turtle.
        sf::Vector2f starting_position { 0, 0 };
        float starting_angle { 0 };

        // When 'turn_left' or 'turn_right' orders are executed, the
        // Turtle pivot at a 'delta_angle' angle (in
        // radian). Initialized at an arbitrary value.
        double delta_angle { math::pi / 2 };

        // When 'move_forward' order is executed, the Turtle will move
        // forward 'step' pixels (at default zoom level). Initialized
        // at an arbitrary value. 
        int step { 5 };

        // The number of iterations done by the L-system.
        int n_iter { 0 };
    };


    // LSysInterpretation is the link between a LSystem and a
    // graphical Turtle interpretation. Each symbol of a L-System is
    // associated with an 'order' which modifies a Turtle.
    struct LSysInterpretation
    {
        // An 'order' is a function modifying a 'Turtle'. Semantically
        // it is an instruction like "move forward" or "turn left".
        using order = std::function<void(impl::Turtle& turtle)>;

        // 'interpretation' is a map linking a symbol of the
        // vocabulary of a L-system to an order. During the
        // interpretation, if the character is encountered, the
        // associated order will be executed.
        using interpretation_map = std::unordered_map<char, order>;
        
        lsys::LSystem lsys { };
        interpretation_map map { };
    };

    
    // Compute all vertices of a turtle interpretation of a L-system.
    // This function iterates first 'n_iter' times the L-system
    // 'turtle.lsys' then interprates the result.
    std::vector<sf::Vertex> compute_vertices(const LSysInterpretation& interpretation,
                                             const DrawingParameters& parameters);

    // All the orders currently defined. //
    // "Turn right" means "turn clockwise" AS SEEN ON THE SCREEN.
    void turn_right(impl::Turtle& turtle);
    void turn_left(impl::Turtle& turtle);
    void go_forward(impl::Turtle& turtle);
}

#endif
