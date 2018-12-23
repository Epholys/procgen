#include <cctype>
#include <cstring>
#include <tuple>
#include <chrono>
#include "procgui.h"
#include "helper_string.h"
#include "WindowController.h"
#include "RenderWindow.h"
#include "VertexPainterRadial.h"
#include "VertexPainterRandom.h"
#include "VertexPainterBlockRandom.h"
#include "VertexPainterSequential.h"
#include "VertexPainterIteration.h"
#include "VertexPainterComposite.h"
#include "VertexPainterConstant.h"

using namespace math;

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
    bool set_up(const std::string& name,
                bool* open = nullptr)
    {
        // If the data is not embedded, creates a new window.
        if (embedded_level == 0)
        {
            bool is_active = ImGui::Begin(name.c_str(), open, ImGuiWindowFlags_NoSavedSettings);
            if(!is_active)
            {
                // Window is collapsed, call End();
                ImGui::End();
            }
            return is_active;
        }
        // Otherwise, set up a CollapsingHeader
        else
        {
            bool is_active = ImGui::CollapsingHeader(name.c_str());
            if(is_active)
            {
                // Avoid name collision between two widgets (like '+' button).
                ImGui::PushID(name.c_str());
                ImGui::Indent();
            }
            return is_active;
        }
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
}

namespace ext::ImGui
{
    // Taken from 'imgui_demo.cpp', helper function to display a help tooltip.
    void ShowHelpMarker(const char* desc)
    {
        ::ImGui::TextDisabled("(?)");
        if (::ImGui::IsItemHovered())
        {
            ::ImGui::BeginTooltip();
            ::ImGui::PushTextWrapPos(450.0f);
            ::ImGui::TextUnformatted(desc);
            ::ImGui::PopTextWrapPos();
            ::ImGui::EndTooltip();
        }
    }

    // Taken from 'imgui_demo.cpp': the styles for colored buttons with a style
    // close to the default button.
    // WARNING: Use these with 'ImGui::PopStyleColor(3)'.
    template<int hue> void PushStyleColoredButton()
    {
        ::ImGui::PushStyleColor(::ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.6f, 0.6f)));
        ::ImGui::PushStyleColor(::ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.7f, 0.7f)));
        ::ImGui::PushStyleColor(::ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(hue/7.0f, 0.8f, 0.8f)));
    }
    auto PushStyleRedButton = PushStyleColoredButton<0>;
    auto PushStyleYellowButton = PushStyleColoredButton<1>;
    auto PushStyleGreenButton = PushStyleColoredButton<2>;
    auto PushStyleTurquoiseButton = PushStyleColoredButton<3>;
    auto PushStyleBlueButton = PushStyleColoredButton<4>;
    auto PushStylePurpleButton = PushStyleColoredButton<5>;
    auto PushStylePinkButton = PushStyleColoredButton<6>;

    bool DragDouble(const char* label, double* v, double v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3f", double power = 1.0f)
    {
        double* min = &v_min;
        double* max = &v_max;
        return ::ImGui::DragScalarN(label, ::ImGuiDataType_Double, v, 1, v_speed, min, max, format, power);
    }
    bool DragDouble2(const char* label, double v[2], double v_speed = 1.0f, double v_min = 0.0f, double v_max = 0.0f, const char* format = "%.3f", double power = 1.0f)
    {
        double* min = &v_min;
        double* max = &v_max;
        return ::ImGui::DragScalarN(label, ::ImGuiDataType_Double, v, 2, v_speed, min, max, format, power);
    }
}

