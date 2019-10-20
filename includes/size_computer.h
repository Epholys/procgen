#ifndef SIZE_COMPUTER_H
#define SIZE_COMPUTER_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "InterpretationMap.h"

class LSystem;

// Functions to compute the element size of the LSystem string production and
// Turtle vertices interpretation.
namespace drawing
{
    constexpr OrderID goforward = drawing::OrderID::GO_FORWARD;
    constexpr OrderID loadposition = drawing::OrderID::LOAD_POSITION;
    constexpr int vx_per_goforward = 1;
    constexpr int vx_per_loadposition = 3;
    constexpr int bytes_per_predecessor = sizeof(char);
    constexpr int bytes_per_vertex = sizeof(sf::Vertex);
    
    // Struct containing the number of element of a complete system:
    struct system_size
    {
        int lsystem_size { 0 };  // number of element for the LSystem
        int vertices_size { 0 }; // number of element for the vertices
    };

    // Classic Matrix helper class to compute the size, as we use Linear
    // Algebra. This is a minimalist class only implementing what's necessary.
    class Matrix
    {
    public:
        // Default rule-of-five ctors
        Matrix() = default;
        ~Matrix() = default;
        // Create a matrix from 'data'
        explicit Matrix(const std::vector<std::vector<int>>& data);
        // Create a matrix filled with 0 with dimension i*j (i=number of
        // columns).
        Matrix(std::size_t i, std::size_t j);
        Matrix(const Matrix& matrix) = default;
        Matrix(Matrix&& matrix) = default;
        Matrix& operator=(const Matrix& matrix) = default;
        Matrix& operator=(Matrix&& matrix) = default;

        Matrix& operator*=(const Matrix& rhs);
        friend Matrix operator*(Matrix lhs, const Matrix& rhs);

        // Getter
        const std::vector<std::vector<int>>& get_data() const;

        // Compute the grand sum : the sum of all element of the matrix.
        int grand_sum() const;
        
    private:
        // Easiest data structure to implement the Matrix.
        // The innermost vectors<int> represents the rows, the outermost
        // vector<int> is a collection of rows.
        // So, the first dim is the column number, and the second the rows number.
        std::vector<std::vector<int>> data_ {{0}};
    };

    // Returns all the predecessors of the system LSystem-InterpretationMap, in
    // a sorted std::string.
    std::string all_predecessors (const LSystem &lsystem, const drawing::InterpretationMap &map);

    // Returns the linear algebra Matrix interpretation of 'lsystem'.
    // In this matrix, a row represents the rule associated with a predecessor,
    // and each value on this row is the number of predecessor in this rule.
    // 
    // Precondition:
    //   - 'predecessors' must be sorted. The coherence of the predecessors in
    //   the matrix and with the map_matrix depends on this.
    Matrix lsys_rules_matrix (const LSystem& lsystem, const std::string& predecessors);

    // Returns the linear algebra Matrix interpretation of 'lsystem'.
    // In this matrix, the unique row represents the rule the number of
    // predecessors in the 'lsystem's axiom.
    // 
    // Precondition:
    //   - 'predecessors' must be sorted. The coherence of the predecessors in
    //   the matrix and with the map_matrix depends on this.
    Matrix lsys_axiom_matrix (const LSystem& lsystem, const std::string& predecessors);
    
    // Returns the linear algebra Matrix interpretation of 'map'
    // In this matrix, each row represents a predecessor, and its value
    // represent the number of vertices produced.
    // 
    // Precondition:
    //   - 'predecessors' must be sorted. The coherence of the predecessors in
    //   the matrix and with the lsys_*_matrices depends on this.
    Matrix map_matrix (const drawing::InterpretationMap& map, const std::string& predecessors);

    // Returns the size of the system 'lsystem'-'map' at iteration 'n_iter'.
    // 
    // If the resulting LSystem string is not balanced regarding the
    // 'save_position' and 'load_position', the returned size will be higher
    // than the real one. As the use case of the function is to warn for large
    // LSystems and to optimize with 'vector.reserve()',n this is not a problem.
    //
    // Preconditions:
    //   - n_iter must be positive.
    //
    // Checking if a LSystem produces balanced strings is complicated, as it is
    // not enough to check each rule, as the axiom, and each different rules may
    // interact with each other:
    //     ax="F]" ; F -> "[F[A]G" and G -> "[A]"
    // produces balanced strings.
    // Another complication is the derivation of brackets themselves, like:
    //     [ -> "[]]"
    // or also the use of 'save_position' and 'load_position' with several
    // predecessors.
    // As a consequence, this function is approximate as the use case does not
    // justifies an exact results.
    system_size compute_max_size(const LSystem &lsystem,
                                 const drawing::InterpretationMap &map,
                                 int n_iter);
}


#endif // SIZE_COMPUTER_H
