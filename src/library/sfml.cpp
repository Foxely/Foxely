// #include <iostream>
// #include <string>
// #include "library/sfml.h"

// #include "foxely.h"


// // SCY_PLUGIN(SFMLPlugin, "SFML Module", "0.0.1")

// ObjectAbstractType foxely_sfml_window_type =
// {
//     "sfml/window",
// };

// Value initWindowNative(int argCount, Value* args)
// {
//     Fox_FixArity(argCount, 3);
//     Fox_PanicIfNot(Fox_IsString(args[0]), "Expected string value");
//     Fox_PanicIfNot(Fox_IsNumber(args[1]), "Expected number value");
//     Fox_PanicIfNot(Fox_IsNumber(args[2]), "Expected number value");
//     sf::VideoMode mode = sf::VideoMode(Fox_ValueToNumber(args[1]), Fox_ValueToNumber(args[2]));
//     sf::RenderWindow* window = new sf::RenderWindow(mode, Fox_ValueToCString(args[0]));

//     Fox_SetInstanceField(args[-1], "m_oWindow", Fox_Abstract(window, &foxely_sfml_window_type));
//     return Fox_Nil;
// }


// Value windowIsOpenNative(int argCount, Value* args)
// {
//     Fox_FixArity(argCount, 0);
//     Value windowField = Fox_GetInstanceField(args[-1], "m_oWindow");

//     sf::RenderWindow* window = (sf::RenderWindow *) Fox_AbstractGetData(Fox_ValueToAbstract(windowField));

//     return Fox_Bool(window->isOpen());
// }

// Value windowClearNative(int argCount, Value* args)
// {
//     Fox_FixArity(argCount, 0);
//     Value windowField = Fox_GetInstanceField(args[-1], "m_oWindow");

//     sf::RenderWindow* window = (sf::RenderWindow *) Fox_AbstractGetData(Fox_ValueToAbstract(windowField));

//     window->clear();
//     return Fox_Nil;
// }

// Value windowDisplayNative(int argCount, Value* args)
// {
//     Fox_FixArity(argCount, 0);
//     Value windowField = Fox_GetInstanceField(args[-1], "m_oWindow");

//     sf::RenderWindow* window = (sf::RenderWindow *) Fox_AbstractGetData(Fox_ValueToAbstract(windowField));

//     window->display();
//     return Fox_Nil;
// }

// Value windowPollNative(int argCount, Value* args)
// {
//     Fox_FixArity(argCount, 1);
//     Value windowField = Fox_GetInstanceField(args[-1], "m_oWindow");

//     sf::RenderWindow* window = (sf::RenderWindow *) Fox_AbstractGetData(Fox_ValueToAbstract(windowField));

//     window->pollEvent();
//     return Fox_Nil;
// }

// SFMLPlugin::SFMLPlugin()
// {
//     // std::cout << "SFMLPlugin: Create" << std::endl;
		// NativeMethods methods =
		// {
		// };

		// NativeMethods windowMethods =
		// {
		// 	std::make_pair<std::string, NativeFn>("init", initWindowNative),
		// 	std::make_pair<std::string, NativeFn>("isOpen", windowIsOpenNative),
		// 	std::make_pair<std::string, NativeFn>("clear", windowClearNative),
		// 	std::make_pair<std::string, NativeFn>("display", windowDisplayNative),
		// };

		// Fox_DefineClass("Window", windowMethods);
//		m_oMethods = methods;

// }

// SFMLPlugin::~SFMLPlugin()
// {
//     // std::cout << "SFMLPlugin: Destroy" << std::endl;
// }

// const char* SFMLPlugin::GetClassName() const
// {
//     return "sfml";
// }