namespace procgui
{
    void display(const LSystem& lsys, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        // --- Axiom ---
        ImGui::Text("Axiom:");

        ImGui::Indent();

        ImGui::Text(lsys.get_axiom().c_str());

        ImGui::Unindent(); 


        // --- Production Rules ---
        ImGui::Text("Production rules:");

        ImGui::Indent(); 
        for (const auto& rule : lsys.get_rules())
        {
            std::ostringstream oss;
            oss << rule.first << " -> " << rule.second;
            std::string str = oss.str();
            ImGui::Text(str.c_str());
        }
        ImGui::Unindent(); 

        // --- Iteration Predecessors ---
        ImGui::Text("Iteration predecessors:");

        ImGui::Indent();

        ImGui::Text(lsys.get_iteration_predecessors().c_str());

        ImGui::Unindent(); 

        
        conclude();
    }

    
    void display(const drawing::DrawingParameters& parameters, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out if the display zone is collapsed.
            return;
        }
    
        // Arbitrary value to align neatly every members.
        const int align = 150;   

        // --- Starting Position ---
        ImGui::Text("Starting Position:"); ImGui::SameLine(align);
        ImGui::Text("x: %.f", parameters.get_starting_position().x); ImGui::SameLine();
        ImGui::Text("y: %.f", parameters.get_starting_position().y);

        // --- Starting Angle ---
        ImGui::Text("Starting Angle:"); ImGui::SameLine(align);
        ImGui::Text("%.lf", math::rad_to_degree(parameters.get_starting_angle())); ImGui::SameLine();
        ImGui::Text("degree");
            
        // --- Angle Delta ---
        ImGui::Text("Angle Delta:"); ImGui::SameLine(align);
        ImGui::Text("%.lf", math::rad_to_degree(parameters.get_delta_angle())); ImGui::SameLine();
        ImGui::Text("degree");

        // --- Step ---
        ImGui::Text("Step:"); ImGui::SameLine(align);
        ImGui::Text("%.1lf", parameters.get_step());

