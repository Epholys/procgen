#ifndef RULE_MAP_BUFFER_H
#define RULE_MAP_BUFFER_H


// TODO remove
#include <iostream>

#include <list>

#include "Observer.h"
#include "RuleMap.h"
#include "helper_algorithm.h"

namespace procgui
{
    // The interface between the pure logic of a RuleMap and the ImGui GUI.
    //
    // ImGui is an immediate-mode GUI, so it can not retain information between
    // frames. This class own a production rule buffer to manage:
    //   - duplicate rules
    //   - adding new rules
    //   - removing rules
    //
    // This is a sensitive class, as edge cases are the rules. We must
    // manage:
    //   - empty rules:
    //   If a rule does not have a predecessor (a null character), it is
    // considered as a scratch buffer. They are not synchronized with the
    // Target or others 'RuleMapBuffer'.
    //
    //   - duplication:
    //   Several rules can be duplicated. If one of these rule is removed, the
    // other one must take its place and be synchronized.
    //
    // We can still directly access the Target for attributes others than the
    // main rule_map.
    //
    // Invariant:
    //   - The Target and the buffer must be synchronized
    //
    // Note:
    //   - This is an Observable, but it simply foward the notification from its
    //   Observer, and does not notify from its internal modifications.
    template<typename Target>
    class RuleMapBuffer : public Observable
    {
    public:
        // must be a derived class of RuleMap
        static_assert(std::is_base_of<RuleMap<typename Target::Successor>, Target>::value, "RuleMapBuffer must refer to a derived class of RuleMap");

        using Successor = typename Target::Successor;

        // A production rule:
        struct Rule
        {
            bool is_active {true};    // If a Rule is a duplicate of an already existing
                                     // rule, it is not active.
            char predecessor {'\0'};
            Successor successor {}; // If the successor is a basic type like 'int',
                               // it will be not be initialized. Do not forget
                               // to override 'add_rule' if this is the case.
            inline bool operator== (const Rule& other) const
                { return is_active == other.is_active &&
                         predecessor == other.predecessor &&
                         successor == other.successor; }
            inline bool operator!= (const Rule& other) const
                { return !(*this == other); }
        };
        using buffer = std::list<Rule>;

        // The iterators come directly from the buffer.
        // Only 'const_iterator' is accessible from the outside.
        using iterator       = typename buffer::iterator;
        using const_iterator = typename buffer::const_iterator;

        // Constructor
        explicit RuleMapBuffer(std::shared_ptr<Target> rule_map_);
        virtual ~RuleMapBuffer();

        RuleMapBuffer(const RuleMapBuffer& other);
        RuleMapBuffer(RuleMapBuffer&& other);
        RuleMapBuffer& operator=(const RuleMapBuffer& other);
        RuleMapBuffer& operator=(RuleMapBuffer&& other);

        // Access the underlying buffer without allowing modifications. Every
        // modification should be done with the associated functions.
        const_iterator begin() const;
        const_iterator end() const;

        // Get the number of rules.
        size_t size() const;

        // Get target
        std::shared_ptr<const Target> get_rule_map() const;
        std::shared_ptr<Target> ref_rule_map() const;

        // Set target
        void set_rule_map(std::shared_ptr<Target> new_rule_map);

        // Add an empty rule: a scratch buffer.
        // Do not forget to override it if there is not a default initialization
        // for the successor.
        void add_rule();

        // Erase the rule at 'cit'.
        // Remove it from the Target if necessary.
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and derenferenceable.
        //
        // 'cit' is invalidated.
        void erase(const_iterator cit);

        // Change the predecessor of the rule at 'cit' to 'pred'.
        // If the 'pred' is null, calls 'remove_predecessor()'.
        // Otherwise, update the buffer and the Target if
        // necessary.
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and derenferenceable.
        //
        // 'cit' may be invalidated.
        void change_predecessor(const_iterator cit, char pred);

        // Change the rule at 'cit' into a scratch buffer.
        // If the rule is valid, remove it from the Target.
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and dereferenceable.
        void remove_predecessor(const_iterator cit);

        // Update the successor of the rule at 'cit' to 'succ'.
        // If the rule is valid, update the Target.
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and derenferenceable.
        void change_successor(const_iterator cit, const Successor& succ);

        // Reverse the last modification by setting all rules of the target by
        // the rules of 'previous_buffer_'.
        void revert();

        // Confirms the change, clear 'previous_buffer_'
        void validate();

    private:
        // Remove a rule from the buffer.
        // Take care of updating the Target according to the existence of
        // duplicate rules in this buffer.
        void remove_rule(char pred);

        // Remove the constness from 'cit'. Can only be used inside this class
        // to modify 'buffer_'.
        iterator remove_const(const_iterator cit);

        // Generate a rule map for the target.
        typename Target::Rules generate_rule_map();

        // The target
        Observer<Target> target_observer_{nullptr};

        // The rule buffer.
        buffer buffer_{};
        // The previous rule buffer. Used to reverse in 'revert()'.
        buffer previous_buffer_{};
    };

    #include "RuleMapBuffer.tpp"
}

#endif // RULE_MAP_BUFFER_H
