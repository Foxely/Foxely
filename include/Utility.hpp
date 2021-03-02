
#ifndef _FOXELY_UTILITY_HPP_
#define _FOXELY_UTILITY_HPP_

#include <string>
#include <fstream>
#include <sstream>

    /**
    * @brief Read file
    * @param strFile the path of the file
    * @param[out] strOutReadContent the content of the file will be set in this variable
    * @return true if the read success, otherwise false
    */
    bool ReadFile(const std::string& strFile, std::string& strOutReadContent);
#endif