#ifndef FOX_ArrayPlugin_H
#define FOX_ArrayPlugin_H


#include "scy/pluga/plugin_api.h"

/// Test plugin implementation
class ArrayPlugin : public fox::pluga::IModule
{
public:
    ArrayPlugin();
    virtual ~ArrayPlugin();

    virtual const char* GetClassName() const;

};


#endif
