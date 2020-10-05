#ifndef FOX_MathPlugin_H
#define FOX_MathPlugin_H


#include "scy/pluga/plugin_api.h"

/// Test plugin implementation
class MathPlugin : public fox::pluga::IModule
{
public:
    MathPlugin();
    virtual ~MathPlugin();

    virtual const char* GetClassName() const;
    virtual NativeMethods GetMethods();

protected:

    std::string _error;
    std::string _value;
    NativeMethods m_oMethods;
};


#endif
