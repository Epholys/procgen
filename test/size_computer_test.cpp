#include <gtest/gtest.h>
#include "size_computer.h"
#include "LSystem.h"
#include "InterpretationMap.h"
#include "Turtle.h"

using namespace drawing;

class size_computer_test :  public ::testing::Test
{
public:
    size_computer_test()
        {
        }

    // 'lsys' and 'map' are designed to cover the maximum of use cases:
    // - no single character axiom
    // - classic rules (F, G), default rule (+, -, [, ], y), and empty rule (x)
    // - predecessors only defined in 'map' rules (y), only defined in 'lsys'
    // rules (x), or only defined in 'lsys' axiom (z).
    const LSystem lsys{"Fz", {{'F', "G+F[+G]x"}, {'G', "F-G-F"}, {'x', ""}}, "FG"};

    const InterpretationMap map{{'F', go_forward},
                                {'G', go_forward},
                                {'-', turn_left},
                                {'+', turn_right},
                                {'[', save_position},
                                {']', load_position},
                                {'y', go_forward}};

    const std::string expected_predecessors = "+-FG[]xyz";
};

TEST_F(size_computer_test, MatrixDataCtor)
{
    std::vector<std::vector<int>> data{{1, 2}, {3, 4}};
    Matrix mat (data);
    
    ASSERT_EQ(data, mat.get_data());
}
TEST_F(size_computer_test, Matrix0Ctor)
{
    std::vector<std::vector<int>> data{{0, 0}, {0, 0}, {0, 0}};
    Matrix mat(3, 2);
    
    ASSERT_EQ(data, mat.get_data());
}
TEST_F(size_computer_test, MatrixCtorKOFullEmpty)
{
    std::vector<std::vector<int>> data {};

    ASSERT_ANY_THROW(Matrix fail (data));
}
TEST_F(size_computer_test, MatrixCtorKOSecondDimEmpty)
{
    std::vector<std::vector<int>> data{{0}, {}, {1}};

    ASSERT_ANY_THROW(Matrix fail(data));
}
TEST_F(size_computer_test, MatrixCtorKOMismatchedDims)
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

TEST_F(size_computer_test, MatrixMultKOMismatchedDim)
{
    std::vector<std::vector<int>> data1{{1, 2, 3}, {3, 2, 1}};
    std::vector<std::vector<int>> data2{{1, 2}, {3, 4}, {5, 6}, {7, 8}};
    Matrix mat1(data1);
    Matrix mat2(data2);

    ASSERT_ANY_THROW(Matrix fail = mat1 * mat2);
}

TEST_F(size_computer_test, MatrixGrandSum)
{
    std::vector<std::vector<int>> data{{1, 2}, {3, 4}, {5, 6}};
    Matrix mat (data);

    constexpr int expected_sum = 1 + 2 + 3 + 4 + 5 + 6;

    ASSERT_EQ(expected_sum, mat.grand_sum());
}

