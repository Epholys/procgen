#include "UniqueColor.h"
#include "UniqueId.h"

#include <gtest/gtest.h>

TEST(UniqueTest, new_id)
{
    UniqueId ids;
    auto id0 = ids.get_id();
    auto id1 = ids.get_id();
    auto id2 = ids.get_id();

    ASSERT_NE(id0, id1);
    ASSERT_NE(id0, id2);
    ASSERT_NE(id1, id2);
}

TEST(UniqueTest, replace_id)
{
    UniqueId ids;
    auto UNUSED1 = ids.get_id();
    auto id1 = ids.get_id();
    auto UNUSED = ids.get_id();

    ids.free_id(id1);
    auto id1bis = ids.get_id();
    ASSERT_EQ(id1, id1bis);
}

TEST(UniqueTest, colors)
{
    UniqueId ids;
    colors::UniqueColor colors;

    auto id0 = ids.get_id();
    auto id1 = ids.get_id();
    auto id2 = ids.get_id();

    auto col0 = colors.get_color(id0);
    auto col1 = colors.get_color(id1);
    auto col2 = colors.get_color(id2);

    ASSERT_NE(col0, col1);
    ASSERT_NE(col0, col2);
    ASSERT_NE(col1, col2);
}
