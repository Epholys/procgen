#ifndef DRAWING_INTERPRETATION_H
#define DRAWING_INTERPRETATION_H

#include <functional>
#include <unordered_map>

#include "DrawingParameters.h"

// Main explanation of drawing in Turtle.h
namespace drawing
{
    // Forward declaration
    namespace impl
    {
        struct Turtle;
    }

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
}

#endif  // DRAWING_INTERPRETATION_H
