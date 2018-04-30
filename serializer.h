#ifndef SERIALIZER
#define SERIALIZER

#include <string>
#include <unordered_map>
#include <variant>
#include <memory>

#include "LSystemView.h"
#include "DrawingParameters.h"

namespace saveload
{
    struct SerializedTree
    {
        using ChildrenVec = std::vector<std::shared_ptr<SerializedTree>>;

        std::string key;
        std::variant <std::string, ChildrenVec> value;
        static constexpr int string_val = 0;
        static constexpr int tree_val = 1;
    };
    using SerializedTreePtr = std::shared_ptr<SerializedTree>;
    
    std::string print_serialized_tree(const SerializedTree& tree, int depth = 0);
    void add_node(SerializedTree::ChildrenVec& vec, const std::string& key, const std::string& value);
    void add_node(SerializedTree::ChildrenVec& vec, const std::string& key, const SerializedTree::ChildrenVec& value);
    
    std::string print_lsystem(const procgui::LSystemView& view);
    SerializedTreePtr serialized_model(const LSystem& lsys);
    SerializedTreePtr serialized_parameters(const drawing::DrawingParameters& params);
    SerializedTreePtr serialized_interpretations(const drawing::InterpretationMap& map);
}

#endif // SERIALIZER
