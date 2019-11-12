#ifndef CONFIG_H
#define CONFIG_H


#include <filesystem>
#include "cereal/cereal.hpp"
#include "size_computer.h"

// Manages the config file containing persistent global variables.
namespace config
{
    // The max limit of bytes for L-Systems. This is a default number, each
    // L-System can override it if the user allows so.
    extern drawing::Matrix::number sys_max_size;  // in bytes

    // The configuration file path.
    static std::filesystem::path config_path = std::filesystem::u8path(u8"config/config.json");
    
    // Serialization
    // 'sys_max_size' is saved in Megabytes.
    template <class Archive>
    void save(Archive& ar, std::uint32_t)
    {
        ar(cereal::make_nvp("sys_max_size", sys_max_size / (1024*1024)));
    }
    template <class Archive>
    void load(Archive& ar, std::uint32_t)
    {
        ar(cereal::make_nvp("sys_max_size", sys_max_size));
        sys_max_size *= 1024 * 1024;
    }
}


#endif // CONFIG_H
