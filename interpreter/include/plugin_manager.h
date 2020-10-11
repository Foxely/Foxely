#pragma once

#include <vector>
#include "scy/pluga/sharedlibrary.h"

class PluginManager
{
public:
	std::vector<fox::SharedLibrary> m_vLibraryImported;
	std::vector<fox::SharedLibrary> m_vLibraries;
    PluginManager() {}
    ~PluginManager()
    {
        for (auto& it : m_vLibraries)
        {
            it.close();
        }
    }

    void GetAllLibrary();
    bool LoadLibrary(const std::string& name);
};

