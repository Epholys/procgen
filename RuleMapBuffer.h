#ifndef RULE_MAP_BUFFER_H
#define RULE_MAP_BUFFER_H


#include <list>
#include <tuple>

#include "RuleMap.h"
#include "Observer.h"
#include "LSystem.h"
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
    //   - synchronization between 'RuleMapBuffer':
    //   Several object can refer to the same Target (e.g. LSystem). A
    // synchronization is necessary between them and implemented with the
    // 'Observer<>' class. However, scratch buffers are unique to each
    // LSystemBuffer, and must be managed separately.
    // 
    //   - empty rules:
    //   If a rule does not have a predecessor (a null character), it is
    // considered as a scratch buffer. They are not synchronized with the
    // Target or others 'RuleMapBuffer'.
    // 
    //   - duplication:
    //   Several rules can be duplicated. If one of these rule is removed, the
    // other one must take its place and be synchronized, even if the removed
    // rule and its replacement are in different 'RuleMapBuffer'.
    // 
    //   - iterator invalidation:
    //   To access the buffer in the GUI, a 'const_iterator' is
    // implemented. Some operations invalidate the iterator, so a set of delayed
    // functions buffers the operation to apply it when it is safe.
    //
    // We can still directly access the Target for attributes others than the
    // main rule_map.
    //
    // Invariant:
    //   - The Target and the buffer must be synchronized
    template<typename Target>
    class RuleMapBuffer : public Observer<Target>
    {
    public:
        /* must be a derived class of RuleMap */
        static_assert(std::is_base_of<RuleMap<typename Target::successor>, Target>::value, "RuleMapBuffer must refer to a derived class of RuleMap");

        using target = Target;
        using succ = typename Target::successor;

        // A production rule: 
        struct Rule
        {
            bool validity {true};    // If a rule is a duplicate of an already existing
                              // rule, it is not valid.
            char predecessor {'\0'};
            succ successor {}; // If the successor is a basic type like 'int',
                               // it will be not be initialized. Do not forget
                               // to override 'add_rule' if this is the case.
            inline bool operator== (const Rule& other) const
                { return validity == other.validity &&
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
        RuleMapBuffer(const std::shared_ptr<Target>& target_);

        // The copy constructor and the assignment operator are needed because
        // we need to register a callback for the new object to
        // 'Observer<Target>'
        RuleMapBuffer(const RuleMapBuffer& other);
        RuleMapBuffer& operator=(const RuleMapBuffer& other);
        
        // Get the Target. Its modification will be automatically synchronized
        // with the Observer pattern.
        Target& get_target() const;

        // Access the underlying buffer without allowing modifications. Every
        // modification should be done with the associated functions.
        const_iterator begin() const;
        const_iterator end() const;

        // Get the number of rules.
        size_t size() const;

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
        //  - Precondition: 'cit' must be valid and derenferenceable.
        void remove_predecessor(const_iterator cit);

        // Update the successor of the rule at 'cit' to 'succ'.
        // If the rule is valid, update the Target.
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and derenferenceable.
        void change_successor(const_iterator cit, const succ& succ);

        // These methods buffer to 'instruction_' the associated method.
        // It is used to prevent iterator invalidation.
        void delayed_add_rule();
        void delayed_erase(const_iterator cit);
        void delayed_change_predecessor(const_iterator cit, char pred);
        void delayed_remove_predecessor(const_iterator cit);
        void delayed_change_successor(const_iterator cit, const succ& succ);

        // Apply the buffered instruction.
        // If 'instruction_' is nullptr, does nothing.
        void apply();
        
    private:
        // Remove a rule from the buffer.
        // Take care of updating the Target according to the existence of
        // duplicate rules in this buffer.
        void remove_rule(char pred);
        
        // Synchronize the rule buffer with the LSystems
        void sync();
       
        // Remove the constness from 'cit'. Can only be used inside this class
        // to modify 'buffer_'.
        iterator remove_const(const_iterator cit);

        // The Target: ease of use instead of calling
        // 'Observer<Target>::target_' each time.
        Target& target_;

        // The rule buffer.
        buffer buffer_;

        // The buffered instruction from the 'delayed_*' methods.
        std::function<void()> instruction_;
    };

    #include "RuleMapBuffer.tpp"
}

#endif // RULE_MAP_BUFFER_H
