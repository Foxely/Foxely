
#include "foxely.h"

void DefineCoreString(VM* pVM)
{
    NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("length", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 0);

            return Fox_Int(Fox_AsString(args[-1])->string.size());
        }),

        std::make_pair<std::string, NativeFn>("count", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Wrong parameter in count function, only string type is allowed.");

            std::string& strObject = Fox_AsString(args[-1])->string;
            std::string& strLetter = Fox_AsString(args[0])->string;

            Fox_PanicIfNot(pVM, strLetter.size() >= 1, "Could not count a word, Expected character type.");
            Fox_PanicIfNot(pVM, strLetter.size() <= 1, "Could not count empty string.");

            int res = 0;
            for (int i = 0; i < strObject.size(); i++)
                if (strObject[i] == strLetter[0])
                    res++;
            return Fox_Int(res);
        }),

        std::make_pair<std::string, NativeFn>("find", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Wrong parameter in find function, Expected string type.");

            std::string& strObject = Fox_AsString(args[-1])->string;
            std::string& strLetter = Fox_AsString(args[0])->string;

            Fox_PanicIfNot(pVM, !strLetter.empty(), "Can't find an empty string.");
            Fox_PanicIfNot(pVM, !strObject.empty(), "Can't find in an empty string.");

            return Fox_Int(strObject.find_first_of(strLetter));
        }),

        std::make_pair<std::string, NativeFn>("split", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 1);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Wrong parameter in split function, Expected string type.");

            std::string& strObject = Fox_AsString(args[-1])->string;
            std::string& strDelimiter = Fox_AsString(args[0])->string;
            ObjectArray* pArray = Fox_AsArray(Fox_NewArray(pVM));

            Fox_PanicIfNot(pVM, !strDelimiter.empty(), "String delimiter can't be empty.");


            size_t          lIdx;
            size_t          l;
            
            for (l = 0; l < strObject.size(); )
            {
                lIdx = strObject.find(strDelimiter, l);
                if (lIdx == l)
                {
                    l++;
                    continue;
                }
                else
                {
                    if (lIdx >= strObject.size())
                        break;
                }
                pArray->m_vValues.push_back(Fox_NewString(pVM, strObject.substr(l, lIdx - l).c_str()));
                l = lIdx + 1;
            }
            if (l < strObject.size())
                pArray->m_vValues.push_back(Fox_NewString(pVM, strObject.substr(l, strObject.size() - l).c_str()));
            return Fox_Object(pArray);
        }),

        std::make_pair<std::string, NativeFn>("replace", [](VM* pVM, int argc, Value* args)
        {
            Fox_FixArity(pVM, argc, 2);
            Fox_PanicIfNot(pVM, Fox_IsString(args[0]), "Wrong parameter in replace function, Expected string type.");
            Fox_PanicIfNot(pVM, Fox_IsString(args[1]), "Wrong parameter in replace function, Expected string type.");

            std::string& strObject = Fox_AsString(args[-1])->string;
            std::string& strFrom = Fox_AsString(args[0])->string;
            std::string& strTo = Fox_AsString(args[1])->string;

            std::string newString;
            newString.reserve(strObject.length());  // avoids a few memory allocations

            std::string::size_type lastPos = 0;
            std::string::size_type findPos;

            while(std::string::npos != (findPos = strObject.find(strFrom, lastPos)))
            {
                newString.append(strObject, lastPos, findPos - lastPos);
                newString += strTo;
                lastPos = findPos + strFrom.length();
            }

            // Care for the rest after last occurrence
            newString += strObject.substr(lastPos);

            strObject.swap(newString);
            return Fox_Nil;
        }),
	};

    pVM->DefineBuiltIn(pVM->stringMethods, methods);
}