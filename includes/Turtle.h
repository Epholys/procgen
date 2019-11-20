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
    // generate the vertices. // TODO FALSE
    // TODO COMMENTS
    namespace impl
    {
        struct Turtle  // TODO CLASS
        {
            explicit Turtle(const DrawingParameters& parameters);

            // // All the parameters necessary to compute the vertices.
            // // Note: This is a non-owning reference. As Turtle is only
            // // used as a temporary object in the 'drawing' namespace,
            // // the danger of lifetime management should be
            // // lower. Every modification should be done with extra
            // // care.
            // const DrawingParameters& parameters;

            // Cosine and sine of the 'delta_angle'. Computed once to speed up
            // calculations.
            double cos {0};
            double sin {1};
            double step {10};

            // The current position and direction of the Turtle.
            struct State {
                sf::Vector2<double> position;
                sf::Vector2<double> direction;
            };
            State state { {0, 0}, // The position on-screen is set in
                                  // LSystemView with transforms.
                          {0, 1} };

            // The state of a turtle can be saved and loaded in a stack.
            std::stack<State> stack { };

            // Each time the Turtle changes its position, the new one is saved
            // in a vertex. However, we can jump from position to position, so
            // it there is additional transparent vertices between jumps.
            std::vector<sf::Vertex> vertices { };

            // Index indicating the position in 'iteration_vec'.
            std::uint8_t iteration {0};

            // For each new vertex created, its iteration count is saved. The
            // iteration count is produced by the LSystem and saved in
            // 'iteration_vec', the operation is simply to copy it in this
            // vector corresponding to the vertices.
            std::vector<std::uint8_t> iteration_of_vertices;

            struct TurtleProduction
            {
                std::vector<sf::Vertex>& vertices;
                std::vector<std::uint8_t>& iterations;
            };
            // Compute all vertices and their iteration count of a turtle interpretation
            // of a L-system.  First, this function iterates 'parameters.n_iter' times
            // the LSystem 'lsys', using and modifying its cache. Then, it interprates
            // the result with 'interpretation' and 'parameters' // TODO COMMENT.
            TurtleProduction compute_vertices(const std::string& lsystem_production,
                                              const std::vector<std::uint8_t>& iterations,
                                              const DrawingParameters& parameters,
                                              const InterpretationMap& interpretation);

        private:
            std::size_t iteration_index {0};

            void init_from_parameters (const DrawingParameters& parameters);
        };
    }
}


#endif // DRAWING_TURTLE_H
