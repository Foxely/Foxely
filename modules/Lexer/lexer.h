#ifndef FOX_LexerPlugin_H
#define FOX_LexerPlugin_H


#include "scy/pluga/plugin_api.h"

/// Test plugin implementation
class LexerPlugin : public fox::pluga::IModule
{
public:
    LexerPlugin();
    virtual ~LexerPlugin();

    virtual const char* GetClassName() const;
};


#endif
