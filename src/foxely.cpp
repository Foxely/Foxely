
#include "foxely.h"
#include "Parser.h"
#include "vm.hpp"

extern "C" Value Fox_StringToValue(VM* vm, const char* str)
{
	return OBJ_VAL(vm->m_oParser.TakeString(str));
}
