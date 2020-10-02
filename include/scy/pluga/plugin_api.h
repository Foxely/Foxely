#ifndef SCY_TestPluginAPI_H
#define SCY_TestPluginAPI_H


#include "scy/pluga/pluga.h"
#include "Parser.h"
#include "vm.hpp"
#include <string>
#include <map>
#include <utility>
#include <functional>

class Value;
class VM;

namespace fox {
    namespace pluga {

		// typedef Value (IPlugin* NativeFn)(int arg_count, Value *args);

        // NOTE: When using STL containers and other complex types you
        // will need to ensure plugins are always built using the same
        // compiler, since system libraries aren't ABI compatible.
        #define PLUGA_ENABLE_STL 1


        /// Virtual base class for plugins
        class IPlugin
        {
			VM* m_pVm;
        protected:
            // using Native = std::pair<std::function<Value(int, Value *)>, int>;
            using Native = std::function<Value(int, Value *)>;
            using NativeMethods = std::map<std::string, Native>;
        public:
            IPlugin(){};
            // IPlugin(VM* pVm) { m_pVm = pVm; };
            virtual ~IPlugin(){};

			void SetEnv(VM* pVm)
			{
				m_pVm = pVm;
			}

            //
            /// Commands

            /// Handle a command from the application.
            virtual bool onCommand(const char* node, const char* data, unsigned int size) = 0;

            virtual bool CallMethod(const char* name, int argCount, Value* args, Value& result) = 0;

            /// Return the last error message as a char pointer.
            /// If no error is set a nullptr will be returned.
            virtual const char* lastError() const = 0;


            //
            /// String accessors

            // Sets the internal string from the given value.
            /// The given value must be null terminated.
            virtual void setValue(const char* value) = 0;

            // Return the internal string value as a char pointer.
            /// Since we are returning a POD type plugins will be ABI agnostic.
            virtual const char* cValue() const = 0;

            virtual const char* GetClassName() const = 0;

        #if PLUGA_ENABLE_STL
            /// Return the internal string value as an STL string.
            /// This method breaks ABI agnosticity.
            /// See the PLUGU_ENABLE_STL definition above.
            virtual std::string sValue() const = 0;
        #endif

		    Value Fox_StringToValue(const char* str)
			{
				return OBJ_VAL(m_pVm->m_oParser.TakeString(str));
			}
        };


        typedef int (*GimmeFiveFunc)(); /// Static function which returns, you guessed
                                        /// it, the number 5!


    } // namespace pluga
} // namespace scy


#endif
