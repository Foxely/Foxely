
#include "Utility.hpp"

bool ReadFile(const std::string& strFile, std::string& strOutReadContent)
{
    std::ifstream oFile(strFile); //open the input file
    if (!oFile.is_open())
        return false;

    std::stringstream strStream;
    strStream << oFile.rdbuf(); //read the file
    strOutReadContent = strStream.str();

    return true;
}