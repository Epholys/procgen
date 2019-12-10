#ifndef CONFIG_H
#define CONFIG_H


#include <experimental/filesystem>
#include "cereal/cereal.hpp"
#include "size_computer.h"

// Manages the config file containing persistent global variables.
namespace config
{
    // The max limit of bytes for L-Systems. This is a default number, each
    // L-System can override it if the user allows so.
    extern drawing::Matrix::number sys_max_size;  // in bytes

    // The configuration file path.
    static std::experimental::filesystem::path config_path = std::experimental::filesystem::u8path(u8"config/config.json");

    // Serialization
    // 'sys_max_size' is saved in Megabytes.
    template <class Archive>
    void save(Archive& ar, u32)
    {
        ar(cereal::make_nvp("sys_max_size", sys_max_size / (1024*1024)));
    }
    template <class Archive>
    void load(Archive& ar, u32)
    {
        ar(cereal::make_nvp("sys_max_size", sys_max_size));
        if (sys_max_size <= 0)
        {
            sys_max_size = 10;
        }
        sys_max_size *= 1024 * 1024;
    }
}


#endif // CONFIG_H
