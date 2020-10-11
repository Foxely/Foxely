#ifndef FOX_OSPlugin_H
#define FOX_OSPlugin_H


#include "scy/pluga/plugin_api.h"

/// Test plugin implementation
class OSPlugin : public fox::pluga::IModule
{
public:
    OSPlugin();
    virtual ~OSPlugin();

    virtual const char* GetClassName() const;
};


#endif
