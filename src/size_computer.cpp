#include <gsl/gsl>
#include "size_computer.h"
#include "LSystem.h"
#include "InterpretationMap.h"

namespace drawing
{
    Matrix::Matrix(const std::vector<std::vector<int>> & data)
        : data_{data}
    {
        Expects(!data.empty());
        std::size_t second_dim = data.at(0).size();
        Expects(second_dim != 0);
        for (auto i=1ull; i<data.size(); ++i)
        {
            Expects(data.at(i).size() == second_dim);
        }
    }

    Matrix& Matrix::operator*=(const Matrix& rhs)
    {
        
        return *this;
    }

    Matrix operator*(Matrix lhs, const Matrix& rhs)
    {
        lhs *= rhs;
        return lhs;
    }
    
    const std::vector<std::vector<int>>& Matrix::get_data() const
    {
        return data_;
    }

    int Matrix::grand_sum() const
    {
        return -1;
    }
    
    std::string all_predecessors (const LSystem &lsystem, const drawing::InterpretationMap &map)
    {
        return "";
    }
    Matrix lsys_matrix (const LSystem& lsystem, const std::string& all_predecessors)
    {
        return {};
    }
    Matrix map_matrix (const drawing::InterpretationMap& map, const std::string& all_predecessors)
    {
        return {};
    }
    
    system_size compute_size(const LSystem &lsystem,
                             const drawing::InterpretationMap &map,
                             int n_iter)
    {
        return system_size{};
    }
}
