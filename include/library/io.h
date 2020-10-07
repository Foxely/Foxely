#ifndef FOX_IOPlugin_H
#define FOX_IOPlugin_H


#include "scy/pluga/plugin_api.h"

/// Test plugin implementation
class IOPlugin : public fox::pluga::IModule
{
public:
    IOPlugin();
    virtual ~IOPlugin();

    virtual const char* GetClassName() const;
};


#endif
