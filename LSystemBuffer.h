#ifndef LSYSTEM_VIEW_H
#define LSYSTEM_VIEW_H


#include <list>
#include <tuple>

#include "LSystem.h"
#include "Observer.h"
#include "helper_string.h"

namespace procgui
{
    // ImGui needs a number for its fixed-size input buffer.
    // We arbitrarily choose 64 characters as the limit for the sucessor of a
    // production rule.
    constexpr int lsys_successor_size = 64;

    // The interface between the pure logic of a LSystem and the ImGui GUI.
    //
    // ImGui is a immediate-mode GUI, so it can not retain information between
    // frame. This class hosts a production rules buffer used in
    // 'procgui::interact_with()' to manage:
    //   - duplicate rules
    //   - adding new rules
    //   - removing rules
    // We can still directly access the LSystem for all trivial attributes like
    // the axiom.
    //
    // A LSystemBuffer must be destructed before the destruction of its LSystem
    // (non owning-reference). For the coherence of the GUI, a LSystem must have
    // a unique LSystemBuffer associated.
    //
    // Note: This class has a lot in common with 'IntepretationMapBuffer'. If a
    // third class has the same properties, all will be refactorized.
    class LSystemBuffer : public Observer<LSystem>
    {
    public:
        using lsys_ = Observer<LSystem>;

        using validity    = bool; // If the rule is a duplicate, it is not valid.
        using predecessor = char;
        using successor   = std::string;
        using entry       = std::tuple<validity, predecessor, successor>;
        using const_iterator = std::list<entry>::const_iterator;

        LSystemBuffer(const std::shared_ptr<LSystem>& lsys);

        LSystem& get_lsys() const;
        const_iterator begin() const;
        const_iterator end() const;

        void add_rule();
        void erase(const_iterator cit);
        void change_predecessor(const_iterator cit, bool valid, predecessor pred);
        void remove_predecessor(const_iterator cit);
        void change_successor(const_iterator cit, successor succ);

        void delayed_add_rule();
        void delayed_erase(const_iterator cit);
        void delayed_change_predecessor(const_iterator cit, bool valid, predecessor pred);
        void delayed_remove_predecessor(const_iterator cit);
        void delayed_change_successor(const_iterator cit, successor succ);
        void apply();
        
        // Synchronize the rule buffer with the LSystem.
        void sync();

    private:
        std::list<entry> buffer_;
        std::function<void()> instruction_;
        // bool lock_;
    };
}

#endif // LSYSTEM_VIEW_H
