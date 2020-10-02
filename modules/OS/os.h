#ifndef SCY_TestPlugin_H
#define SCY_TestPlugin_H


#include "scy/pluga/plugin_api.h"

/// Test plugin implementation
class OSPlugin : public fox::pluga::IModule
{
public:
    OSPlugin();
    virtual ~OSPlugin();

    virtual const char* GetClassName() const;
    virtual NativeMethods GetMethods() const;

protected:

    std::string _error;
    std::string _value;
    std::string m_strClassName;
    NativeMethods m_oMethods;
};


#endif
