#ifndef CONFIG_H
#define CONFIG_H


#include <filesystem>
#include "cereal/cereal.hpp"
#include "size_computer.h"

namespace config
{
    extern drawing::Matrix::number sys_max_size;  // in bytes

    static std::filesystem::path config_path = std::filesystem::u8path(u8"config/config.json");
    template <class Archive>
    void save(Archive& ar, std::uint32_t)
    {
        // In MiB
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
