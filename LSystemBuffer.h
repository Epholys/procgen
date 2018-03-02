#ifndef LSYSTEM_VIEW_H
#define LSYSTEM_VIEW_H


#include <list>
#include <tuple>

#include "LSystem.h"
#include "Observer.h"
#include "helper_algorithm.h"

namespace procgui
{
    // ImGui needs a number for its fixed-size input buffer.
    // We arbitrarily choose 64 characters as the limit for the sucessor of a
    // production rule.
    constexpr int lsys_successor_size = 64;

    // The interface between the pure logic of a LSystem and the ImGui GUI.
    //
    // ImGui is an immediate-mode GUI, so it can not retain information between
    // frames. This class own a production rule buffer to manage:
    //   - duplicate rules
    //   - adding new rules
    //   - removing rules
    //
    // This is a sensitive class, as edge cases are the rules. We must
    // manage:
    //   - synchronization between 'LSystemBuffer':
    //   Several object can refer to the same 'LSystem'. A synchronization is
    // necessary between them and implemented with the 'Observer<>'
    // class. However, scratch buffers are unique to each LSystemBuffer, and
    // must be managed separately.
    // 
    //   - empty rules:
    //   If a rule does not have a predecessor (a null character), it is
    // considered as a scratch buffer. They are not synchronized with the
    // 'LSystem' or others 'LSystemBuffer'.
    // 
    //   - duplication:
    //   Several rules can be duplicated. If one of these rule is removed, the
    // other one must take its place and be synchronized, even if the removed
    // rule and its replacement are in different 'LSystemBuffer'
    // 
    //   - iterator invalidation:
    //   To access the buffer in the GUI, a 'const_iterator' is
    // implemented. Some operations invalidate the iterator, so a set of delayed
    // functions buffers the operation to apply it when it is safe.
    //
    // We can still directly access the 'LSystem' for all trivial attributes
    // like the axiom.
    //
    // A 'LSystemBuffer' must be destructed before the destruction of its
    // LSystem (non owning-reference). For the coherence of the GUI, a LSystem
    // must have a unique LSystemBuffer associated.
    //
    // Note: This class has a lot in common with 'IntepretationMapBuffer'. If a
    // third class has the same properties, all will be refactorized.
    class LSystemBuffer : public Observer<LSystem>
    {
    public:
        using succ = std::string;

        struct Rule
        {
            bool validity;
            char predecessor;
            succ successor;
            inline bool operator== (const Rule& other) const
                { return validity == other.validity &&
                         predecessor == other.predecessor &&
                         successor == other.successor; }
            inline bool operator!= (const Rule& other) const
                { return !(*this == other); }
        };
        using buffer      = std::list<Rule>;

        // The iterators come directly from the buffer.
        // Only 'const_iterator' is accessible from the outside.
        using iterator       = buffer::iterator;
        using const_iterator = buffer::const_iterator;

        // Constructor
        LSystemBuffer(const std::shared_ptr<LSystem>& lsys);

        // Get the LSystem. Its modification will be automatically synchronized
        // with the Observer pattern.
        LSystem& get_lsys() const;

        // Access the underlying buffer without allowing modifications. Every
        // modification should be done with the associated functions.
        const_iterator begin() const;
        const_iterator end() const;

        // Add an empty rule: a scratch buffer.
        void add_rule();

        // Erase the rule at 'cit'.
        // If the rule is not a scratch buffer and is valid, remove it from the
        // LSystem.
        // Otherwise, delete it from the buffer.
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and derenferenceable.
        //
        // 'cit' is invalidated.
        void erase(const_iterator cit);

        // Change the predecessor of the rule at 'cit' to 'pred'.
        // If the 'pred' is null, calls 'remove_predecessor()'.
        // If the 'pred' is valid, remove the old rule and add a new one.
        // one.
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and derenferenceable.
        //
        // 'cit' is invalidated if 'pred' is not a duplicate.
        void change_predecessor(const_iterator cit, char pred);

        // Change the rule at 'cit' into a scratch buffer.
        // If the rule is valid, remove it from the 'LSystem'
        //
        // Exception:
        //  - Precondition: 'cit' must be valid and derenferenceable.
        void remove_predecessor(const_iterator cit);

        // Update the successor of the rule at 'cit' to 'succ'.
        // If the rule is valid, update the LSystem.
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
        void remove_rule(char pred);
        
        // Synchronize the rule buffer with the LSystem.
        void sync();
       
        // const_iterator find_duplicate(const_iterator cit);

        // Check if the rule at 'cit' has a duplicate in the buffer.
        bool has_duplicate(const_iterator cit);

        // Find an existing rule with the predicate 'pred'.
        // If such a rule does not exists, returns 'buffer_.cend()';
        const_iterator find_existing(char pred);

        // Remove the constness from 'cit'. Can only be used inside this class
        // to modify 'buffer_'.
        iterator remove_const(const_iterator cit);

        LSystem& lsys_;

// The rule buffer.
        buffer buffer_;

        // The buffered instruction from the 'delayed_*' methods.
        std::function<void()> instruction_;
    };
}

#endif // LSYSTEM_VIEW_H
