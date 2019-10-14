#include <gtest/gtest.h>
#include "size_computer.h"
#include "LSystem.h"
#include "InterpretationMap.h"

using namespace drawing;

class size_computer_test :  public ::testing::Test
{
public:
    size_computer_test()
        {
        }

    // TODO EXPLAIN CHOICES (x, y, ...)
    const LSystem lsys{"F", {{'F', "G+F[+G]x"}, {'G', "F-G-F"}, {'x', ""}}, "FG"};

    const InterpretationMap map{{'F', go_forward},
                                {'G', go_forward},
                                {'-', turn_left},
                                {'+', turn_right},
                                {'[', save_position},
                                {']', load_position},
                                {'y', go_forward}};
};

TEST_F(size_computer_test, MatrixCtorFullEmpty)
{
    std::vector<std::vector<int>> data {};

    ASSERT_ANY_THROW(Matrix fail (data));
}
TEST_F(size_computer_test, MatrixCtorSecondDimEmpty)
{
    std::vector<std::vector<int>> data{{0}, {}, {1}};

    ASSERT_ANY_THROW(Matrix fail(data));
}
TEST_F(size_computer_test, MatrixCtorMismatchedDims)
{
    std::vector<std::vector<int>> data{{1, 2}, {4, 5, 6}, {7, 8}};

    ASSERT_ANY_THROW(Matrix fail(data));
}

// https://www.wolframalpha.com/input/?i=%7B%7B1%2C+2%2C+3%7D%2C%7B3%2C+2%2C+1%7D%2C%7B1%2C+2%2C+3%7D%7D*%7B%7B1%2C+2%7D%2C%7B3%2C4%7D%2C%7B5%2C6%7D%7D
TEST_F(size_computer_test, MatrixMult)
{
    std::vector<std::vector<int>> data1{{1, 2, 3}, {3, 2, 1}, {1, 2, 3}};
    std::vector<std::vector<int>> data2{{1, 2}, {3, 4}, {5, 6}};
    Matrix mat1(data1);
    Matrix mat2(data2);
    Matrix result = mat1 * mat2;
    
    std::vector<std::vector<int>> expected_result{{22, 28}, {14, 20}, {22, 28}};

    ASSERT_EQ(expected_result, result.get_data());
}

TEST_F(size_computer_test, MatrixGrandSum)
{
    // TODO EXCEPTION

    ASSERT_TRUE(false);
}

TEST_F(size_computer_test, all_predecessors)
{
    // TODO LSYS MAP
    // GET ALL STRINGS
    // SORT

    ASSERT_TRUE(false);
}
TEST_F(size_computer_test, lsys_matrix)
{
    // PREDS:  F G x + - [ ] y        // TODO SORT
    // AX: [1 0 0 0 0 0 0]            // UPDATE W/ SORT
    // RULES: [[1 2 1 2 0 1 1 0]  : F // UPDATE W/ SORT
    //         [2 1 0 0 2 0 0 0]  : G
    //         [0 0 0 0 0 0 0 0]  : x
    //         [0 0 0 1 0 0 0 0]  : +
    //         [0 0 0 0 1 0 0 0]  : -
    //         [0 0 0 0 0 1 0 0]  : [
    //         [0 0 0 0 0 0 1 0]  : ]
    //         [0 0 0 0 0 0 0 1]] : y
    // CHECK ^^^THIS^^^ MATRIX AS EXPECTED
    
    ASSERT_TRUE(false);
}
TEST_F(size_computer_test, map_matrix)
{
    // PREDS:  F G x + - [ ] y        // TODO SORT
    // [[1]  : F                      // UPDATE W/ SORT
    //  [1]  : G
    //  [0]  : x
    //  [0]  : +
    //  [0]  : -
    //  [0]  : [
    //  [3]  : ]
    //  [1]] : y
    // CHECK ^^^THIS^^^ MATRIX AS EXPECTED    

    ASSERT_TRUE(false);
}
TEST_F(size_computer_test, compute_size)
{
  const LSystem lsys("F", {{'F', "G+F[+G]x"}, {'G', "F-G-F"}, {'x', ""}}, "FG");

  const InterpretationMap map{{'F', go_forward},    {'G', go_forward},
                              {'-', turn_left},     {'+', turn_right},
                              {'[', save_position}, {']', load_position},
                              {'y', go_forward}};
  constexpr int n_iter = 7;

  auto sizes = compute_size(lsys, map, n_iter);

 // Computed directely from the application
  
  constexpr int expected_lsys_size = 5832;
  constexpr int expected_vx_size = 3829;
  ASSERT_EQ(sizes.lsystem_size, expected_lsys_size);
  ASSERT_EQ(sizes.vertices_size, expected_vx_size);
}
