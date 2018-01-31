#ifndef TURTLE_H
#define TURTLE_H


#include <vector>
#include <string>
#include <cmath>
#include <functional>
#include <stack>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "helper_math.h"
#include "helper_string.h"
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
            struct State {
                sf::Vector2f position { 0, 0 };
                float angle { 0 };
            };
            State state { };

            // The state of a turtle can be saved and loaded in a stack.
            std::stack<State> stack { };
            
            // Each time the Turtle changes its position, the new one is saved
            // in a vertex. Moreover, as we can jump from position to position,
            // we must save the different continuous paths separately, to
            // correctly display the whole trajectory of the turtle. 'paths' is
            // this data structure.
            std::vector<std::vector<sf::Vertex>> paths { };
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

    // An 'order_fn' is a function modifying a 'Turtle'. Semantically it is an
    // instruction like "move forward" or "turn left".
    using order_fn = std::function<void(impl::Turtle& turtle)>;

    // All the orders currently defined.
    enum class OrderID {
        GO_FORWARD,
        TURN_RIGHT,
        TURN_LEFT,
        SAVE_POSITION,
        LOAD_POSITION,
    };

    // An 'Order' is the association of an 'order_fn' and an identifier to allow
    // equality comparison between orders, as 'std::function<>' does not have
    // it.
    struct Order {
        OrderID id;
        order_fn order;
        void operator() (impl::Turtle& t) { order(t); }
    };
    inline bool operator== (const Order& lhs, const Order& rhs)
    {
        return lhs.id == rhs.id;
    }
    
    void go_forward_fn(impl::Turtle& turtle);
    // "Turn right" means "turn clockwise" AS SEEN ON THE SCREEN.
    void turn_right_fn(impl::Turtle& turtle);
    void turn_left_fn(impl::Turtle& turtle);
    void save_position_fn(impl::Turtle& turtle);
    void load_position_fn(impl::Turtle& turtle);
    
    const Order go_forward    { OrderID::GO_FORWARD,    go_forward_fn };
    const Order turn_right    { OrderID::TURN_RIGHT,    turn_right_fn };
    const Order turn_left     { OrderID::TURN_LEFT,     turn_left_fn  };
    const Order save_position { OrderID::SAVE_POSITION, save_position_fn };
    const Order load_position { OrderID::LOAD_POSITION, load_position_fn };
        
    
    // WARNING: if new orders are added, do not forget to complete the order
    // database in 'InterpretationMapBuffer.h'. These informations are in two
    // files due to separation of concerns: the database is specific to the GUI.

    // 'InterpretationMap' is a map linking a symbol of the vocabulary of a
    // L-system to an order. During the interpretation, if the character is
    // encountered, the associated order will be executed.
    using InterpretationMap = std::unordered_map<char, Order>;
    
    // Compute all paths of a turtle interpretation of a L-system.
    // First, this function iterates 'parameters.n_iter' times the LSystem
    // 'lsys', using and modifying its cache. Then, it interprates the result
    // with 'interpretation' and 'parameters'.
    std::vector<std::vector<sf::Vertex>> compute_path(lsys::LSystem& lsys,
                                                      InterpretationMap& interpretation,
                                                      const DrawingParameters& parameters);
}

#endif
