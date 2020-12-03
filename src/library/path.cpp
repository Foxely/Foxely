#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "library/library.h"
#include "foxely.h"

// "/foo/bar/baz.txt" --> "baz.txt"
std::string GetFilename(const std::string& strFilepath, std::string strDelimiter)
{
    return strFilepath.substr(strFilepath.find_last_of(strDelimiter) + 1);
}

void DefinePathModule(VM* pVM)
{
    NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("getPath", [] (VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 2);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Expected string type.");
            Fox_PanicIfNot(pVM, Fox_IsString(args[1]), "Expected string type.");

            std::string& strPath = Fox_AsString(args[0])->string;
            std::string& strDelimiter = Fox_AsString(args[1])->string;

            return Fox_NewString(pVM, "strPath.substr( 0, strPath.find_last_of( strDelimiter ) + 1 ).c_str()");
        }),

        std::make_pair<std::string, NativeFn>("getFilename", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 2);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Expected string type.");
            Fox_PanicIfNot(pVM, Fox_IsString(args[1]), "Expected string type.");
            
            std::string& strPath = Fox_AsString(args[0])->string;
            std::string& strDelimiter = Fox_AsString(args[1])->string;
            
            return Fox_NewString(pVM, strPath.substr(strPath.find_last_of(strDelimiter) + 1).c_str());
        }),

        std::make_pair<std::string, NativeFn>("getFileExtension", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 2);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Expected string type.");
            Fox_PanicIfNot(pVM, Fox_IsString(args[1]), "Expected string type.");

            std::string& strFilepath = Fox_AsString(args[0])->string;
            std::string& strDelimiter = Fox_AsString(args[1])->string;
            
            std::string strFilename = GetFilename(strFilepath, strDelimiter);
            std::string::size_type n = strFilename.find_last_of('.');

            if (n != std::string::npos)
                return Fox_NewString(pVM, strFilename.substr(n).c_str());

            return Fox_Nil;
        }),

        std::make_pair<std::string, NativeFn>("removeExtension", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Expected string type.");

            std::string& strFilename = Fox_AsString(args[0])->string;

            size_t lastindex = strFilename.find_last_of(".");
            return Fox_NewString(pVM, strFilename.substr(0, lastindex).c_str()); 
        }),
	};

    pVM->DefineModule("path");
    pVM->DefineLib("path", "path", methods);
}