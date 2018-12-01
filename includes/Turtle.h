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
            explicit Turtle(const DrawingParameters& parameters,
                            const std::vector<int>& str_recursion);
            
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
            std::vector<sf::Vertex> vertices { };


            // All the recursion count produced by the LSystem. For each new
            // vertices, its recursion count will be copies to 'vertices_recursion'.
            const std::vector<int> str_recursion;
            
            // Index indicating the position in str_recursion.
            std::size_t recursion_index {0};

            // For each new vertex created, its recursion count is saved. The
            // recursion count is produced by the LSystem and saved in
            // 'str_recursion', the operation is simply to copy it in this
            // vector corresponding to the vertices.
            std::vector<int> vertices_recursion;
        };
    }

    // Compute all vertices and their recursion count of a turtle interpretation
    // of a L-system.  First, this function iterates 'parameters.n_iter' times
    // the LSystem 'lsys', using and modifying its cache. Then, it interprates
    // the result with 'interpretation' and 'parameters'. The third returned
    // value is the maximum number of recursion count.
    std::tuple<std::vector<sf::Vertex>, std::vector<int>, int>
        compute_vertices(LSystem& lsys,
                         InterpretationMap& interpretation,
                         const DrawingParameters& parameters);
}


#endif // DRAWING_TURTLE_H
