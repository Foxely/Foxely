#include "object.hpp"
#include "Parser.h"
#include "vm.hpp"

ObjectClosure::ObjectClosure(VM* oVM, ref<ObjectFunction> func)
{
    type = OBJ_CLOSURE;
    function = func;
    upvalueCount = func->upValueCount;
    func->module = oVM->currentModule;
    upValues = std::vector<ref<ObjectUpvalue>>(func->upValueCount);
    for (int i = 0; i < func->upValueCount; i++) {
        upValues[i] = NULL;
    }
}