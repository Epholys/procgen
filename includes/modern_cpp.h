#ifndef MODERN_CPP_H
#define MODERN_CPP_H

#ifdef _WIN32
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif



#endif // MODERN_CPP_H