TEST_F(size_computer_test, all_predecessors)
{
    const std::string predecessors = all_predecessors(lsys, map);
    
    ASSERT_EQ(expected_predecessors, predecessors);
}
TEST_F(size_computer_test, lsys_rules_matrix)
{
    /// const LSystem lsys{"Fz", {{'F', "G+F[+G]x"}, {'G', "F-G-F"}, {'x', ""}}, "FG"};
    // PREDS:  + - F G [ ] x y z               (y is in the InterpretationMap)
    // AX: [0 0 1 0 0 0 1]
    // RULES: [[1 0 0 0 0 0 0 0 0]  : + -> +
    //         [0 1 0 0 0 0 0 0 0]  : - -> -
    //         [2 0 1 2 1 1 1 0 0]  : F -> G+F[+G]x
    //         [0 2 2 1 0 0 0 0 0]  : G -> F-G-F
    //         [0 0 0 0 1 0 0 0 0]  : [ -> [
    //         [0 0 0 0 0 1 0 0 0]  : ] -> ]
    //         [0 0 0 0 0 0 0 0 0]  : x ->
    //         [0 0 0 0 0 0 0 1 0]  : y -> y
    //         [0 0 0 0 0 0 0 0 1]] : z -> z
    const std::vector<std::vector<int>> expected_data =
    { {1, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 0, 0, 0, 0, 0, 0},
      {2, 0, 1, 2, 1, 1, 1, 0, 0},
      {0, 2, 2, 1, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 1, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 1} };

    const Matrix computed_matrix = lsys_rules_matrix(lsys, expected_predecessors);

    ASSERT_EQ(expected_data, computed_matrix.get_data());
}
TEST_F(size_computer_test, lsys_rules_matrixKOnoSorted)
{
    const std::string wrong_predecessors = "-+GF][zyx";

    ASSERT_ANY_THROW(lsys_rules_matrix(lsys, wrong_predecessors));
}
TEST_F(size_computer_test, lsys_axiom_matrix)
{
    /// const LSystem lsys{"Fz", {{'F', "G+F[+G]x"}, {'G', "F-G-F"}, {'x', ""}}, "FG"};
    // PREDS:  + - F G [ ] x y z               (y is in the InterpretationMap)
    // AX: [0 0 1 0 0 0 1]
    // RULES: [[1 0 0 0 0 0 0 0 0]  : + -> +
    //         [0 1 0 0 0 0 0 0 0]  : - -> -
    //         [2 0 1 2 1 1 1 0 0]  : F -> G+F[+G]x
    //         [0 2 2 1 0 0 0 0 0]  : G -> F-G-F
    //         [0 0 0 0 1 0 0 0 0]  : [ -> [
    //         [0 0 0 0 0 1 0 0 0]  : ] -> ]
    //         [0 0 0 0 0 0 0 0 0]  : x ->
    //         [0 0 0 0 0 0 0 1 0]  : y -> y
    //         [0 0 0 0 0 0 0 0 1]] : z -> z
    const std::vector<std::vector<int>> expected_data = { {0, 0, 1, 0, 0, 0, 0, 0, 1} };

    const Matrix computed_matrix = lsys_axiom_matrix(lsys, expected_predecessors);

    ASSERT_EQ(expected_data, computed_matrix.get_data());
}
TEST_F(size_computer_test, lsys_axiom_matrixKOnoSorted)
{
    const std::string wrong_predecessors = "-+GF][zyx";

    ASSERT_ANY_THROW(lsys_axiom_matrix(lsys, wrong_predecessors));
}
TEST_F(size_computer_test, map_matrix)
{
    // const InterpretationMap map{{'F', go_forward},
    //                             {'G', go_forward},
    //                             {'-', turn_left},
    //                             {'+', turn_right},
    //                             {'[', save_position},
    //                             {']', load_position},
    //                             {'y', go_forward}};
    // const int vx_per_goforward = 1;
    // const int vx_per_loadposition = 3;
    // PREDS:  + - F G [ ] x y z      (x,z are in the LSystem)
    // [[0]  : +
    //  [0]  : -
    //  [1]  : F
    //  [1]  : G
    //  [0]  : [
    //  [3]  : ] 
    //  [0]  : x
    //  [1]  : y
    //  [1]] : z
    const std::vector<std::vector<int>> expected_data =
    { {0},
      {0},
      {vx_per_goforward},
      {vx_per_goforward},
      {0},
      {vx_per_loadposition},
      {0},
      {vx_per_goforward},
      {0}};

    const Matrix computed_matrix = map_matrix(map, expected_predecessors);

    ASSERT_EQ(expected_data, computed_matrix.get_data());
}
TEST_F(size_computer_test, map_matrixKOnoSorted)
{
    const std::string wrong_predecessors = "-+GF][zyx";

    ASSERT_ANY_THROW(map_matrix(map, wrong_predecessors));
}
TEST_F(size_computer_test, compute_max_size)
{
  constexpr int n_iter = 7;

  auto sizes = compute_max_size(lsys, map, n_iter);

  // Computed directely from the application
  constexpr int expected_lsys_size = 5833 * bytes_per_predecessor;
  constexpr int expected_vx_size = 3829 * bytes_per_vertex;
  ASSERT_EQ(sizes.lsystem_size, expected_lsys_size);
  ASSERT_EQ(sizes.vertices_size, expected_vx_size);

}
// Test if compute_max_size returns a bigger or equal size for unbalanced
// LSystems.
TEST_F(size_computer_test, compute_max_size_unbalanced)
{
    constexpr int n_iter = 7;
    LSystem unbalanced_lsys { "F", {{'F', "F][]G"}, {'G', "F]]"}}, "" };
    InterpretationMap no_const_map = map;
    DrawingParameters params;
    params.set_n_iter(n_iter);
    
    const auto [str, _1, _2] = unbalanced_lsys.produce(n_iter);
    const auto& [vx, _3, _4] = drawing::compute_vertices(unbalanced_lsys, no_const_map, params);

    int lsys_size = str.size() * bytes_per_predecessor;
    int vx_size = vx.size() * bytes_per_vertex;
    
    auto sizes = compute_max_size(lsys, map, n_iter);

    ASSERT_GE(sizes.lsystem_size, lsys_size);
    ASSERT_GE(sizes.vertices_size, vx_size);
}
TEST_F(size_computer_test, compute_max_sizeKO_n)
{
  constexpr int n_iter = -1;

  ASSERT_ANY_THROW(compute_max_size(lsys, map, n_iter));
}
