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
        std::size_t row_dim = data.at(0).size();
        Expects(row_dim != 0);
        for (auto i=1ull; i<data.size(); ++i)
        {
            Expects(data.at(i).size() == row_dim);
        }
    }

    Matrix::Matrix(std::size_t i, std::size_t j)
    {
        std::vector<int> inners (j, 0);
        std::vector<std::vector<int>> data (i, inners);
        data_ = data;
    }
    
    Matrix& Matrix::operator*=(const Matrix& rhs)
    {
        const size_t this_column_dim = data_.size();
        const size_t this_row_dim = data_.at(0).size();
        const size_t rhs_column_dim = rhs.data_.size();
        const size_t rhs_row_dim = rhs.data_.at(0).size();
        Expects(this_row_dim == rhs_column_dim);

        Matrix mult (this_column_dim, rhs_row_dim);
        
        for (auto i=0ull; i<this_column_dim; ++i)
        {
            for (auto j=0ull; j<rhs_row_dim; ++j)
            {
                for (auto k=0ull; k<rhs_column_dim; ++k)
                {
                    mult.data_.at(i).at(j) += data_.at(i).at(k) * rhs.data_.at(k).at(j);
                }
            }
        }

        *this = mult;
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
        int sum = 0;
        for (auto i=0ull; i<data_.size(); ++i)
        {
            for (auto j=0ull; j<data_.at(0).size(); ++j)
            {
                sum += data_.at(i).at(j);
            }
        }
        return sum;
    }
    
    std::string all_predecessors (const LSystem &lsystem, const drawing::InterpretationMap &map)
    {
        std::string predecessors;
        for (const auto& pred : lsystem.get_axiom())
        {
            predecessors += pred;
        }
        for (const auto& [pred, succ] : lsystem.get_rules())
        {
            predecessors += pred;
        }
        for (const auto& [pred, rule] : map.get_rules())
        {
            predecessors += pred;
        }
        std::sort(begin(predecessors), end(predecessors));
        auto last = std::unique(begin(predecessors), end(predecessors));
        predecessors.erase(last, end(predecessors));
        return predecessors;
    }
    Matrix lsys_axiom_matrix (const LSystem& lsystem, const std::string& predecessors)
    {
        Expects(std::is_sorted(begin(predecessors), end(predecessors)));
        
        const auto n = predecessors.size();
        std::vector<int> inner (n, 0);

        const auto& axiom = lsystem.get_axiom();
        for (auto i=0ull; i<n; ++i)
        {
            inner.at(i) = std::count(begin(axiom), end(axiom), predecessors.at(i));
        }
        std::vector<std::vector<int>> data (1, inner);
        return Matrix(data);
    }
    Matrix lsys_rules_matrix (const LSystem& lsystem, const std::string& predecessors)
    {
        Expects(std::is_sorted(begin(predecessors), end(predecessors)));
        
        // Please see the documentation to understand the matrix.
        const auto n = predecessors.size();
        std::vector<int> inner (n, 0);
        std::vector<std::vector<int>> data (n, inner);

        const auto& rules = lsystem.get_rules();
        for (auto i=0ull; i<n; ++i)
        {
            // For each 'row' representing the rule of 'row_pred', 
            auto& row = data.at(i);
            char row_pred = predecessors.at(i);

            // Check if a 'rule' exists, 
            if (rules.count(row_pred) > 0)
            {
                const auto& rule = rules.at(row_pred);
                for (auto j=0ull; j<n; ++j)
                {
                    // and add the number of each predecessor in the successor
                    row.at(j) = std::count(begin(rule), end(rule), predecessors.at(j));
                }
            }
            else
            {
                // If this is a default rule, simply add '1' in the correct place
                row.at(i) = 1;
            }
        }
        
        return Matrix(data);
    }
    Matrix map_matrix (const drawing::InterpretationMap& map, const std::string& predecessors)
    {
        Expects(std::is_sorted(begin(predecessors), end(predecessors)));

        const auto n = predecessors.size();
        std::vector<int> inner(1, 0);
        std::vector<std::vector<int>> data(predecessors.size(), inner);

        const auto& rules = map.get_rules();
        for (auto i=0ull; i<n; ++i)
        {
            // For each 'row' representing the number of vertices for
            // 'row_pred',
            auto& row = data.at(i);
            char row_pred = predecessors.at(i);

            // Check if a 'order' exists.
            if (rules.count(row_pred) > 0)
            {
                const auto& order = rules.at(row_pred);

                // If so, put the correct number of vertices in the row.
                if (order.id == goforward)
                {
                    row.at(0) = vx_per_goforward;
                }
                else if (order.id == loadposition)
                {
                    row.at(0) = vx_per_loadposition;
                }
            }
            // Otherwise, keep the row at 0.
        }

        return Matrix(data);
    }

    system_size compute_max_size(const LSystem &lsystem,
                                 const drawing::InterpretationMap &map,
                                 int n_iter) {
      Expects(n_iter >= 0);

      system_size sizes;
      
      const auto predecessors = all_predecessors(lsystem, map);
      const auto axiom_mat = lsys_axiom_matrix(lsystem, predecessors);
      const auto lsys_mat = lsys_rules_matrix(lsystem, predecessors);
      const auto map_mat = map_matrix(map, predecessors);

      Matrix lsys_production = axiom_mat;
      for (int i = 0; i < n_iter; ++i)
      {
          lsys_production *= lsys_mat;
      }
      sizes.lsystem_size = lsys_production.grand_sum() * bytes_per_predecessor;

      Matrix vertices_interpretation = lsys_production * map_mat;
      sizes.vertices_size = vertices_interpretation.grand_sum();
      sizes.vertices_size += 1; // first vertex
      sizes.vertices_size *= bytes_per_vertex;
      
      return sizes;
    }
}
