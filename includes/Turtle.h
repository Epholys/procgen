#ifndef DRAWING_TURTLE_H
#define DRAWING_TURTLE_H


#include <vector>
#include <stack>

#include "LSystem.h"
#include "DrawingParameters.h"
#include "InterpretationMap.h"

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

            // Cosine and sine of the 'delta_angle'. Computed once to speed up
            // calculations.
            const double cos = std::cos(parameters.get_delta_angle());
            const double sin = std::sin(parameters.get_delta_angle());


            // The current position and direction of the Turtle.
            struct State {
                sf::Vector2<double> position;
                sf::Vector2<double> direction;
            };
            State state { {0, 0}, // The position on-screen is set in
                                  // LSystemView with transforms.
                          {std::cos(parameters.get_starting_angle()),
                           std::sin(parameters.get_starting_angle())}};

            // The state of a turtle can be saved and loaded in a stack.
            std::stack<State> stack { };
            
            // Each time the Turtle changes its position, the new one is saved
            // in a vertex. However, we can jump from position to position, so
            // it there is additional transparent vertices between jumps.
            std::vector<sf::Vertex> vertices { sf::Vector2f{state.position} };
        };
    }

    // Compute all vertices of a turtle interpretation of a L-system.
    // First, this function iterates 'parameters.n_iter' times the LSystem
    // 'lsys', using and modifying its cache. Then, it interprates the result
    // with 'interpretation' and 'parameters'.
    std::vector<sf::Vertex> compute_vertices(LSystem& lsys,
                                             InterpretationMap& interpretation,
                                             const DrawingParameters& parameters);
}


#endif // DRAWING_TURTLE_H
