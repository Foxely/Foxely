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
    virtual NativeMethods GetMethods() const;

protected:

    std::string _error;
    std::string _value;
    std::string m_strClassName;
    NativeMethods m_oMethods;
};


#endif
