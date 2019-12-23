#include <gtest/gtest.h>
#include "cereal/archives/json.hpp"

#include "LSystemView.h"
#include "VertexPainterLinear.h"

using namespace procgui;
using namespace drawing;
using namespace colors;

// TODO use test fixture instead of parameters_example

// This class is very visual, so it is difficult to test. However, one part is
// testable : constructors.
// There is very little getters in LSystemView and RuleMapBuffer<>, so accessing
// attribute to check correct copy/move behavior and correct notification
// behavior is impossible. It could be argued that adding these getters with
// careful use of 'const' could do not harm and allow further testing. However,
// adding these methods without any other use case may cause minimal bloat now,
// but much more in the future. As such, these getters will not be implemented
// for now.

// Constructors are important as they implement a deep-copy, and with the
// pointer-fest it is important to test them. There's also little things like
// selection, colors, and unique identifers that must be tested.

struct parameters_example
{
    std::string name {"Testing"};
    std::shared_ptr<LSystem> lsys = std::make_shared<LSystem>("X",
                                                              LSystem::Rules({{'F', "FF"}, {'X', "F[+X][-X]"}}),
                                                              "X");
    std::shared_ptr<InterpretationMap> map = std::make_shared<InterpretationMap>(default_interpretation_map);
    DrawingParameters params {{10,10}, 1, 1, 5, 3};
    std::shared_ptr<VertexPainterWrapper> painter =
        std::make_shared<VertexPainterWrapper>(
            std::make_shared<VertexPainterLinear>(
                std::make_shared<ColorGeneratorWrapper>(
                    std::make_shared<LinearGradient>(LinearGradient::keys({{sf::Color::Red, 0.}, {sf::Color::Blue, 1.0}})))));
};

TEST(LSystemView, copy_ctor)
{
    parameters_example params;
    LSystemView copied_view (params.name, params.lsys, params.map, params.params, params.painter);
    copied_view.select();
    LSystemView copy_view (copied_view);

    // Does not work: only the 'RuleMapBuffer<>' is fetched and it is is not
    // possible to check the inequality of their 'RuleMap<>' AND their coherence
    // with the 'Observer<>''s pointer. Adding more code as getters may
    // introduce less encapsulation later.
    // ASSERT_NE(&copied_view.get_lsystem_buffer(), &copy_view.get_lsystem_buffer());
    // ASSERT_NE(&copied_view.get_interpretation_buffer(), &copy_view.get_interpretation_buffer());
    ASSERT_NE(&(*copied_view.get_vertex_painter_wrapper().unwrap()),
              &(*copy_view.get_vertex_painter_wrapper().unwrap()));
    ASSERT_NE(copied_view.get_id(), copy_view.get_id());
    ASSERT_NE(copied_view.get_color(), copy_view.get_color());
    ASSERT_FALSE(copy_view.is_selected());
}

TEST(LSystemView, copy_assignment_ctor)
{
    parameters_example params;
    LSystemView assigned_view (params.name, params.lsys, params.map, params.params, params.painter);
    assigned_view.select();
    LSystemView assign_view ({100, 100}, 5);
    assign_view = assigned_view;

    // Does not work: only the 'RuleMapBuffer<>' is fetched and it is is not
    // possible to check the inequality of their 'RuleMap<>' AND their coherence
    // with the 'Observer<>''s pointer. Adding more code as getters may
    // introduce less encapsulation later.
    // ASSERT_NE(&copied_view.get_lsystem_buffer(), &copy_view.get_lsystem_buffer());
    // ASSERT_NE(&copied_view.get_interpretation_buffer(), &copy_view.get_interpretation_buffer());
    ASSERT_NE(&(*assigned_view.get_vertex_painter_wrapper().unwrap()),
              &(*assign_view.get_vertex_painter_wrapper().unwrap()));
    ASSERT_NE(assigned_view.get_id(), assign_view.get_id());
    ASSERT_NE(assigned_view.get_color(), assign_view.get_color());
    ASSERT_FALSE(assign_view.is_selected());
}

TEST(LSystemView, move_ctor)
{
    parameters_example params;
    LSystemView moved_view (params.name, params.lsys, params.map, params.params, params.painter);
    moved_view.select();
    auto id = moved_view.get_id();
    auto color = moved_view.get_color();
    LSystemView move_view (std::move(moved_view));

    // Does not work: only the 'RuleMapBuffer<>' is fetched and it is is not
    // possible to check the equality of their 'RuleMap<>' AND their coherence
    // with the 'Observer<>''s pointer. Adding more code as getters may
    // introduce less encapsulation later.
    ASSERT_EQ(&(*move_view.get_vertex_painter_wrapper().unwrap()),
              &(*params.painter->unwrap()));
    ASSERT_EQ(move_view.get_id(), id);
    ASSERT_EQ(move_view.get_color(), color);
    ASSERT_FALSE(move_view.is_selected());
}

TEST(LSystemView, assign_move_ctor)
{
    parameters_example params;
    LSystemView moved_view (params.name, params.lsys, params.map, params.params, params.painter);
    moved_view.select();
    auto id = moved_view.get_id();
    auto color = moved_view.get_color();
    LSystemView move_view ({100, 100}, 5);
    move_view = std::move(moved_view);

    // Does not work: only the 'RuleMapBuffer<>' is fetched and it is is not
    // possible to check the equality of their 'RuleMap<>' AND their coherence
    // with the 'Observer<>''s pointer. Adding more code as getters may
    // introduce less encapsulation later.
    ASSERT_EQ(&(*move_view.get_vertex_painter_wrapper().unwrap()),
              &(*params.painter->unwrap()));
    ASSERT_EQ(move_view.get_id(), id);
    ASSERT_EQ(move_view.get_color(), color);
    ASSERT_FALSE(move_view.is_selected());
}
