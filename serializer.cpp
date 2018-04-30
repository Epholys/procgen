#include <sstream>
#include "gsl/gsl_assert"
#include "helper_string.h"
#include "helper_math.h"
#include "serializer.h"

namespace saveload
{
    std::string newline(int depth)
    {
        std::string res;
        res += "\n";
        for (int i=0; i<depth; ++i)
        {
            res += "\t";
        }
        return res;

    }
    
    std::string print_serialized_tree(const SerializedTree& tree, int depth)
    {
        std::ostringstream ss;

        auto key = tree.key;
        key = trim(key);
        Expects(is_single_word(key));

        ss << newline(depth);
        ss << key;

        auto index = tree.value.index();
        Expects(index != std::variant_npos);

        switch (index)
        {
        case SerializedTree::string_val:
            ss << " = \"" << std::get<SerializedTree::string_val>(tree.value) << "\"";
            break;
        case SerializedTree::tree_val:
            ss << newline(depth) << "BEGIN";
            ++depth;
            for (const auto& node : std::get<SerializedTree::tree_val>(tree.value))
            {
                ss << print_serialized_tree(*node, depth);
            }
            --depth;
            ss << newline(depth) << "END";
        }

        return ss.str();
    }
    void add_node(SerializedTree::ChildrenVec& vec, const std::string& key, const std::string& value)
    {
        auto node = std::make_shared<SerializedTree>();
        node->key = key;
        node->value = value;
        vec.push_back(std::move(node));
    }

    void add_node(SerializedTree::ChildrenVec& vec, const std::string& key, const SerializedTree::ChildrenVec& value)
    {
        auto node = std::make_shared<SerializedTree>();
        node->key = key;
        node->value = value;
        vec.push_back(std::move(node));        
    }

    std::string print_lsystem(const procgui::LSystemView& view)
    {
        SerializedTree tree;
        
        SerializedTree::ChildrenVec data
               {serialized_model(*view.Observer<LSystem>::get_target()),
                serialized_parameters(view.get_parameters()),
                serialized_interpretations(*view.Observer<drawing::InterpretationMap>::get_target())};
        
        tree.key = "LSYSTEM";
        tree.value = data;
        return print_serialized_tree(tree);
    }
    
    SerializedTreePtr serialized_model(const LSystem& lsys)
    {
        auto tree = std::make_shared<SerializedTree>();
        
        SerializedTree::ChildrenVec data;
        add_node(data, "axiom", lsys.get_axiom());

        SerializedTree::ChildrenVec vec;
        for (const auto& rule : lsys.get_rules())
        {
            add_node(vec, std::string()+rule.first, rule.second);
        }
        add_node(data, "rules", vec);

        tree->key = "LSystem";
        tree->value = data;
        return  tree;
    }
    SerializedTreePtr serialized_parameters(const drawing::DrawingParameters& params)
    {
        auto tree = std::make_shared<SerializedTree>();

        SerializedTree::ChildrenVec data;
        add_node(data, "starting_angle", std::to_string(math::rad_to_degree(params.starting_angle)));
        add_node(data, "delta_angle", std::to_string(math::rad_to_degree(params.delta_angle)));
        add_node(data, "step", std::to_string(params.step));
        add_node(data, "n_iter", std::to_string(params.n_iter));

        tree->key = "DrawingParameters";
        tree->value = data;
        return tree;
    }
    
    SerializedTreePtr serialized_interpretations(const drawing::InterpretationMap& map)
    {
        auto tree = std::make_shared<SerializedTree>();

        SerializedTree::ChildrenVec data;
        for (const auto& rule : map.get_rules())
        {
            add_node(data, std::string()+rule.first, procgui::get_order_entry(rule.second).name);
        }

        tree->key = "InterpretationMap";
        tree->value = data;
        return tree;
    }
}
