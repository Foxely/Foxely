

#ifndef FOX_NAME_UTILS_HPP_
#define FOX_NAME_UTILS_HPP_

#include <cstring>

struct name_util {
    /* Remove parts from typename that aren't needed for component name */
    static void trim_name(char *typeName) {
        std::size_t len = std::strlen(typeName);
        
        /* Remove 'const' */
        std::size_t const_len = std::strlen("const ");
        if ((len > const_len) && !std::strncmp(typeName, "const ", const_len)) {
            std::memmove(typeName, typeName + const_len, len - const_len);
            typeName[len - const_len] = '\0';
            len -= const_len;
        }

        /* Remove 'struct' */
        std::size_t struct_len = std::strlen("struct ");
        if ((len > struct_len) && !std::strncmp(typeName, "struct ", struct_len)) {
            std::memmove(typeName, typeName + struct_len, len - struct_len);
            typeName[len - struct_len] = '\0';
            len -= struct_len;
        }

        /* Remove 'class' */
        std::size_t class_len = std::strlen("class ");
        if ((len > class_len) && !std::strncmp(typeName, "class ", class_len)) {
            std::memmove(typeName, typeName + class_len, len - class_len);
            typeName[len - class_len] = '\0';
            len -= class_len;
        }            

        while (typeName[len - 1] == ' ' ||
                typeName[len - 1] == '&' ||
                typeName[len - 1] == '*') 
        {
            len --;
            typeName[len] = '\0';
        }

        /* Remove const at end of string */
        if (len > const_len) {
            if (!std::strncmp(&typeName[len - const_len], " const", const_len)) {
                typeName[len - const_len] = '\0';
            }
        }
    }
};

#if defined(__clang__)
    static const unsigned int FRONT_SIZE = sizeof("static const char* name_helper<") - 1u;
    static const unsigned int BACK_SIZE = sizeof(">::name() [T = ]") - 1u;

    template <typename T>
    struct name_helper
    {
        static const char* name() {
            static const size_t size = (sizeof(__PRETTY_FUNCTION__) - FRONT_SIZE - BACK_SIZE) / 2 + 1u;
            static char typeName[size + 6] = {};
            memcpy(typeName, __PRETTY_FUNCTION__ + FRONT_SIZE, size - 1u);
            name_util::trim_name(typeName);
            return typeName;
        }
    };    
#elif defined(__GNUC__)
    template <typename T>
    struct name_helper
    {
        static std::string name() {
            static const std::string start="static const char* name_helper<T>::name() [with T = ";
            static const std::string end = "; std::string = std::__cxx11::basic_string<char>]";
            std::string funcMacro = __PRETTY_FUNCTION__;
            return funcMacro.substr(start.size(), funcMacro.size() - end.size() - start.size());
        }
    };
#elif defined(_WIN32)
    static const unsigned int FRONT_SIZE = sizeof("name_helper<") - 1u;
    static const unsigned int BACK_SIZE = sizeof(">::name") - 1u;
    
    template <typename T>
    struct name_helper
    {
            static const char* name(void) {
                static const size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
                static char typeName[size + 6] = {};
                memcpy(typeName, __FUNCTION__ + FRONT_SIZE, size - 1u);
                name_util::trim_name(typeName);
                return typeName;
            }
    };
#else
#error "implicit component registration not supported"
#endif

#endif