#ifndef SCY_SharedLibrary_H
#define SCY_SharedLibrary_H

#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include "dlfcn.h"

namespace fox
{
    struct lib_t
    {
        void* handle;
        char* errmsg;
    };
    typedef lib_t lib_t;

    static int fox__dlerror(lib_t* lib);


    int fox_dlopen(const char* filename, lib_t* lib)
    {
        dlerror(); /* Reset error status. */
        lib->errmsg = NULL;
        lib->handle = dlopen(filename, RTLD_LAZY);
        return lib->handle ? 0 : fox__dlerror(lib);
    }


    void fox_dlclose(lib_t* lib)
    {
        free(lib->errmsg);
        lib->errmsg = NULL;

        if (lib->handle) {
            /* Ignore errors. No good way to signal them without leaking memory. */
            dlclose(lib->handle);
            lib->handle = NULL;
        }
    }


    int fox_dlsym(lib_t* lib, const char* name, void** ptr)
    {
        dlerror(); /* Reset error status. */
        *ptr = dlsym(lib->handle, name);
        return fox__dlerror(lib);
    }


    const char* fox_dlerror(const lib_t* lib)
    {
        return lib->errmsg ? lib->errmsg : "no error";
    }


    static int fox__dlerror(lib_t* lib)
    {
        const char* errmsg;

        free(lib->errmsg);

        errmsg = dlerror();

        if (errmsg)
        {
            lib->errmsg = strdup(errmsg);
            return -1;
        }
        else
        {
            lib->errmsg = NULL;
            return 0;
        }
    }


    struct SharedLibrary
    {
        /// Opens a shared library.
        /// The filename is in utf-8. Returns true on success and false on error.
        /// Call `SharedLibrary::error()` to get the error message.
        bool open(const std::string& path)
        {
            if (fox_dlopen(path.c_str(), &_lib)) {
                setError("Cannot load library");
                return false;
            }
            return true;
        }

        /// Closes the shared library.
        void close()
        {
            fox_dlclose(&_lib);
        }

        /// Retrieves a data pointer from a dynamic library.
        /// It is legal for a symbol to map to nullptr.
        /// Returns 0 on success and -1 if the symbol was not found.
        bool sym(const char* name, void** ptr)
        {
            if (fox_dlsym(&_lib, name, ptr)) {
                setError("Symbol '%s' not found." + std::string(name));
                return false;
            }
            return true;
        }

        void setError(const std::string& prefix)
        {
            std::string err(fox_dlerror(&_lib));
            if (err.empty())
                return;
            _error = prefix + ": " + err;
            throw std::runtime_error(prefix + ": " + err);
        }

        std::string error() const { return _error; }

    protected:
        lib_t _lib;
        std::string _error;
    };
} // namespace fox


#endif // SCY_SharedLibrary_H