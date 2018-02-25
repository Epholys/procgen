#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "helper_algorithm.h"

TEST(helper_algorithm_test, find_duplicate_it)
{
    std::vector<int> v { 1, 2, 3, 1 };

    auto first  = v.begin();
    auto second = std::next(first);
    auto last   = v.end();
    auto penultimate = std::prev(last);

    ASSERT_TRUE(find_duplicate(first, first, last) == penultimate);
    ASSERT_TRUE(find_duplicate(first, first, penultimate) == penultimate);
    ASSERT_TRUE(find_duplicate(second, first, last) == last);
}

TEST(helper_algorithm_test, find_duplicate_it_pred)
{
    std::vector<std::string> v { "AA", "BB", "CC", "AB" };

    auto predicate =
        [](const auto& s1, const auto& s2)
        {
            return s1.at(0) == s2.at(0);
        };
    
    auto first  = v.begin();
    auto second = std::next(first);
    auto last   = v.end();
    auto penultimate = std::prev(last);

    ASSERT_TRUE(find_duplicate(first, first, last, predicate) == penultimate);
    ASSERT_TRUE(find_duplicate(first, first, penultimate, predicate) == penultimate);
    ASSERT_TRUE(find_duplicate(second, first, last, predicate) == last);
}
