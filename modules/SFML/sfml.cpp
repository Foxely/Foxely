#include "sfml.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>


#include "foxely.h"


SCY_PLUGIN(SFMLPlugin, "SFML Module", "0.0.1")

ObjectAbstractType foxely_sfml_window_type =
{
    "sfml/window",
};

Value windowNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 3);
    Fox_PanicIfNot(Fox_IsString(args[0]), "Expected string value");
    Fox_PanicIfNot(Fox_IsNumber(args[1]), "Expected number value");
    Fox_PanicIfNot(Fox_IsNumber(args[2]), "Expected number value");
    // sf::VideoMode mode = sf::VideoMode(Fox_ValueToNumber(args[1]), Fox_ValueToNumber(args[2]));
    auto window = Fox_Window();
    Value instance = Fox_DefineInstanceOf("Window");

    // Fox_SetInstanceField(instance, "m_oWindow", Fox_Abstract(&window, &foxely_sfml_window_type));
    return instance;
}

RenderWindow* Fox_Window()
{
        RenderWindow *m;
        sf::RenderWindow *obj;

        m      = (typeof(m))malloc(sizeof(*m));
        obj    = new sf::RenderWindow(sf::VideoMode{1280, 720}, "dvdv");
        m->obj = obj;

		return m;
	}

Value windowIsOpenNative(int argCount, Value* args)
{
    // Fox_FixArity(argCount, 0);
    // Value windowField = Fox_GetInstanceField(args[-1], "m_oWindow");

    // sf::RenderWindow* window = (sf::RenderWindow *) Fox_AbstractGetData(Fox_ValueToAbstract(windowField));

    // return BOOL_VAL(window->isOpen());
    return NIL_VAL;
}

Value readLineNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");
    FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));
    size_t len = 0;
    char* line = NULL;
    ssize_t read;
    
    read = getline(&line, &len, fp);
    if (read != -1) {
        Value lineValue = Fox_StringToValue(line);
        if (line)
            free(line);
        return lineValue;
    }
    return NIL_VAL;
}

Value closeNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");

    if (!ValuesEqual(fpField, NIL_VAL))
    {
        FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));
        fclose(fp);
    }
    
    return NIL_VAL;
}



SFMLPlugin::SFMLPlugin()
{
    // std::cout << "SFMLPlugin: Create" << std::endl;
}

SFMLPlugin::~SFMLPlugin()
{
    // std::cout << "SFMLPlugin: Destroy" << std::endl;
}

const char* SFMLPlugin::GetClassName() const
{
    return "sfml";
}

NativeMethods SFMLPlugin::GetMethods()
{
	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("window", windowNative),
	};

    NativeMethods windowMethods =
	{
		std::make_pair<std::string, NativeFn>("isOpen", windowIsOpenNative),
	};

    Fox_DefineClass("Window", windowMethods);

	return methods;
}