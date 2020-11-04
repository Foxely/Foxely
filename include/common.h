#ifndef fox_common_h
#define fox_common_h

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

inline bool IsRepl;
// inline bool DEBUG_TRACE_EXECUTION;
// namespace helper
// {
//     inline bool FileExists(const std::string& file)
//     {
//         struct stat buffer;
//         return (stat(file.c_str(), &buffer) == 0);
//     }

//     inline std::string FileToString(const std::string& file)
//     {
//         std::ifstream fin(file);

//         if (!fin.is_open())
//         {
//             throw std::runtime_error("file not found!");
//         }

//         std::stringstream buffer;
//         buffer << fin.rdbuf() << '\0';
//         return buffer.str();
//     }
// }

#endif