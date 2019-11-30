#ifndef DRAWING_INTERPRETATION_H
#define DRAWING_INTERPRETATION_H

#include <functional>

#include "cereal/cereal.hpp"
#include "cereal/types/unordered_map.hpp"

#include "DrawingParameters.h"
#include "RuleMap.h"
#include "helper_string.h"
#include "LoadMenu.h"

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

    void go_forward_fn(impl::Turtle& turtle);
    // "Turn right" means "turn clockwise" AS SEEN ON THE SCREEN.
    void turn_right_fn(impl::Turtle& turtle);
    void turn_left_fn(impl::Turtle& turtle);
    void save_position_fn(impl::Turtle& turtle);
    void load_position_fn(impl::Turtle& turtle);

    // An 'Order' is the association of an 'order_fn', an identifier to allow
    // equality comparison between orders (as 'std::function<>' does not have
    // it) and a name used in the GUI and the serialization.
    struct Order {
        Order() : Order(OrderID::GO_FORWARD,/*go_forward_fn,*/ "Go forward") {}
        Order(OrderID id, /*const order_fn& fn,*/ const std::string& name)
            : id{id},/* order{fn},*/ name{name} {}

        OrderID id;
//        order_fn order;
        std::string name;
//        void operator() (impl::Turtle& t) { order(t); }
    };
    inline bool operator== (const Order& lhs, const Order& rhs)
    {
        return lhs.id == rhs.id;
    }

    const Order go_forward    { OrderID::GO_FORWARD,    /*go_forward_fn,*/ "Go forward" };
    const Order turn_right    { OrderID::TURN_RIGHT,    /*turn_right_fn,*/ "Turn right" };
    const Order turn_left     { OrderID::TURN_LEFT,     /*turn_left_fn,*/ "Turn left"  };
    const Order save_position { OrderID::SAVE_POSITION, /*save_position_fn,*/ "Save position" };
    const Order load_position { OrderID::LOAD_POSITION, /*load_position_fn,*/ "Load position" };

    // All the orders available.
    const std::vector<Order> all_orders { go_forward, turn_right, turn_left,
                                          save_position, load_position };
    // All the names of the orders (used in the GUI).
    // Note: They must be in the same strict order as 'all_orders'.
    const std::vector<const char*> all_orders_name =
        [](){ std::vector<const char*> v;
              for(const auto& o : all_orders)
                  v.push_back(o.name.c_str());
              return v; }();

    const std::vector<std::string> all_orders_json_name =
        [](){ std::vector<std::string> v;
              for(const auto& o : all_orders)
                  v.push_back(to_camel_case(o.name));
              return v; }();



    // Minimal serialization for Orders: we save and load only the associated
    // name.
    template<class Archive>
    std::string save_minimal (Archive&, const Order& order)
    {
        return to_camel_case(order.name);
    }

    template<class Archive>
    void load_minimal (Archive&, Order& order, const std::string& str)
    {
        auto it = std::find_if(begin(all_orders_json_name), end(all_orders_json_name),
                               [str](const auto& o){return o == str;});

        int index = 0;
        if (it == end(all_orders_json_name))
        {
            controller::LoadMenu::add_loading_error_message("One InterpretationMap's order does not exist, it is now set to go_forward");
        }
        else
        {
            index = std::distance(begin(all_orders_json_name), it);
        }
        Expects(index >= 0);
        Expects(static_cast<decltype(all_orders)::size_type>(index) < all_orders.size());
        order = *(begin(all_orders) + index);
    }

    // 'InterpretationMap' is a map linking a symbol of the vocabulary of a
    // L-system to an order. During the interpretation, if the character is
    // encountered, the associated order will be executed.
    class InterpretationMap : public RuleMap<Order>
    {
    public:
        // Constructors simply redirecting to RuleMap<Order> constructors.
        InterpretationMap() = default;
        virtual ~InterpretationMap() = default;
        explicit InterpretationMap(const rule_map& rules);
        InterpretationMap(std::initializer_list<typename rule_map::value_type> init);
        InterpretationMap(const InterpretationMap& other) = default;
        InterpretationMap& operator=(const InterpretationMap& other) = default;
        InterpretationMap(InterpretationMap&& other) = default;
        InterpretationMap& operator=(InterpretationMap&& other) = default;

    private:
        // Serialization
        friend class cereal::access;

        template<class Archive>
        void save (Archive& ar, const u32) const
            {
                // Custom save to have a pretty map between predecessors and
                // orders.
                for(const auto& i : rules_)
                    ar(cereal::make_nvp(std::string()+i.first, i.second));
            }

        template<class Archive>
        void load (Archive& ar, const u32)
            {
                // Complex loading as we do not save the 'map' in a standard
                // way.
                rules_.clear();

                bool key_too_big = false;
                bool void_key = false;
                auto hint = rules_.begin();
                while(true)
                {
                    const auto namePtr = ar.getNodeName();

                    if(!namePtr)
                        break;

                    std::string loaded_key = namePtr;
                    Order value; ar(value);
                    char key = ' ';
                    if (loaded_key.size() != 0)
                    {
                        if (loaded_key.size() > 1)
                        {
                            key_too_big = true;
                        }
                        key = loaded_key.at(0);
                        hint = rules_.emplace_hint(hint, key, std::move(value));
                    }
                    else
                    {
                        void_key = true;
                    }
                }

                if (key_too_big)
                {
                    controller::LoadMenu::add_loading_error_message("One or more InterpretationMap's key was too big, it is now cropped.");
                }
                if (void_key)
                {
                    controller::LoadMenu::add_loading_error_message("One or more InterpretationMap's key was empty, so it was ignored.");
                }
            }
    };


    // The default interpretation map used when creating new LSystems.
    const InterpretationMap default_interpretation_map
    { { 'F', go_forward },
      { '-', turn_left  },
      { '+', turn_right },
      { '[', save_position },
      { ']', load_position } };
}

#endif  // DRAWING_INTERPRETATION_H
