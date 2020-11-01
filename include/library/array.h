#ifndef FOX_ArrayPlugin_H
#define FOX_ArrayPlugin_H


#include "scy/pluga/plugin_api.h"
#include "foxely.h"

/// Test plugin implementation
class ArrayPlugin : public fox::pluga::IModule
{
public:
    ArrayPlugin(VM* oVM);
    virtual ~ArrayPlugin();

    virtual const char* GetClassName() const;

};

// FOX_MODULE(array);

#endif
