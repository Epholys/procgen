#include <gtest/gtest.h>
#include "Indicator.h"

class Foo : public Indicator
{
public:
    void modifying_operation()
        {
            indicate_modification();
        }
};

TEST(IndicatorTest, default_ctor_not_modified)
{
    Foo indicator;
    ASSERT_FALSE(indicator.poll_modification());
}

TEST(IndicatorTest, poll_modification)
{
    Foo indicator;

    indicator.modifying_operation();
    ASSERT_TRUE(indicator.poll_modification());
    ASSERT_FALSE(indicator.poll_modification());
}

TEST(IndicatorTest, copy_ctor_modification_propagation)
{
    Foo indicator;
    indicator.modifying_operation();

    Foo indicator_copy (indicator);
    ASSERT_TRUE(indicator_copy.poll_modification());
}

TEST(IndicatorTest, copy_assignment_modification_propagation)
{
    Foo indicator;
    indicator.modifying_operation();

    Foo indicator_copy;
    indicator_copy = indicator;
    ASSERT_TRUE(indicator_copy.poll_modification());
}

TEST(IndicatorTest, move_ctor_modification_propagation)
{
    Foo indicator;
    indicator.modifying_operation();

    Foo indicator_moved (std::move(indicator));
    ASSERT_TRUE(indicator_moved.poll_modification());
}

TEST(IndicatorTest, move_assignement_modification_propagation)
{
    Foo indicator;
    indicator.modifying_operation();

    Foo indicator_moved;
    indicator_moved = std::move(indicator);
    ASSERT_TRUE(indicator_moved.poll_modification());
}
