#ifndef DRAWING_TURTLE_H
#define DRAWING_TURTLE_H


#include <vector>
#include <stack>

#include "LSystem.h"
#include "DrawingParameters.h"
#include "InterpretationMap.h"

// A Turtle is a computer graphics concept from the language logo. Imagine a pen
// attached to a turtle on the screen. The turtle can go forward or pivot. As it
// moves around, the pen draws straight lines. So, with some simple
// instructions, it is possible to create complex drawings.
//
// In this project, the Turtle is implemented as a graphic interpretation of the
// result of L-systems. Each character of L-system vocabulary is associated to
// an order like "move forward". The string is intepreted linearly, character by
// character, to form a complete drawing.
namespace drawing
{
    // This data structure contains all informations concerning the
    // current state of the interpretation. It could be enriched later
    // by some attributes of DrawingParameters to allow more
    // flexibility.
    //
    // The usage is to create one with DrawingParameters and then call
    // 'compute_vertices()' with the data from a 'LSystem' and an
    // 'InterpretationMap'.
    //
    // Invariant:
    //   - 'vertices_', 'iterations_' and 'transparency_' have the same number
    //   of elements. This invariant is only checked at the end of
    //   'compute_vertices()', as these members must be manipulated by the
    //   'IntepretationMap' functions
    //   - 'iteration_index_' and 'iteration_depth_' are correctly associated.
    //
    struct Turtle
    {
        Turtle() = default;
        explicit Turtle(const DrawingParameters& parameters);
        ~Turtle() = default;
        Turtle(const Turtle& other) = default;
        Turtle(Turtle&& other) = default;
        Turtle& operator=(const Turtle& other) = default;
        Turtle& operator=(Turtle&& other) = default;

        // Cosine and sine of the 'delta_angle'. Computed once to speed up
        // calculations.
        double cos_ {0};
        double sin_ {1};
        // The absolute step. The visuals are scaled on screen with the
        // DrawingParameters 'scale_'.
        // The value is 1 as floating-point are really precise at this scale.
        static constexpr double step_ {1};

        // The current position and direction of the Turtle.
        struct State {
            sf::Vector2<double> position;
            sf::Vector2<double> direction;
        };
        State state_ { {0, 0}, // (0, 0) as the position on-screen is set in
                               // LSystemView with transforms.
                       {0, 1} };

        // The state of a turtle can be saved and loaded in a stack.
        std::stack<State> stack_ { };


        // Reinit all the previous members from 'parameters'
        void init_from_parameters (const DrawingParameters& parameters);


        // The result of 'compute_vertices()'.
        // They all are references to private members, to avoid unecessary
        // copies. Please be careful of the lifetime of 'Turtle'.
        struct TurtleProduction
        {
            std::vector<sf::Vertex>& vertices;     // The vertices
            const std::vector<u8>& iterations;     // Their iteration depth
            const std::vector<bool>& transparency; // If their are invisible
        };
        // Compute all vertices, their iteration depth and their transparency of
        // a turtle interpretation of a L-system.
        // For each symbol in 'lsystem_production', this function interpets it
        // with the orders from 'interpretation'.
        // 'size' is an optional argument that reserves 'size' elements in the
        // result vectors before starting computation. If the value is equals or
        // larger than the size of the vectors, no reallocation will take place,
        // reducing the time this function takes to execute.
        TurtleProduction compute_vertices(const std::string& lsystem_production,
                                          const std::vector<u8>& lsystem_iterations,
                                          const InterpretationMap& interpretation,
                                          unsigned long long size = 0);

        // The vertices computed by 'compute_vertices()'
        std::vector<sf::Vertex> vertices_ { };

        // The iterations associated to each vertex computed by
        // 'compute_vertices()'.
        // Each symbol in the 'lsystem_production' have an iteration depth in
        // 'lsystem_iterations'. However, a symbol can have none, one, or
        // several vertices associated, so this member links the vertices with
        // the iteration depth of the associated symbol.
        std::vector<u8> iterations_ {};

        // Defines for each vertex in 'vertices_' if it must be invisible to the
        // user.
        // A big optimization is to do only one draw call with a LineStrip. To
        // do so, there are invisible vertices that link the different branches
        // when loading or saving a state. These vertices must stay transparent,
        // even when coloring all the other, so there is this vector of flags.
        std::vector<bool> transparency_ {};

        // The current depth at 'iteration_index_'
        u8 iteration_depth_ {0};

    private:
        // Index indicating the position in 'iterations' from 'compute_vertices()'.
        std::size_t iteration_index_ {0};

    };
}


#endif // DRAWING_TURTLE_H
