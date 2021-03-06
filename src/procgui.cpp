#include "procgui.h"

#include "ColorsGenerator.h"
#include "RenderWindow.h"
#include "VertexPainterComposite.h"
#include "VertexPainterConstant.h"
#include "VertexPainterIteration.h"
#include "VertexPainterLinear.h"
#include "VertexPainterRadial.h"
#include "VertexPainterRandom.h"
#include "VertexPainterSequential.h"
#include "WindowController.h"
#include "config.h"
#include "helper_string.h"
#include "imgui/imgui_internal.h"
#include "imgui_extension.h"

#include <cctype>
#include <cstring>

using namespace math;
using std::clamp;

namespace
{
// At value 0, 'set_up()' will open a new window. Otherwise, it creates a
// CollapsingHeader. 'push/pop_embedded()' is called before and after
// calling 'interact_with()' to create a tree of embedded content.
int embedded_level = 0;

void push_embedded()
{
    ++embedded_level;
}

void pop_embedded()
{
    if (embedded_level > 0)
    {
        --embedded_level;
    }
}

// The two next function are shared with the 'display()' and
// 'interact_with()' functions. They manage window creation and integration.

// Open a window or create a CollapsingHeader according to the value of
// 'embedded_level' with the name 'name'. This name is important: if two
// windows has the same name, the data will be displayed inside the same
// window. If two CollapsingHeaders have the same name, some collision
// between the widgets will create some innapropriate behaviour.
// If 'open' is set, the window will show a close widget which when clicking
// will set the boolean to false.
// Returns 'false' if the window or the CollapsingHeader is collapsed, to
// early-out.
bool set_up(const std::string& name, bool* open = nullptr)
{
    // If the data is not embedded, creates a new window.
    if (embedded_level == 0)
    {
        bool is_active = ImGui::Begin(name.c_str(), open, ImGuiWindowFlags_NoSavedSettings);
        if (!is_active)
        {
            // Window is collapsed, call End();
            ImGui::End();
        }
        return is_active;
    }
    // Otherwise, set up a CollapsingHeader


    bool is_active = ImGui::CollapsingHeader(name.c_str());
    if (is_active)
    {
        // Avoid name collision between two widgets (like '+' button).
        ImGui::PushID(name.c_str());
        ImGui::Indent();
    }
    return is_active;
}

// Concludes the window or CollapsingHeader
void conclude()
{
    // If this function is called at the window level, close the window.
    if (embedded_level == 0)
    {
        ImGui::Separator();
        ImGui::End();
    }
    else
    {
        ImGui::Unindent();
        ImGui::PopID();
    }
}

void show_drag_info()
{
    ext::ImGui::ShowHelpMarker("Click and drag to quickly change value.");
}
} // namespace


