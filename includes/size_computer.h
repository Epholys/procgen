#ifndef SIZE_COMPUTER_H
#define SIZE_COMPUTER_H

#include <vector>
#include <string>

class LSystem;
namespace drawing
{
    class InterpretationMap;
}

namespace drawing
{
    struct system_size
    {
        int lsystem_size { 0 };
        int vertices_size { 0 };
    };

    class Matrix
    {
    public:
        Matrix() = default;
        ~Matrix() = default;
        explicit Matrix(const std::vector<std::vector<int>>& data);
        Matrix(const Matrix& matrix) = default;
        Matrix(Matrix&& matrix) = default;
        Matrix& operator=(const Matrix& matrix) = default;
        Matrix& operator=(Matrix&& matrix) = default;

        Matrix& operator*=(const Matrix& rhs);
        friend Matrix operator*(Matrix lhs, const Matrix& rhs);
        
        const std::vector<std::vector<int>>& get_data() const;
        int grand_sum() const;
        
    private:
        std::vector<std::vector<int>> data_ {{0}};
    };


    std::string all_predecessors (const LSystem &lsystem, const drawing::InterpretationMap &map);
    Matrix lsys_matrix (const LSystem& lsystem, const std::string& all_predecessors);
    Matrix map_matrix (const drawing::InterpretationMap& map, const std::string& all_predecessors);
    // PRECOND: Well-formed (balanced save/load)
    system_size compute_size(const LSystem &lsystem,
                             const drawing::InterpretationMap &map,
                             int n_iter);
}


#endif // SIZE_COMPUTER_H
