#include "Parser.h"
#include "vm.hpp"
#include "object.hpp"

ObjectClosure::ObjectClosure(VM* oVM, ObjectFunction *func)
{
    type = OBJ_CLOSURE;
    function = func;
    upvalueCount = func->upValueCount;
    upValues = oVM->gc.NewArray<ObjectUpvalue*>(func->upValueCount);
    for (int i = 0; i < func->upValueCount; i++) {
        upValues[i] = NULL;
    }
}