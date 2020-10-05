#ifndef SCY_PluginAPI_H
#define SCY_PluginAPI_H

#include <string>
#include <map>
#include <utility>
#include "pluga.h"
#include "Parser.h"
#include "vm.hpp"


class Value;

typedef Value (*NativeFn)(int arg_count, Value *args);

namespace fox {
    namespace pluga {

        // NOTE: When using STL containers and other complex types you
        // will need to ensure plugins are always built using the same
        // compiler, since system libraries aren't ABI compatible.
        #define PLUGA_ENABLE_STL 1


        /// Virtual base class for plugins
        class IModule
        {
        public:
            IModule() {};
            virtual ~IModule() {};

            virtual const char* GetClassName() const {};
            virtual NativeMethods GetMethods() {};
        };

    } // namespace pluga
} // namespace fox


#endif
