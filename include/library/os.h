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
    virtual NativeMethods GetMethods();

protected:

    std::string _error;
    std::string _value;
    NativeMethods m_oMethods;
};


#endif
