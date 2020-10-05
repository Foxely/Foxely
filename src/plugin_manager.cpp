// #include <filesystem>
#include <algorithm>

#include "plugin_manager.h"
#include "scy/pluga/plugin_api.h"
#include "foxely.h"

// namespace fs = std::filesystem;

void PluginManager::GetAllLibrary()
{
    // std::string path = "./modules";
    // for (const auto & entry : fs::recursive_directory_iterator(path))
    // {
    //     if (entry.path().extension() == ".so")
    //     {
    //         try {
    //             // Load the shared library
    //             fox::SharedLibrary lib;
    //             lib.open(entry.path());
    //             lib.m_strLibname = std::string(entry.path().stem().string());

    //             // Get plugin descriptor and exports
    //             // fox::pluga::PluginDetails *info;
    //             // lib.sym("exports", reinterpret_cast<void **>(&info));

    //             // API version checking
    //             // if (info->apiVersion != SCY_PLUGIN_API_VERSION)
    //             //     throw std::runtime_error(util::format(
    //             //         "Plugin version mismatch. Expected %s, got %s.",
    //             //         SCY_PLUGIN_API_VERSION, info->apiVersion));

    //             // Close the plugin and free memory
    //             m_vLibraries.push_back(lib);
    //         } catch (std::exception &exc) {
    //             std::cerr << "Error: " << exc.what() << std::endl;
    //         }
    //     }
    // }
}

bool PluginManager::LoadLibrary(const std::string& name)
{
    // for (auto& it : m_vLibraries)
    // {
    //     if (it.m_strLibname == name)
    //     {
    //         std::vector<fox::SharedLibrary>::iterator itFind = std::find(m_vLibraryImported.begin(), m_vLibraryImported.end(), it);
    //         if (itFind != m_vLibraryImported.end())
    //             return true;

    //         fox::pluga::PluginDetails *info;
    //         it.sym("exports", reinterpret_cast<void **>(&info));

    //         auto plugin = reinterpret_cast<fox::pluga::IModule *>(info->initializeFunc());
    //         NativeMethods methods = plugin->GetMethods();
    //         VM::GetInstance()->DefineLib(plugin->GetClassName(), methods);
    //         m_vLibraryImported.push_back(it);
    //         return true;
    //     }
    // }
    return false;
}