        conclude();
    }

    void display(const drawing::InterpretationMap& map, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out if the display zone is collapsed.
            return;
        }

        for(auto interpretation : map.get_rules())
        {
            std::string name = interpretation.second.name;
            ImGui::Text("%c -> %s", interpretation.first, name.c_str());

        }
        
        conclude();
    }

    

    void interact_with(drawing::DrawingParameters& parameters, const std::string& name)
    {
        if( !set_up(name) )
        {
            return;
        }
    
        // --- Starting position ---
        double pos[2] = { parameters.get_starting_position().x,
                         parameters.get_starting_position().y };
        if (ext::ImGui::DragDouble2("Starting position", pos,
                               1.f, 0.f, 0.f, "%.lf") )
        {
            // is_modified_ is not set: the render state take care of translating the view.
            ext::sf::Vector2d starting_position {pos[0], pos[1]};
            parameters.set_starting_position(starting_position);
        }

        // --- Starting angle ---
        double starting_angle_deg = math::rad_to_degree(parameters.get_starting_angle());
        if (ext::ImGui::DragDouble("Starting Angle", &starting_angle_deg,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            parameters.set_starting_angle(math::degree_to_rad(starting_angle_deg));
        }

        // --- Angle Delta ---
        double delta_angle_deg = math::rad_to_degree(parameters.get_delta_angle());
        if (ext::ImGui::DragDouble("Angle Delta", &delta_angle_deg,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            parameters.set_delta_angle(math::degree_to_rad(delta_angle_deg));
        }

        // --- Step ---
        double step = parameters.get_step();
        if(ext::ImGui::DragDouble("Step", &step, 0.2f, 0.f, 0.f, "%#.1lf"))
        {
            parameters.set_step(step);
        }

        // --- Iterations ---
        // Arbitrary value to avoid resource depletion happening with higher
        // number of iterations (several GiB of memory usage and huge CPU
        // load).
        const int n_iter_max = 10;
        int n_iter = parameters.get_n_iter();
        if(ImGui::SliderInt("Iterations", &n_iter, 0, n_iter_max))
        {
            parameters.set_n_iter(n_iter);
        }
        ImGui::SameLine(); ext::ImGui::ShowHelpMarker("CTRL+click and click to directly input values. Higher values will use all of your memory and CPU");

        conclude();

    }

    void interact_with(LSystemBuffer& buffer, const std::string& name)
    {
        if( !set_up(name) )
        {
            return;
        }

        // The LSystem itelf
        LSystem& lsys = *buffer.get_target();
        
        // --- Axiom ---
        auto buf = string_to_array<lsys_successor_size>(lsys.get_axiom());
                        
        if (ImGui::InputText("Axiom", buf.data(), lsys_successor_size))
        {
            lsys.set_axiom(array_to_string(buf));
        }

        //  --- Rules ---
        // [ predecessor ] -> [ successor ] [-] (remove rule) | [+] (add rule)

        ImGui::Text("Production rules:");
        interact_with_buffer(buffer,
           [&buffer](auto it)
            {
                // Interact with the successor. Except for the input size, does not
                // have any constraints.
                auto array = string_to_array<lsys_successor_size>(it->successor);
                if(ImGui::InputText("##succ", array.data(), lsys_successor_size))
                {
                    buffer.delayed_change_successor(it, array_to_string(array));
                    return true;
                }
                return false;
            });

        // --- Iteration Predecessors ---
        buf = string_to_array<lsys_successor_size>(lsys.get_iteration_predecessors());
        if (ImGui::InputText("Iteration predecessors", buf.data(), lsys_successor_size))
        {
            lsys.set_iteration_predecessors(array_to_string(buf));
        }
        
        conclude();
    }

    void interact_with(InterpretationMapBuffer& buffer, const std::string& name)
    {
        if( !set_up(name) )
        {
            // Early out
            return;
        }

        using namespace drawing;
        
        interact_with_buffer(buffer,
            [&buffer](auto it)
            {
                // ImGui::ListBox needs:
                //   - An array of 'char *' for the different elements
                //   - An index to select between these elements

                // As 'all_orders_name' has the exact same order as
                // 'all_orders', the index is common.
                // 
                // The index is calculated by finding in the vector the order
                // and using the distance between the first element and the
                // current one.
                auto selected_interpretation_it = std::find(all_orders.begin(),
                                                            all_orders.end(),
                                                            it->successor);
                int index = std::distance(all_orders.begin(), selected_interpretation_it);
                if(ImGui::ListBox("##order", &index, all_orders_name.data(), all_orders_name.size()))
                {
                    buffer.delayed_change_successor(it, all_orders.at(index));
                    return true;
                }
                return false;
            });

        conclude();
    }
}
namespace
{
    void interact_with(colors::VertexPainterConstant& painter, bool from_composite=false)
    {
        if (!from_composite)
        {
            ::procgui::interact_with(*painter.get_generator_buffer(), "Colors",
                                     ::procgui::color_buffer_mode::CONSTANT);
        }
    }
    void interact_with(colors::VertexPainterLinear& painter, bool from_composite=false)
    {
        // --- Gradient angle ---
        double angle = painter.get_angle();
        if (ext::ImGui::DragDouble("Gradient Angle", &angle,
                              1.f, 0.f, 360.f, "%.lf") )
        {
            painter.set_angle(angle);
        }

        if (!from_composite)
        {
            ::procgui::interact_with(*painter.get_generator_buffer(), "Colors",
                                     ::procgui::color_buffer_mode::GRADIENTS);
        }
    }

    void interact_with(colors::VertexPainterRadial& painter, bool from_composite=false)
    {
        // --- Center ---
        float center[2] = {painter.get_center().x, painter.get_center().y};
        if (ImGui::DragFloat2("Circle Center", center,
                              0.01f, 0.f, 1.f, "%.2f") )
        {
            painter.set_center(sf::Vector2f(center[0], center[1]));
        }
        
        if (!from_composite)
        {
            ::procgui::interact_with(*painter.get_generator_buffer(), "Colors",
                                     ::procgui::color_buffer_mode::GRADIENTS);
        }
    }

    void interact_with(colors::VertexPainterRandom& painter, bool from_composite=false)
    {
        ext::ImGui::PushStyleGreenButton();
        if (ImGui::Button("Randomize"))
        {
            painter.randomize();
        }
        ImGui::PopStyleColor(3);
        if (!from_composite)
        {
            ::procgui::interact_with(*painter.get_generator_buffer(), "Colors",
                                     ::procgui::color_buffer_mode::GRADIENTS);
        }
    }

    void interact_with(colors::VertexPainterBlockRandom& painter, bool from_composite=false)
    {
        int block_size = painter.get_block_size();
        if (ImGui::DragInt("Block size", &block_size, 1, 1, std::numeric_limits<int>::max()))
        {
            painter.set_block_size(block_size);
        }
        
        if (!from_composite)
        {
            ::procgui::interact_with(*painter.get_generator_buffer(), "Colors",
                                     ::procgui::color_buffer_mode::GRADIENTS);
        }
    }
    
    void interact_with(colors::VertexPainterSequential& painter, bool from_composite=false)
    {
        float angle = painter.get_factor();
        if (ImGui::DragFloat("Repetition factor", &angle,
                             0.01f, 0.f, std::numeric_limits<float>::max(), "%.2f") )
        {
            painter.set_factor(angle);
        }
            
        if (!from_composite)
        {
            ::procgui::interact_with(*painter.get_generator_buffer(), "Colors",
                                     ::procgui::color_buffer_mode::GRADIENTS);
        }
    }

    void interact_with(colors::VertexPainterIteration& painter, bool from_composite=false)
    {
        if (!from_composite)
        {
            ::procgui::interact_with(*painter.get_generator_buffer(), "Colors",
                                     ::procgui::color_buffer_mode::GRADIENTS);
        }
    }

    void interact_with(colors::VertexPainterComposite& painter)
    {
        procgui::interact_with(*painter.get_main_painter(), "Slave Painter", true);

        int index = 0;
        auto child_painters = painter.get_child_painters();
        auto to_remove = end(child_painters);
        bool add_new_painter = false;
        bool add_copied_painter = false;
        auto to_add = end(child_painters);

        ::ext::ImGui::PushStyleGreenButton();
        if (ImGui::Button("Add Painter here"))
        {
            to_add = begin(child_painters);
            add_new_painter = true;
        }
        ImGui::PopStyleColor(3);

        ::ext::ImGui::PushStyleTurquoiseButton();
        if (colors::VertexPainterComposite::has_copied_painter() && ImGui::Button("Paste copied Painter here"))
        {
            ImGui::SameLine();
            to_add = begin(child_painters);
            add_copied_painter = true;
        }
        ImGui::PopStyleColor(3);

        
        for (auto it = begin(child_painters); it != end(child_painters); ++it)
        {
            ImGui::PushID(index);
            
            push_embedded();
            ::procgui::interact_with(**it, "", false /* is_slave_of_composite */);
            pop_embedded();

            ::ext::ImGui::PushStyleGreenButton();
            if (ImGui::Button("Add Painter here"))
            {
                to_add = next(it);
                add_new_painter = true;
            }
            ImGui::PopStyleColor(3);

            if (it != begin(child_painters))
            {
                ImGui::SameLine();
                ::ext::ImGui::PushStyleRedButton();
                if (ImGui::Button("Remove previous Painter"))
                {
                    to_remove = it;
                }
                ImGui::PopStyleColor(3);
            }

            ::ext::ImGui::PushStylePurpleButton();
            if (ImGui::Button("Copy previous Painter"))
            {
                colors::VertexPainterComposite::save_painter((*it)->get_target());
            }
            ImGui::PopStyleColor(3);

            if (colors::VertexPainterComposite::has_copied_painter())
            {
                ImGui::SameLine();
                ::ext::ImGui::PushStyleTurquoiseButton();
                if(ImGui::Button("Paste copied Painter here"))
                {
                    to_add = next(it);
                    add_copied_painter = true;
                }
                ImGui::PopStyleColor(3);
            }
            
            ImGui::Separator();
            ImGui::PopID();
            index++;
        }
        
        if (to_remove != end(child_painters))
        {
            child_painters.erase(to_remove);
            painter.set_child_painters(child_painters);
        }
        else if (add_new_painter)
        {
            child_painters.insert(to_add, std::make_shared<colors::VertexPainterBuffer>());
            painter.set_child_painters(child_painters);
        }
        else if (add_copied_painter)
        {
            child_painters.insert(to_add,
                                  std::make_shared<colors::VertexPainterBuffer>(colors::VertexPainterComposite::get_copied_painter()));
            painter.set_child_painters(child_painters);
        }
    }
}
namespace procgui
{
    void interact_with(colors::VertexPainterBuffer& painter_buffer,
                       const std::string& name,
                       bool is_slave_of_composite)
    {
        if (!set_up(name))
        {
            // Early out if the display zone is collapsed.
            return;
        }

        auto painter = painter_buffer.get_painter();
        
        // Represents the index of the next ListBox. Set by inspecting the
        // polyphormism. 
        int index = 0;
        const auto& info = typeid(*painter).hash_code();
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
        else if (info == typeid(colors::VertexPainterBlockRandom).hash_code())
        {
            index = 4;
        }
        else if (info == typeid(colors::VertexPainterSequential).hash_code())
        {
            index = 5;
        }
        else if (info == typeid(colors::VertexPainterIteration).hash_code())
        {
            index = 6;
        }
        else if (info == typeid(colors::VertexPainterComposite).hash_code())
        {
            index = 7;
        }
        else
        {
            Expects(false);
        }

        bool new_generator = false;
        if (!is_slave_of_composite)
        {
            const char* generators[8] = {"Constant", "Linear", "Radial",
                                         "Random", "BlockRandom", "Sequential",
                                         "Iterative", "Composite"};
            new_generator =  ImGui::ListBox("Vertex Painter", &index, generators, 8);
            
        }
        else 
        {
            const char* generators[7] = {"Constant", "Linear", "Radial",
                                         "Random", "BlockRandom", "Sequential", "Iterative"};
            new_generator = ImGui::ListBox("Vertex Painter", &index, generators, 7);
        }
        
        
        // Create a new VertexPainter
        if (new_generator)
        {
            if (index == 0)
            {
                painter = std::make_shared<colors::VertexPainterConstant>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else if (index == 1)
            {
                painter = std::make_shared<colors::VertexPainterLinear>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else if (index == 2)
            {
                painter = std::make_shared<colors::VertexPainterRadial>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else if (index == 3)
            {
                painter = std::make_shared<colors::VertexPainterRandom>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else if (index == 4)
            {
                painter = std::make_shared<colors::VertexPainterBlockRandom>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else if (index == 5)
            {
                painter = std::make_shared<colors::VertexPainterSequential>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else if (index == 6)
            {
                painter = std::make_shared<colors::VertexPainterIteration>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else if (index == 7)
            {
                painter = std::make_shared<colors::VertexPainterComposite>(painter->get_generator_buffer()->get_generator()->clone());
            }
            else
            {
                Ensures(false);
            }
            // Updates ColorGeneratorBuffer and VertexPainter and a 'notify()'
            // waterfall. 
            painter_buffer.set_painter(painter);
        }

        // Does not use embedded_level, the generator will be displayed just
        // after the generator selection.
        if (index == 0)
        {
            auto constant = std::dynamic_pointer_cast<colors::VertexPainterConstant>(painter);
            ::interact_with(*constant, is_slave_of_composite);
        }
        else if (index == 1)
        {
            auto linear = std::dynamic_pointer_cast<colors::VertexPainterLinear>(painter);
            ::interact_with(*linear, is_slave_of_composite);
        }
        else if (index == 2)
        {
            auto radial = std::dynamic_pointer_cast<colors::VertexPainterRadial>(painter);
            ::interact_with(*radial, is_slave_of_composite);
        }
        else if (index == 3)
        {
            auto random = std::dynamic_pointer_cast<colors::VertexPainterRandom>(painter);
            ::interact_with(*random, is_slave_of_composite);
        }
        else if (index == 4)
        {
            auto block_random = std::dynamic_pointer_cast<colors::VertexPainterBlockRandom>(painter);
            ::interact_with(*block_random, is_slave_of_composite);
        }
        else if (index == 5)
        {
            auto sequential = std::dynamic_pointer_cast<colors::VertexPainterSequential>(painter);
            ::interact_with(*sequential, is_slave_of_composite);
        }
        else if (index == 6)
        {
            auto iteration = std::dynamic_pointer_cast<colors::VertexPainterIteration>(painter);
            ::interact_with(*iteration, is_slave_of_composite);
        }
        else if (index == 7)
        {
            auto composite = std::dynamic_pointer_cast<colors::VertexPainterComposite>(painter);
            ::interact_with(*composite);
        }
        else
        {
            Ensures(false);
        }

        conclude();

    }
}
namespace
{
    void interact_with(colors::ConstantColor& constant)
    {
        // Color selection widget.
        ImVec4 imcolor = constant.get_color();
        if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            constant.set_color(imcolor);
        }

        // Display the ConstantColor preview.
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size {500., 30.};
        draw_list->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x+size.x, pos.y+size.y),
                                 ImGui::ColorConvertFloat4ToU32(imcolor));
        ImGui::Dummy(size);
    }

    void interact_with(colors::LinearGradient& gen)
    {
        bool is_modified = false;
        
        auto keys = gen.get_raw_keys();

        // Modify 'gen''s keys: colors and position
        for (unsigned i=0; i<keys.size(); ++i)
        {
            ImGui::PushID(i);
            ImGui::BeginGroup();

            // Color
            auto& sfcolor = keys.at(i).first;
            ImVec4 imcolor = sfcolor;
            if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
            {
                is_modified = true;
            }
            sfcolor = imcolor;

            // Position
            ImGui::PushItemWidth(50);
            if(ImGui::DragFloat("", &keys.at(i).second, 0.01, 0., 1., "%.2f"))
            {
                is_modified = true;
            }
            ImGui::PopItemWidth();

            ImGui::EndGroup();
            ImGui::PopID();
            ImGui::SameLine();
        }

        // Button '+' to add a key.
        ext::ImGui::PushStyleGreenButton();
        if (ImGui::Button("+"))
        {
            is_modified = true;
            keys.push_back({sf::Color::White, 1.f});
        }
        ImGui::PopStyleColor(3);

        // Button '-' to remove a key
        ext::ImGui::PushStyleRedButton();
        if (keys.size() > 2 && (ImGui::SameLine(), ImGui::Button("-")))
        {
            is_modified = true;
            keys.pop_back();
        }
        ImGui::PopStyleColor(3);
        
        if (is_modified)
        {
            gen.set_keys(keys);
        }

        // Preview the color gradient
        auto k = gen.get_sanitized_keys();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size {500., 30.};
        float x = pos.x;
        double ratio = 0.f;
        for (unsigned i=0; i<k.size()-1; ++i)
        {
            const auto& col1 = k.at(i).first;
            const auto& col2 = k.at(i+1).first;
            const auto& f = k.at(i+1).second;
            draw_list->AddRectFilledMultiColor({x, pos.y}, ImVec2(x+size.x*(f-ratio), pos.y+size.y),
                                               IM_COL32(col1.r, col1.g, col1.b, col1.a),
                                               IM_COL32(col2.r, col2.g, col2.b, col2.a),
                                               IM_COL32(col2.r, col2.g, col2.b, col2.a),
                                               IM_COL32(col1.r, col1.g, col1.b, col1.a));
            ratio = f;
            x = pos.x + size.x*f;
        }

        ImGui::Dummy(size);
    }

    void interact_with(colors::DiscreteGradient& gen)
    {
        bool is_modified = false;
        
        auto keys = gen.get_keys();

        // Modify 'gen''s keys: key's color and transitional colors between
        // these keys.

        // If the user add or remove a transitional color, it have to offset all
        // next keys' position. 'modifier' conserve the offset's value.
        // The last color is managed at the end, to not add the transitional
        // colors part.
        int modifier = 0;
        for (unsigned i=0; i<keys.size()-1; ++i)
        {
            ImGui::PushID(i);

            // Key Color
            auto& sfcolor = keys.at(i).first;
            ImVec4 imcolor = sfcolor;
            if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
            {
                is_modified = true;
            }
            sfcolor = imcolor;

            ImGui::SameLine();
            ImGui::BeginGroup();

            // Number of transitional colors.
            int diff = keys.at(i+1).second - keys.at(i).second - 1;
            ImGui::Text(std::to_string(diff).c_str());

            // '+' button to add a transitional color.
            ext::ImGui::PushStyleGreenButton();
            if (ImGui::Button("+"))
            {
                is_modified = true;
                ++modifier;
            }
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            // '-' button to remove a transitional color
            ext::ImGui::PushStyleRedButton();
            if (diff > 0 && ImGui::Button("-"))
            {
                is_modified = true;
                --modifier;
            }
            ImGui::PopStyleColor(3);

            // Offset the current key.
            keys.at(i+1).second += modifier;

            ImGui::EndGroup();
            ImGui::PopID();
            ImGui::SameLine();
        }

        // Last color widget.
        ImGui::PushID(keys.size());
        auto& sfcolor = keys.back().first;
        ImVec4 imcolor = sfcolor;
        if(ImGui::ColorEdit4("Color", (float*)&imcolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf))
        {
            is_modified = true;
        }
        sfcolor = imcolor;
        ImGui::PopID();

        ImGui::SameLine();
        // Button '+' to add a key.
        ext::ImGui::PushStyleGreenButton();
        if (ImGui::Button("+"))
        {
            is_modified = true;
            keys.push_back({sf::Color::White, keys.back().second+1});
        }
        ImGui::PopStyleColor(3);

        // Button '-' to remove a key
        ext::ImGui::PushStyleRedButton();
        if (keys.size() > 1 && (ImGui::SameLine(), ImGui::Button("-")))
        {
            is_modified = true;
            keys.pop_back();
        }
        ImGui::PopStyleColor(3);
        
        // Preview the color gradient
        std::vector<sf::Color> colors = gen.get_colors();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 size {500., 30.};
        float x = pos.x;
        double width = size.x / colors.size();
        for (const auto& color : colors)
        {
            draw_list->AddRectFilled({x, pos.y}, ImVec2(x+width, pos.y+size.y), IM_COL32(color.r, color.g, color.b, color.a));
            x += width;
        }
        ImGui::Dummy(size);

        if (is_modified)
        {
            gen.set_keys(keys);
        }
    }
} // namespace 
namespace procgui
{ 
    void interact_with(colors::ColorGeneratorBuffer& color_buffer,
                       const std::string&,
                       color_buffer_mode mode)
    {
       // Always call this function in a predefined window.
        Expects(embedded_level > 0);

        auto gen = color_buffer.get_generator();
        
        // Represents the index of the next ListBox. Set by inspecting the
        // polyphormism. 
        int index = 0;
        const auto& info = typeid(*gen).hash_code();
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
        if (mode == color_buffer_mode::CONSTANT)
        {
            const char* generators[1] = {"Constant"};
            new_generator = ImGui::ListBox("Color Generator", &index, generators, 1);
        }
        else if (mode == color_buffer_mode::GRADIENTS)
        {
            --index;
            const char* generators[2] = {"Linear Gradient", "Discrete Gradient"};
            new_generator = ImGui::ListBox("Color Generator", &index, generators, 2);
            ++index;
        }
        else if (mode == color_buffer_mode::ALL)
        {
            const char* generators[3] = {"Constant", "Linear Gradient", "Discrete Gradient"};
            new_generator = ImGui::ListBox("Color Generator", &index, generators, 3);
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
                gen = std::make_shared<colors::ConstantColor>();
            }
            else if (index == 1)
            {
                gen = std::make_shared<colors::LinearGradient>();
            }
            else if (index == 2)
            {
                gen = std::make_shared<colors::DiscreteGradient>();

            }
            else
            {
                Ensures(false);
            }
            // Updates ColorGeneratorBuffer and VertexPainter and a 'notify()'
            // waterfall. 
            color_buffer.set_generator(gen);
        }

        // Does not use embedded_level, the generator will be displayed just
        // after the generator selection.
        if (index == 0)
        {
            auto constant = std::dynamic_pointer_cast<colors::ConstantColor>(gen);
            ::interact_with(*constant);
        }
        else if (index == 1)
        {
            auto gradient = std::dynamic_pointer_cast<colors::LinearGradient>(gen);
            ::interact_with(*gradient);
        }
        else if (index == 2)
        {
            auto discrete = std::dynamic_pointer_cast<colors::DiscreteGradient>(gen);
            ::interact_with(*discrete);
        }
        else
        {
            Ensures(false);
        }
    }

    void interact_with(LSystemView& lsys_view, const std::string& name, bool* open)
    {
        if (open && !(*open))
        {
            return;
        }

        // To avoid collision with other window of the same name, create a
        // unique ID for the created window. We override the mecanism of
        // same name ==> same window for the LSystemView which is a unique
        // window for a system.
        std::stringstream ss;
        ss << "##" << lsys_view.get_id(); // Each window of LSystemView
                                          // is conserved by its id.
        if (embedded_level == 0)
        {
            // Make the window appear at the mouse double-click position with a
            // correct size.
            // Warning: lots of arbitrary values.
            // Shift the window next to the LSystemView and shift it again for
            // the window position always appearing on-screen in its entirety. 
            int windowX = window::window_size.x;
            int windowY = window::window_size.y;
            sf::Vector2i pos = controller::WindowController::get_mouse_position();
            auto bounding_box = lsys_view.get_bounding_box();

            // Shift the window to the right.
            auto absolute_right_side = controller::WindowController::absolute_mouse_position({bounding_box.left + bounding_box.width,0});
            pos.x = absolute_right_side.x + 50;

            // If the window would be out of the current screen, shift it to the left.
            if (pos.x + 500 > windowX)
            {
                auto absolute_left_side = controller::WindowController::absolute_mouse_position({bounding_box.left,0});
                pos.x = absolute_left_side.x - 550;                
            }
            pos.y -= 150;

            // If the window is too far up or down, shift it down or up.
            pos.y = pos.y < 0 ? 0 : pos.y;
            pos.y = pos.y + 450 > windowY ? windowY-450 : pos.y;
            ImGui::SetNextWindowPos(sf::Vector2i{pos.x,pos.y}, ImGuiSetCond_Appearing);
            ImGui::SetNextWindowSize({500,450}, ImGuiSetCond_Appearing);

            // The window's title background is set to the unique color
            // associated with the 'lsys_view_'.
            auto color = lsys_view.get_color();
            ImGui::PushStyleColor(ImGuiCol_TitleBg,
                                  static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive,
                                  static_cast<ImVec4>(ImColor(color.r, color.g, color.b)));

        }
        if (!set_up(name+ss.str(), open))
        {
            if (embedded_level == 0)
            {
                ImGui::PopStyleColor(2);
            }
            // Early out if the display zone is collapsed.
            return;
        }

        push_embedded();
        interact_with(lsys_view.ref_parameters(), "Drawing Parameters"+ss.str());
        interact_with(lsys_view.ref_lsystem_buffer(), "LSystem"+ss.str());
        interact_with(lsys_view.ref_interpretation_buffer(), "Interpretation Map"+ss.str());
        interact_with(lsys_view.ref_vertex_painter_buffer(), "Painter");
        pop_embedded();

        conclude();

        if (embedded_level == 0)
        {
            ImGui::PopStyleColor(2);
        }
    }
}
