#ifndef FOX_ModulePlugin_H
#define FOX_ModulePlugin_H


#include "scy/pluga/plugin_api.h"

/// Test plugin implementation
class ModulePlugin : public fox::pluga::IModule
{
public:
    ModulePlugin();
    virtual ~ModulePlugin();

    virtual const char* GetClassName() const;
};


#endif