namespace procgui
{
void display(const LSystem& lsys, const std::string& name)
{
    if (!set_up(name))
    {
        // Early out if the display zone is collapsed.
        return;
    }

    // --- Axiom ---
    ImGui::Text("Axiom:");

    ImGui::Indent();

    ImGui::Text("%s", lsys.get_axiom().c_str());

    ImGui::Unindent();


    // --- Production Rules ---
    ImGui::Text("Production rules:");

    ImGui::Indent();
    for (const auto& rule : lsys.get_rules())
    {
        std::ostringstream oss;
        oss << rule.first << " -> " << rule.second;
        std::string str = oss.str();
        ImGui::Text("%s", str.c_str());
    }
    ImGui::Unindent();

    // --- Iteration Predecessors ---
    ImGui::Text("Iteration predecessors:");

    ImGui::Indent();

    ImGui::Text("%s", lsys.get_iteration_predecessors().c_str());

    ImGui::Unindent();


    conclude();
}


void display(const drawing::DrawingParameters& parameters, const std::string& name)
{
    if (!set_up(name))
    {
        // Early out if the display zone is collapsed.
        return;
    }

    // Arbitrary value to align neatly every members.
    const int align = 150;

    // --- Starting Position ---
    ImGui::Text("Starting Position:");
    ImGui::SameLine(align);
    ImGui::Text("x: %.f", parameters.get_starting_position().x);
    ImGui::SameLine();
    ImGui::Text("y: %.f", parameters.get_starting_position().y);

    // --- Starting Angle ---
    ImGui::Text("Starting Angle:");
    ImGui::SameLine(align);
    ImGui::Text("%.lf", math::rad_to_degree(parameters.get_starting_angle()));
    ImGui::SameLine();
    ImGui::Text("degree");

    // --- Angle Delta ---
    ImGui::Text("Angle Delta:");
    ImGui::SameLine(align);
    ImGui::Text("%.lf", math::rad_to_degree(parameters.get_delta_angle()));
    ImGui::SameLine();
    ImGui::Text("degree");

    // --- Step ---
    ImGui::Text("Step:");
    ImGui::SameLine(align);
    ImGui::Text("%.1lf", parameters.get_step());

    conclude();
}

void display(const drawing::InterpretationMap& map, const std::string& name)
{
    if (!set_up(name))
    {
        // Early out if the display zone is collapsed.
        return;
    }

    for (const auto& interpretation : map.get_rules())
    {
        std::string name = interpretation.second.name;
        ImGui::Text("%c -> %s", interpretation.first, name.c_str());
    }

    conclude();
}


void interact_with(drawing::DrawingParameters& parameters, const std::string& name)
{
    if (!set_up(name))
    {
        return;
    }

    // --- Starting position ---
    std::array<double, 2> pos {parameters.get_starting_position().x,
                               parameters.get_starting_position().y};
    if (ext::ImGui::DragDouble2("Starting position", pos.data(), 1.f, 0.f, 0.f, "%.lf"))
    {
        // is_modified_ is not set: the render state take care of translating the view.
        ext::sf::Vector2d starting_position {pos[0], pos[1]};
        parameters.set_starting_position(starting_position);
    }
    ImGui::SameLine();
    show_drag_info();

    // --- Starting angle ---
    double starting_angle_deg = parameters.get_starting_angle();
    if (ext::ImGui::SliderAngleDouble("Starting Angle", &starting_angle_deg, 0.))
    {
        starting_angle_deg = clamp(starting_angle_deg, 0., 2 * math::pi);
        parameters.set_starting_angle(starting_angle_deg);
    }

    // --- Angle Delta ---
    double delta_angle_deg = parameters.get_delta_angle();
    if (ext::ImGui::SliderAngleDouble("Angle Delta", &delta_angle_deg, 0.))
    {
        delta_angle_deg = clamp(delta_angle_deg, 0., 2 * math::pi);
        parameters.set_delta_angle(delta_angle_deg);
    }

    // --- Step ---
    double step = parameters.get_step();
    if (ext::ImGui::DragDouble("Step", &step, 0.2f, 0, double_max_limit, "%#.1lf"))
    {
        step = clamp(step, 0., double_max_limit);
        parameters.set_step(step);
    }
    ImGui::SameLine();
    show_drag_info();

    // --- Iterations ---
    // Arbitrary value to avoid resource depletion happening with higher
    // number of iterations (several GiB of memory usage and huge CPU
    // load).
    constexpr int n_iter_max = 10;
    constexpr int absolute_max = 255; // uint8_t max.
    const int current_n_iter = parameters.get_n_iter();
    int n_iter = current_n_iter;
    if (ImGui::SliderInt("Iterations", &n_iter, 0, n_iter_max))
    {
        if (n_iter < 0)
        {
            n_iter = current_n_iter;
        }
        else if (n_iter > absolute_max)
        {
            n_iter = absolute_max;
        }
        parameters.set_n_iter(n_iter);
    }
    ImGui::SameLine();
    ext::ImGui::ShowHelpMarker("CTRL+click and click to directly input values. Higher values will "
                               "use all of your memory and CPU");

    conclude();
}

void interact_with(LSystemBuffer& buffer, const std::string& name)
{
    if (!set_up(name))
    {
        return;
    }

    // The LSystem itelf
    auto& lsys = buffer.ref_rule_map();

    // --- Axiom ---
    auto buf = string_to_array<lsys_successor_size>(lsys.get_axiom());

    if (ImGui::InputText("Axiom", buf.data(), lsys_successor_size))
    {
        lsys.set_axiom(array_to_string(buf));
    }

    //  --- Rules ---
    // [ predecessor ] -> [ successor ] [-] (remove rule) | [+] (add rule)

    ImGui::Text("Production rules:");
    interact_with_buffer(buffer, [&buffer](auto it, bool& updated) {
        // Interact with the successor. Except for the input size, does not
        // have any constraints.
        auto array = string_to_array<lsys_successor_size>(it->successor);
        if (ImGui::InputText("##succ", array.data(), lsys_successor_size) && !updated)
        {
            buffer.change_successor(it, array_to_string(array));
            updated = true;
        }
    });

    // --- Iteration Predecessors ---
    buf = string_to_array<lsys_successor_size>(lsys.get_iteration_predecessors());
    if (ImGui::InputText("Iteration predecessors", buf.data(), lsys_successor_size))
    {
        lsys.set_iteration_predecessors(array_to_string(buf));
    }
    ImGui::SameLine();
    ext::ImGui::ShowHelpMarker("Used in the Iterative Painter.");

    conclude();
}

void interact_with(InterpretationMapBuffer& buffer, const std::string& name)
{
    if (!set_up(name))
    {
        // Early out
        return;
    }

    using namespace drawing;

    interact_with_buffer(buffer, [&buffer](auto it, bool& updated) {
        // ImGui::ListBox needs:
        //   - An array of 'char *' for the different elements
        //   - An index to select between these elements

        // As 'all_orders_name' has the exact same order as
        // 'all_orders', the index is common.
        //
        // The index is calculated by finding in the vector the order
        // and using the distance between the first element and the
        // current one.
        auto selected_interpretation_it = std::find_if(
            all_orders.begin(),
            all_orders.end(),
            [&it](const auto& o) { return o.id == it->successor.id; });
        int index = std::distance(all_orders.begin(), selected_interpretation_it);
        if (ImGui::ListBox("##order", &index, all_orders_name.data(), all_orders_name.size())
            && !updated)
        {
            buffer.change_successor(it, all_orders.at(index));
            updated = true;
        }
    });

    conclude();
}
} // namespace procgui
namespace
{
void interact_with(colors::VertexPainterConstant& painter)
{
    ImGui::Text("Paint all th vertices in the same color.");
    ::procgui::interact_with(painter.ref_generator_wrapper(),
                             "Colors",
                             ::procgui::color_wrapper_mode::CONSTANT);
}
void interact_with(colors::VertexPainterLinear& painter, bool from_composite = false)
{
    ImGui::Text("Paint the vertices on both side of an axis.");

    bool display_helper = painter.get_display_flag();
    if (ImGui::Checkbox("Display angle marker", &display_helper))
    {
        painter.set_display_flag(display_helper);
    }

    // --- Gradient angle ---
    float angle = math::degree_to_rad(painter.get_angle());
    if (ImGui::SliderAngle("Gradient Angle", &angle, 0.f))
    {
        angle = clamp(angle, 0.f, 2 * static_cast<float>(math::pi));
        painter.set_angle(math::rad_to_degree(angle));
    }

    if (!from_composite)
    {
        ::procgui::interact_with(painter.ref_generator_wrapper(),
                                 "Colors",
                                 ::procgui::color_wrapper_mode::GRADIENTS);
    }
}

void interact_with(colors::VertexPainterRadial& painter, bool from_composite = false)
{
    ImGui::Text("Paint the vertices around a center.");

    bool display_helper = painter.get_display_flag();
    if (ImGui::Checkbox("Display center marker", &display_helper))
    {
        painter.set_display_flag(display_helper);
    }

    // --- Center ---
    std::array<float, 2> center {painter.get_center().x, painter.get_center().y};
    if (ImGui::SliderFloat2("Circle Center", center.data(), 0.f, 1.f, "%.2f"))
    {
        center[0] = clamp(center[0], 0.f, 1.f);
        center[1] = clamp(center[1], 0.f, 1.f);
        painter.set_center(sf::Vector2f(center[0], center[1]));
    }

    if (!from_composite)
    {
        ::procgui::interact_with(painter.ref_generator_wrapper(),
                                 "Colors",
                                 ::procgui::color_wrapper_mode::GRADIENTS);
    }
}

void interact_with(colors::VertexPainterRandom& painter, bool from_composite = false)
{
    ImGui::Text("Paint the vertices randomly in blocks.");

    int block_size = painter.get_block_size();
    if (ImGui::DragInt("Block size", &block_size, 1, 1, std::numeric_limits<int>::max()))
    {
        if (block_size > 0)
        {
            painter.set_block_size(block_size);
        }
    }
    ImGui::SameLine();
    show_drag_info();

    ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
    if (ImGui::Button("Randomize"))
    {
        painter.randomize();
    }
    ImGui::PopStyleColor(3);

    if (!from_composite)
    {
        ::procgui::interact_with(painter.ref_generator_wrapper(),
                                 "Colors",
                                 ::procgui::color_wrapper_mode::GRADIENTS);
    }
}

void interact_with(colors::VertexPainterSequential& painter, bool from_composite = false)
{
    ImGui::Text("Paint the vertices in the order of the array.");

    double factor = painter.get_factor();
    if (ext::ImGui::DragDouble("Repetition factor", &factor, 0.01f, 0.f, double_max_limit, "%.2f"))
    {
        factor = clamp(factor, 0., double_max_limit);
        painter.set_factor(factor);
    }
    ImGui::SameLine();
    show_drag_info();

    if (!from_composite)
    {
        ::procgui::interact_with(painter.ref_generator_wrapper(),
                                 "Colors",
                                 ::procgui::color_wrapper_mode::GRADIENTS);
    }
}

void interact_with(colors::VertexPainterIteration& painter, bool from_composite = false)
{
    ImGui::Text(
        "Paint the vertices according to the iteration depth\nof \"Iteration predecessors\".");

    if (!from_composite)
    {
        ::procgui::interact_with(painter.ref_generator_wrapper(),
                                 "Colors",
                                 ::procgui::color_wrapper_mode::GRADIENTS);
    }
}

void interact_with(colors::VertexPainterComposite& painter)
{
    // Composite of composite will have ImGui's widget ID collision issue.
    static int nested_id = 0;
    ImGui::PushID(nested_id++);

    static std::string child_title;
    child_title += "-- " + painter.get_main_painter().unwrap()->type_name() + " ";

    // ImGui's ID for the several child painters.
    int index = 0;
    auto& child_painters = painter.ref_child_painters();
    // If set, 'to_remove' points to the child painter to remove.
    auto to_remove = end(child_painters);
    // If set, 'to_add' points to the position in which to add a painter.
    auto to_add = end(child_painters);
    // If true, we add a new painter at 'to_add'.
    bool add_new_painter = false;
    // If true, we add the copied painter at 'to_add'.
    bool add_copied_painter = false;

    ImGui::Separator();

    // Button to add a new painter at the beginning.
    ::ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
    if (ImGui::Button("Add Painter here"))
    {
        to_add = begin(child_painters);
        add_new_painter = true;
    }
    ImGui::PopStyleColor(3);

    // Button to add the copied painter at the beginning.
    ::ext::ImGui::PushStyleColoredButton<ext::ImGui::Turquoise>();
    if (colors::VertexPainterComposite::has_copied_painter()
        && ImGui::Button("Paste copied Painter here"))
    {
        to_add = begin(child_painters);
        add_copied_painter = true;
    }
    ImGui::PopStyleColor(3);

    ImGui::Separator();

    // Begin the child painters
    for (auto it = begin(child_painters); it != end(child_painters); ++it)
    {
        child_title += std::to_string(index + 1) + " ";
        ImGui::PushID(index);

        // Interact with this child painter.
        push_embedded();
        ::procgui::interact_with(*it, child_title);
        pop_embedded();

        ImGui::Separator();

        // Button to add a new painter at this position.
        ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
        if (ImGui::Button("Add Painter here"))
        {
            to_add = next(it);
            add_new_painter = true;
        }
        ImGui::PopStyleColor(3);

        // Button to remove the previous painter.
        if (child_painters.size() > 1)
        {
            ImGui::SameLine();
            ::ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
            if (ImGui::Button("Remove previous Painter"))
            {
                to_remove = it;
            }
            ImGui::PopStyleColor(3);
        }

        // Button to copy the previous painter.
        ::ext::ImGui::PushStyleColoredButton<ext::ImGui::Purple>();
        if (ImGui::Button("Copy previous Painter"))
        {
            colors::VertexPainterComposite::save_painter(it->unwrap());
        }
        ImGui::PopStyleColor(3);

        // Button to add the copied painter at this position.
        if (colors::VertexPainterComposite::has_copied_painter())
        {
            ImGui::SameLine();
            ::ext::ImGui::PushStyleColoredButton<ext::ImGui::Turquoise>();
            if (ImGui::Button("Paste copied Painter here"))
            {
                to_add = next(it);
                add_copied_painter = true;
            }
            ImGui::PopStyleColor(3);
        }

        ImGui::Separator();
        ImGui::PopID();
        index++;
        child_title.erase(child_title.size() - 2);
    }

    // Remove the painter pointed by 'to_remove', if set.
    if (to_remove != end(child_painters))
    {
        child_painters.erase(to_remove);
        std::vector<colors::VertexPainterWrapper> copy = child_painters;
        painter.set_child_painters(copy);
    }
    // Add a new painter if necessary.
    else if (add_new_painter)
    {
        child_painters.insert(to_add, colors::VertexPainterWrapper());
        std::vector<colors::VertexPainterWrapper> copy = child_painters;
        painter.set_child_painters(copy);
    }
    // Paste the copied painter if necessary.
    else if (add_copied_painter)
    {
        child_painters.insert(
            to_add,
            colors::VertexPainterWrapper(colors::VertexPainterComposite::get_copied_painter()));
        std::vector<colors::VertexPainterWrapper> copy = child_painters;
        painter.set_child_painters(copy);
    }

    nested_id--;
    const auto name = painter.get_main_painter().unwrap()->type_name();
    const auto where = child_title.rfind(name);
    Expects(where != std::string::npos);
    child_title.erase(where - 3, name.size() + 4); //"-- "=3 + name + " "=1

    ImGui::PopID();
}

void create_new_vertex_painter(colors::VertexPainterWrapper& wrapper,
                               const colors::VertexPainter& painter,
                               int index,
                               int old_index)
{
    colors::ColorGeneratorWrapper next_generator;
    if (index == 0)
    {
        auto default_generator = std::make_shared<colors::ConstantColor>();
        next_generator = colors::ColorGeneratorWrapper(default_generator);
    }
    else if (index != 0 && old_index == 0)
    {
        auto default_generator = std::make_shared<colors::LinearGradient>();
        next_generator = colors::ColorGeneratorWrapper(default_generator);
    }
    else
    {
        next_generator = painter.get_generator_wrapper();
    }

    std::shared_ptr<colors::VertexPainter> new_painter;
    switch (index)
    {
    case 0:
        new_painter = std::make_shared<colors::VertexPainterConstant>(next_generator);
        break;

    case 1:
        new_painter = std::make_shared<colors::VertexPainterLinear>(next_generator);
        break;

    case 2:
        new_painter = std::make_shared<colors::VertexPainterRadial>(next_generator);
        break;

    case 3:
        new_painter = std::make_shared<colors::VertexPainterRandom>(next_generator);
        break;

    case 4:
        new_painter = std::make_shared<colors::VertexPainterSequential>(next_generator);
        break;

    case 5:
        new_painter = std::make_shared<colors::VertexPainterIteration>(next_generator);
        break;

    default:
        Ensures(false);
        break;
    }

    wrapper.wrap(new_painter);
}

int vertex_painter_list(colors::VertexPainterWrapper& painter_wrapper)
{
    const auto& painter = *painter_wrapper.unwrap();

    // Represents the index of the next ListBox. Set by inspecting the
    // polyphormism.
    int index = 0;
    const auto& info = typeid(painter).hash_code();
    if (info == typeid(colors::VertexPainterConstant).hash_code())
    {
        index = 0;
    }
    else if (info == typeid(colors::VertexPainterLinear).hash_code())
    {
        index = 1;
    }
    else if (info == typeid(colors::VertexPainterRadial).hash_code())
    {
        index = 2;
    }
    else if (info == typeid(colors::VertexPainterRandom).hash_code())
    {
        index = 3;
    }
    else if (info == typeid(colors::VertexPainterSequential).hash_code())
    {
        index = 4;
    }
    else if (info == typeid(colors::VertexPainterIteration).hash_code())
    {
        index = 5;
    }
    else
    {
        Expects(false);
    }

    int old_index = index;
    const std::array<const char* const, 6> generators {"Constant",
                                                       "Linear",
                                                       "Radial",
                                                       "Random",
                                                       "Sequential",
                                                       "Iterative"};
    bool new_generator = ImGui::ListBox("Vertex Painter", &index, generators.data(), 6)
                         && index != old_index;

    // Create a new VertexPainter
    if (new_generator)
    {
        create_new_vertex_painter(painter_wrapper, painter, index, old_index);
    }

    return index;
}
} // namespace
namespace procgui
{
void interact_with(colors::VertexPainterWrapper& painter_wrapper, const std::string& name)
{
    if (!set_up(name))
    {
        // Early out if the display zone is collapsed.
        return;
    }
    ImGui::Indent();

    // Get the painter from the wrapper.
    auto painter = painter_wrapper.unwrap();

    // Define an empty VertexPainterComposite pointer. If 'painter' is a
    // VertexPainterComposite or if a new composite is created, this pointer
    // will be filled.
    std::shared_ptr<colors::VertexPainterComposite> composite;
    int index = -1; // Index defining the type of 'painter'. Values are
                    // defined in 'vertex_painter_list()'.


    const auto& concrete = *painter;
    auto type = typeid(concrete).hash_code();
    bool is_composite = type == typeid(colors::VertexPainterComposite).hash_code();
    if (is_composite)
    {
        // If 'painter' is a composite, it will now be accessed through
        // 'composite'. 'painter' now refers to the main painter of
        // 'composite'.
        composite = std::dynamic_pointer_cast<colors::VertexPainterComposite>(painter);
        auto main = composite->get_main_painter();
        index = ::vertex_painter_list(main);
        composite->set_main_painter(main);
        painter = composite->get_main_painter().unwrap();
    }
    else
    {
        // Show the 'vertex_painter_list()' and update 'painter' in case in
        // which 'painter' is modified.
        index = ::vertex_painter_list(painter_wrapper);
        painter = painter_wrapper.unwrap();
    }


    // Check if the new/main painter is a VertexPainterConstant
    bool is_constant = index == 0;

    // If not, composite features are shown
    if (!is_constant)
    {
        // Checkbox to choose if the VertexPainter is composite.
        bool checked = ImGui::Checkbox("Composite", &is_composite);
        ImGui::SameLine();
        ext::ImGui::ShowHelpMarker("Paint vertices according to children painters.");
        bool create_new_composite = checked && is_composite;
        bool remove_composite = checked && (!is_composite);

        if (create_new_composite)
        {
            // A new VertexPainterComposite is created and refered by
            // 'composite'. 'painter' is now the main painter of 'composite'.
            composite = std::make_shared<colors::VertexPainterComposite>();
            composite->set_main_painter(colors::VertexPainterWrapper(painter));
            painter_wrapper.wrap(composite);
        }
        if (remove_composite)
        {
            // The main painter of 'composite' is promoted as the real painter.
            auto default_generator = std::make_shared<colors::LinearGradient>();
            painter->set_generator_wrapper(colors::ColorGeneratorWrapper(default_generator));
            painter_wrapper.wrap(painter);
            composite.reset();
        }
    }
    // Special case if we switch from a composite to a constant
    else if (is_composite)
    {
        is_composite = false;
        painter_wrapper.wrap(painter);
        composite.reset();
    }

    // Does not use embedded_level, the generator will be displayed just
    // after the generator selection.
    switch (index)
    {
    case 0:
    {
        auto constant = std::dynamic_pointer_cast<colors::VertexPainterConstant>(painter);
        ::interact_with(*constant);
        break;
    }

    case 1:
    {
        auto linear = std::dynamic_pointer_cast<colors::VertexPainterLinear>(painter);
        ::interact_with(*linear, is_composite);
        break;
    }
    case 2:
    {
        auto radial = std::dynamic_pointer_cast<colors::VertexPainterRadial>(painter);
        ::interact_with(*radial, is_composite);
        break;
    }
    case 3:
    {
        auto random = std::dynamic_pointer_cast<colors::VertexPainterRandom>(painter);
        ::interact_with(*random, is_composite);
        break;
    }
    case 4:
    {
        auto sequential = std::dynamic_pointer_cast<colors::VertexPainterSequential>(painter);
        ::interact_with(*sequential, is_composite);
        break;
    }
    case 5:
    {
        auto iteration = std::dynamic_pointer_cast<colors::VertexPainterIteration>(painter);
        ::interact_with(*iteration, is_composite);
        break;
    }
    default:
        Ensures(false);
        break;
    }

    if (composite)
    {
        ::interact_with(*composite);
    }

    ImGui::Unindent();
    conclude();
}
} // namespace procgui
namespace
{
void interact_with(colors::ConstantColor& constant)
{
    // Color selection widget.
    ImVec4 imcolor = constant.get_imcolor();
    std::array<float, 4> color {imcolor.x, imcolor.y, imcolor.z, imcolor.w};
    if (ImGui::ColorEdit4("Color",
                          color.data(),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel
                              | ImGuiColorEditFlags_AlphaPreviewHalf
                              | ImGuiColorEditFlags_AlphaBar))
    {
        if (!colors::is_transparent(imcolor))
        {
            constant.set_imcolor({color[0], color[1], color[2], color[3]});
        }
    }

    // Display the ConstantColor preview.
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 window_pos = ImGui::GetCursorPos();
    float space_until_border = ImGui::GetWindowWidth() - window_pos.x - 30.f;
    float xsize = (space_until_border < 400) ? space_until_border : 400;
    ImVec2 size {xsize, 30.};
    float checker_box_size = 10.f;
    ImGui::RenderColorRectWithAlphaCheckerboard(
        ImVec2(screen_pos.x, screen_pos.y),
        ImVec2(screen_pos.x + size.x, screen_pos.y + size.y),
        IM_COL32(0, 0, 0, 0),
        checker_box_size,
        ImVec2(0, 0));
    draw_list->AddRectFilled(ImVec2(screen_pos.x, screen_pos.y),
                             ImVec2(screen_pos.x + size.x, screen_pos.y + size.y),
                             ImGui::ColorConvertFloat4ToU32(imcolor));
    ImGui::Dummy(size);
}

void interact_with(colors::LinearGradient& gen)
{
    // Two flags to check if the user just stopped to modify the gradient.
    static bool was_focusing_previous_frame = false;
    bool is_focusing_this_frame = false;
    // Persistent keys to freely modify them here without perverting the
    // pure ones of LinearGradient.
    static colors::LinearGradient::keys keys_buffer;
    // Hacky pointer to make 'keys_buffer' and 'was_focusing_previous_frame'
    // exclusive to one generator. This function is called for each existing
    // LinearGradient, so all the static variables are shared... but we only
    // want the keys buffer behavior for one generator, hence this pointer.
    static colors::LinearGradient* generator_address = nullptr;

    bool is_modified = false;

    colors::LinearGradient::keys keys;
    if ((generator_address != nullptr) && generator_address == &gen && was_focusing_previous_frame)
    {
        // Correct generator && user is interacting with the keys.
        keys = keys_buffer;
    }
    else
    {
        keys = gen.get_keys();
    }

    // Insertion flag and positions
    bool will_insert = false;
    auto to_insert = begin(keys);
    // Deletion flag and positions
    bool will_remove = false;
    auto to_remove = begin(keys);
    // Used as ID for ImGui
    int index = 0;

    const auto& style = ImGui::GetStyle();
    float block_pos = 0.f; // The position in the window
    // Modify 'gen''s keys: colors and position
    for (auto it = begin(keys); it != end(keys); ++it)
    {
        float block_size = 0.f; // The size of the current color block
                                // [+][-] [COL] [+]
                                //        [0.5]
        ImGui::PushID(index);

        // Button '+' to add a key.
        ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
        if (ImGui::Button("+"))
        {
            will_insert = true;
            to_insert = it;

            block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        // Button '-' to remove a key.
        ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
        if (keys.size() > 2 && ImGui::Button("-"))
        {
            will_remove = true;
            to_remove = it;

            block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();


        ImGui::BeginGroup();

        // Color
        ImVec4 imcolor = it->imcolor;
        std::array<float, 4> color {imcolor.x, imcolor.y, imcolor.z, imcolor.w};
        if (ImGui::ColorEdit4("Color",
                              color.data(),
                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel
                                  | ImGuiColorEditFlags_AlphaPreviewHalf
                                  | ImGuiColorEditFlags_AlphaBar)
            && !colors::is_transparent(imcolor))
        {
            is_modified = true;
            it->imcolor = {color[0], color[1], color[2], color[3]};
        }

        // Key's Position
        ImGui::PushItemWidth(50);
        if (ImGui::DragFloat("", &it->position, 0.01, 0., 1., "%.2f"))
        {
            is_modified = true;
        }
        if (ImGui::IsItemActive())
        {
            is_focusing_this_frame = true;
        }
        ImGui::PopItemWidth();

        ImGui::EndGroup();
        block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;

        ImGui::PopID();

        block_size *= 2; // I'm bad with the imgui layout system, so here's
                         // an arbitrary constant to have the desired effect.
        block_pos += block_size;
        if (block_pos + block_size < ImGui::GetWindowContentRegionMax().x)
        {
            // There is enough space, continue on this line.
            ImGui::SameLine();
        }
        else
        {
            // Not enough space, go to the next line and reset the position.
            block_pos = 0.f;
        }

        ++index;
    }

    // Button '+' to add a key at the end.
    ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
    if (ImGui::Button("+"))
    {
        will_insert = true;
        to_insert = end(keys);
    }
    ImGui::PopStyleColor(3);

    // Insertion at 'will_insert'
    if (will_insert)
    {
        is_modified = true;
        if (to_insert == begin(keys))
        {
            auto first_it = begin(keys);
            auto second_it = next(first_it);
            first_it->position = second_it->position / 2.;
            keys.insert(to_insert, {sf::Color::White, 0.f});
        }
        else if (to_insert == end(keys))
        {
            auto ultimate_it = prev(end(keys));
            auto penultimate_it = prev(ultimate_it);
            ultimate_it->position = (1 + penultimate_it->position) / 2.;
            keys.push_back({sf::Color::White, 1.f});
        }
        else
        {
            keys.insert(
                to_insert,
                {sf::Color::White, (to_insert->position + prev(to_insert)->position) / 2.f});
        }
    }
    // Deletion at 'will_remove'
    else if (will_remove)
    {
        is_modified = true;
        if (to_remove == begin(keys))
        {
            next(to_remove)->position = 0.;
        }
        keys.erase(to_remove);
    }

    if (is_modified)
    {
        gen.set_keys(keys);
    }

    if (is_focusing_this_frame)
    {
        // The user started key modification
        was_focusing_previous_frame = true;
        keys_buffer = keys;
        generator_address = &gen;
    }
    else if ((generator_address != nullptr) && generator_address == &gen && !is_focusing_this_frame
             && was_focusing_previous_frame)
    {
        // The user stopped key modification for the concerned generator.
        was_focusing_previous_frame = false;
        generator_address = nullptr;
    }


    // Preview the color gradient
    auto k = gen.get_keys();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImVec2 window_pos = ImGui::GetCursorPos();
    float space_until_border = ImGui::GetWindowWidth() - window_pos.x - 30.f;
    float xsize = (space_until_border < 400) ? space_until_border : 400;
    ImVec2 size {xsize, 30.};
    float checker_box_size = 10.f;
    ImGui::RenderColorRectWithAlphaCheckerboard(
        ImVec2(screen_pos.x, screen_pos.y),
        ImVec2(screen_pos.x + size.x, screen_pos.y + size.y),
        IM_COL32(0, 0, 0, 0),
        checker_box_size,
        ImVec2(0, 0));
    float x = screen_pos.x;
    double ratio = 0.f;
    for (unsigned i = 0; i < k.size() - 1; ++i)
    {
        const auto& col1 = k.at(i).imcolor;
        const auto& col2 = k.at(i + 1).imcolor;
        const auto& f = k.at(i + 1).position;
        draw_list->AddRectFilledMultiColor({x, screen_pos.y},
                                           ImVec2(x + size.x * (f - ratio), screen_pos.y + size.y),
                                           ImGui::ColorConvertFloat4ToU32(col1),
                                           ImGui::ColorConvertFloat4ToU32(col2),
                                           ImGui::ColorConvertFloat4ToU32(col2),
                                           ImGui::ColorConvertFloat4ToU32(col1));

        ratio = f;
        x = screen_pos.x + size.x * f;
    }

    ImGui::Dummy(size);
}

void interact_with(colors::DiscreteGradient& gen)
{
    constexpr int max_colors = 10000;

    bool is_modified = false;

    auto keys = gen.get_keys();
    bool will_insert = false;
    bool insert_before = false;
    auto to_insert = begin(keys);
    bool will_remove = false;
    auto to_remove = begin(keys);

    // Modify 'gen''s keys: key's color and transitional colors between
    // these keys.

    // If the user add or remove a transitional color, it have to offset all
    // next keys' position. 'modifier' conserve the offset's value.
    // The last color is managed at the end, to not add the transitional
    // colors part.
    int index = 0;
    int modifier = 0;

    const auto& style = ImGui::GetStyle();
    float block_pos = 0.f; // The position in the window
    for (auto it = begin(keys); it != prev(end(keys)); ++it, ++index)
    {
        float block_size = 0.f; // The size of the block
                                // [+][-][COL][+]
                                //               [INT][-][+]
        ImGui::PushID(index);

        ImGui::PushID(0);
        ::ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
        if (ImGui::Button("+"))
        {
            will_insert = true;
            insert_before = true;
            to_insert = it;

            block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;
        }
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
        if (keys.size() > 2 && ImGui::Button("-"))
        {
            will_remove = true;
            to_remove = it;

            block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;
        }
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::PopID();

        // Key Color
        ImVec4 imcolor = it->imcolor;
        std::array<float, 4> color {imcolor.x, imcolor.y, imcolor.z, imcolor.w};
        if (ImGui::ColorEdit4("Color",
                              color.data(),
                              ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel
                                  | ImGuiColorEditFlags_AlphaPreviewHalf
                                  | ImGuiColorEditFlags_AlphaBar))
        {
            if (!colors::is_transparent(imcolor))
            {
                is_modified = true;
                it->imcolor = {color[0], color[1], color[2], color[3]};
                ;
            }

            block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;
        }

        ImGui::PushID(1);
        ImGui::SameLine();
        ::ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
        if (ImGui::Button("+"))
        {
            will_insert = true;
            insert_before = false;
            to_insert = it;

            block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;
        }
        ImGui::PopStyleColor(3);
        ImGui::PopID();

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::Text(" "); // '\n'

        // Number of transitional colors.
        int diff = next(it)->index - it->index - 1 + modifier;
        int diff_copy = diff;

        ImGui::PushItemWidth(75.f);
        if (ImGui::InputInt("", &diff, 1, 1) && diff >= 0)
        {
            is_modified = true;
            modifier += diff - diff_copy;
        }
        ImGui::PopItemWidth();

        // Offset the current key.
        next(it)->index += modifier;

        ImGui::EndGroup();
        block_size += ImGui::GetItemRectSize().x + style.ItemSpacing.x;

        ImGui::PopID();

        block_size *= 2.3; // I'm bad with the imgui layout system, so here's
                           // an arbitrary constant to have the desired effect.
        block_pos += block_size;
        if (block_pos + block_size < ImGui::GetWindowContentRegionMax().x)
        {
            // There is enough space, continue on this line.
            ImGui::SameLine();
        }
        else
        {
            // Not enough space, go to the next line and reset the position.
            block_pos = 0.f;
        }
    }

    // Button '+' to add a key.
    ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
    if (ImGui::Button("+"))
    {
        will_insert = true;
        insert_before = true;
        to_insert = prev(end(keys));
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    // Button '-' to remove a key
    ext::ImGui::PushStyleColoredButton<ext::ImGui::Red>();
    if (keys.size() > 2 && (ImGui::SameLine(), ImGui::Button("-")))
    {
        will_remove = true;
        to_remove = prev(end(keys));
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    // Last color widget.
    ImGui::PushID(keys.size());
    ImVec4 imcolor = keys.back().imcolor;
    std::array<float, 4> color {imcolor.x, imcolor.y, imcolor.z, imcolor.w};
    if (ImGui::ColorEdit4("Color",
                          color.data(),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel
                              | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_AlphaBar)
        && !colors::is_transparent(imcolor))
    {
        is_modified = true;
        keys.back().imcolor = {color[0], color[1], color[2], color[3]};
        ;
    }
    ImGui::PopID();

    ImGui::PushID(keys.size());
    ImGui::SameLine();
    // Button '+' to add a key.
    ext::ImGui::PushStyleColoredButton<ext::ImGui::Green>();
    if (ImGui::Button("+"))
    {
        is_modified = true;
        keys.push_back({sf::Color::White, keys.back().index + 1});
    }
    ImGui::PopStyleColor(3);
    ImGui::PopID();


    if (will_insert && insert_before)
    {
        is_modified = true;
        auto inserted = begin(keys);
        if (to_insert == begin(keys))
        {
            inserted = keys.insert(to_insert, {sf::Color::White, 0});
        }
        else
        {
            inserted = keys.insert(to_insert, {sf::Color::White, to_insert->index});
        }
        for (auto it = next(inserted); it != end(keys); ++it)
        {
            it->index += 1;
        }
    }
    else if (will_insert && !insert_before)
    {
        is_modified = true;
        auto inserted = keys.insert(next(to_insert), {sf::Color::White, to_insert->index + 1});
        for (auto it = next(inserted); it != end(keys); ++it)
        {
            it->index += 1;
        }
    }
    else if (will_remove)
    {
        is_modified = true;
        if (to_remove == begin(keys))
        {
            keys.erase(to_remove);
            int first_key_index = begin(keys)->index;
            for (auto it = begin(keys); it != end(keys); ++it)
            {
                it->index -= first_key_index;
            }
        }
        else
        {
            keys.erase(to_remove);
        }
    }


    // Preview the color gradient
    std::vector<sf::Color> colors = gen.get_colors();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (draw_list->VtxBuffer.Size + colors.size() > max_colors)
    {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Can't display preview: too many colors");
    }
    else
    {
        ImVec2 screen_pos = ImGui::GetCursorScreenPos();
        ImVec2 window_pos = ImGui::GetCursorPos();
        float space_until_border = ImGui::GetWindowWidth() - window_pos.x - 30.f;
        float xsize = (space_until_border < 400) ? space_until_border : 400;
        ImVec2 size {xsize, 30.};
        float checker_box_size = 10.f;
        ImGui::RenderColorRectWithAlphaCheckerboard(
            ImVec2(screen_pos.x, screen_pos.y),
            ImVec2(screen_pos.x + size.x, screen_pos.y + size.y),
            IM_COL32(0, 0, 0, 0),
            checker_box_size,
            ImVec2(0, 0));
        float x = screen_pos.x;
        double width = size.x / colors.size();
        for (const auto& color : colors)
        {
            draw_list->AddRectFilled({x, screen_pos.y},
                                     ImVec2(x + width, screen_pos.y + size.y),
                                     IM_COL32(color.r, color.g, color.b, color.a));
            x += width;
        }
        ImGui::Dummy(size);
    }

    if (is_modified && keys.back().index < max_colors)
    {
        gen.set_keys(keys);
    }
}
} // namespace
namespace procgui
{
void interact_with(colors::ColorGeneratorWrapper& color_wrapper,
                   const std::string& /*unused*/,
                   color_wrapper_mode mode)
{
    // Always call this function in a predefined window.
    Expects(embedded_level > 0);

    auto generator = color_wrapper.unwrap();

    // Represents the index of the next ListBox. Set by inspecting the
    // polyphormism.
    int index = 0;
    const auto& concrete = *generator; // To avoid typeid evaluation clang warning
    const auto& info = typeid(concrete).hash_code();
    if (info == typeid(colors::ConstantColor).hash_code())
    {
        index = 0;
    }
    else if (info == typeid(colors::LinearGradient).hash_code())
    {
        index = 1;
    }
    else if (info == typeid(colors::DiscreteGradient).hash_code())
    {
        index = 2;
    }
    else
    {
        Expects(false);
    }

    bool new_generator = false;
    int old_index = index;
    if (mode == color_wrapper_mode::CONSTANT)
    {
        std::array<const char* const, 1> generators {"Constant"};
        new_generator = ImGui::ListBox("Color Generator", &index, generators.data(), 1)
                        && index != old_index;
        ;
    }
    else if (mode == color_wrapper_mode::GRADIENTS)
    {
        --index;
        --old_index; // Change index to match the ListBox.
        std::array<const char* const, 2> generators {"Linear Gradient", "Discrete Gradient"};
        new_generator = ImGui::ListBox("Color Generator", &index, generators.data(), 2)
                        && index != old_index;
        ;
        ++index; // Change index to match the typeid indices.
    }
    else if (mode == color_wrapper_mode::ALL)
    {
        std::array<const char* const, 3> generators {"Constant",
                                                     "Linear Gradient",
                                                     "Discrete Gradient"};
        new_generator = ImGui::ListBox("Color Generator", &index, generators.data(), 3)
                        && index != old_index;
        ;
    }
    else
    {
        Expects(false);
    }

    // Create a new ColorGenerator
    if (new_generator)
    {
        if (index == 0)
        {
            generator = std::make_shared<colors::ConstantColor>();
        }
        else if (index == 1)
        {
            generator = std::make_shared<colors::LinearGradient>();
        }
        else if (index == 2)
        {
            generator = std::make_shared<colors::DiscreteGradient>();
        }
        else
        {
            Ensures(false);
        }
        // Updates ColorGeneratorWrapper and VertexPainter and a 'notify()'
        // waterfall.
        color_wrapper.wrap(generator);
    }

    // Does not use embedded_level, the generator will be displayed just
    // after the generator selection.
    if (index == 0)
    {
        auto constant = std::dynamic_pointer_cast<colors::ConstantColor>(generator);
        ::interact_with(*constant);
    }
    else if (index == 1)
    {
        auto gradient = std::dynamic_pointer_cast<colors::LinearGradient>(generator);
        ::interact_with(*gradient);
    }
    else if (index == 2)
    {
        auto discrete = std::dynamic_pointer_cast<colors::DiscreteGradient>(generator);
        ::interact_with(*discrete);
    }
    else
    {
        Ensures(false);
    }
}

void interact_with_global_parameters(bool& box_is_visible)
{
    const std::string name = "Application parameters";
    if (!set_up(name))
    {
        return;
    }

    // Select background color.
    ImVec4 imcolor = sfml_window::background_color;
    std::array<float, 4> color {imcolor.x, imcolor.y, imcolor.z, imcolor.w};
    if (ImGui::ColorEdit4("Background Color",
                          color.data(),
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel
                              | ImGuiColorEditFlags_NoAlpha))
    {
        sfml_window::background_color = {color[0], color[1], color[2], color[3]};
        ;
    }
    ImGui::SameLine();
    ImGui::Text("Background Color");

    ImGui::Checkbox("LSystem's box visibility", &box_is_visible);

    // Maximum size of complete L-System. Part of the configuration file.
    static constexpr drawing::Matrix::number max_size_limit = 1024 * 1024;   // 1 TiB
    drawing::Matrix::number max_size = config::sys_max_size / (1024 * 1024); // -->MiB
    if (ext::ImGui::InputUnsignedLongLong(
            "Maximum size in memory of L-System before warning, in MegaBytes",
            &max_size))
    {
        if (max_size == 0)
        {
            max_size = 1;
        }
        else if (max_size > max_size_limit)
        {
            // Avoid too big numbers but also hacky way to limit the size
            // explosion for negative number underflow.
            // A patch for imgui for InputScalar would be ideal.
            max_size = max_size_limit;
        }

        config::sys_max_size = max_size * 1024 * 1024; // --> Bytes
    }


    conclude();
}


void interact_with(LSystemView& lsys_view, const std::string& name, bool is_modified, bool* open)
{
    if ((open != nullptr) && !(*open))
    {
        return;
    }

    // To avoid collision with other window of the same name, create a
    // unique ID for the created window. We override the mecanism of
    // same name ==> same window for the LSystemView which is a unique
    // window for a system.
    std::stringstream str_ID;
    str_ID << lsys_view.get_id();
    std::stringstream window_name;
    window_name << name << (is_modified ? "*" : "") << "###"
                << str_ID.str(); // Each window of LSystemView is conserved by its id.
    if (embedded_level == 0)
    {
        // Make the window appear at the mouse double-click position with a
        // correct size.
        // Warning: lots of arbitrary values.
        // Shift the window next to the LSystemView and shift it again for
        // the window position always appearing on-screen in its entirety.
        auto sfml_window_size = sfml_window::window.getSize();
        int sfml_windowX = sfml_window_size.x;
        int sfml_windowY = sfml_window_size.y;
        sf::Vector2f next_window_size {500, 600};
        next_window_size.x = next_window_size.x > sfml_window_size.x ? sfml_window_size.x
                                                                     : next_window_size.x;
        next_window_size.y = next_window_size.y > sfml_window_size.y ? sfml_window_size.y
                                                                     : next_window_size.y;
        ImGui::SetNextWindowSize(next_window_size, ImGuiSetCond_Appearing);


        sf::Vector2i pos = controller::WindowController::get_mouse_position();
        auto bounding_box = lsys_view.get_bounding_box();

        // Shift the window to the right.
        auto absolute_right_side = controller::WindowController::absolute_mouse_position(
            {bounding_box.left + bounding_box.width, 0});
        pos.x = absolute_right_side.x + 50;

        // If the window would be out of the current screen, shift it to the left.
        if (pos.x + next_window_size.x > sfml_windowX)
        {
            auto absolute_left_side = controller::WindowController::absolute_mouse_position(
                {bounding_box.left, 0});
            pos.x = absolute_left_side.x - next_window_size.x - 50;

            // If the window is still out of screen, shift it to the border.
            if (pos.x < 0)
            {
                pos.x = 0;
            }
        }
        pos.y -= 150;

        // If the window is too far up or down, shift it down or up.
        pos.y = pos.y < 0 ? 0 : pos.y;
        pos.y = pos.y + next_window_size.y > sfml_windowY ? sfml_windowY - next_window_size.y
                                                          : pos.y;

        ImGui::SetNextWindowPos(sf::Vector2i {pos.x, pos.y}, ImGuiSetCond_Appearing);

        // The window's title background is set to the unique color
        // associated with the 'lsys_view_'.
        auto color = lsys_view.get_color();
        ImGui::PushStyleColor(ImGuiCol_TitleBg,
                              static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive,
                              static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));
    }
    if (!set_up(window_name.str(), open))
    {
        if (embedded_level == 0)
        {
            ImGui::PopStyleColor(2);
        }
        // Early out if the display zone is collapsed.
        return;
    }

    push_embedded();
    interact_with(lsys_view.ref_parameters(), "Drawing Parameters##" + str_ID.str());
    interact_with(lsys_view.ref_lsystem_buffer(), "LSystem##" + str_ID.str());
    interact_with(lsys_view.ref_interpretation_buffer(), "Interpretation Map##" + str_ID.str());
    interact_with(lsys_view.ref_vertex_painter_wrapper(), "Painter##" + str_ID.str());

    bool bounding_box_visibility = lsys_view.box_is_visible();
    interact_with_global_parameters(bounding_box_visibility);
    lsys_view.set_box_visibility(bounding_box_visibility);
    pop_embedded();

    conclude();

    if (embedded_level == 0)
    {
        ImGui::PopStyleColor(2);
    }
}
} // namespace procgui
