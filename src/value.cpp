
#include <stdio.h>
#include <iostream>

#include "memory.h"
#include "value.hpp"
#include "object.hpp"

ValueArray::ValueArray()
{
    m_vValues = std::vector<Value>();
    m_iCapacity = 0;
    m_iCount = 0;
}

void ValueArray::WriteValueArray(Value value)
{
//     if (array->capacity < array->count + 1) {
//     int oldCapacity = array->capacity;
//     array->capacity = GROW_CAPACITY(oldCapacity);
//     array->values = GROW_ARRAY(Value, array->values,
//                                oldCapacity, array->capacity);
//   }

//   array->values[array->count] = value;
//   array->count++;
    m_vValues.push_back(value);
}

bool ValuesEqual(Value a, Value b)
{
    if (a.type != b.type) return false;

    switch (a.type) {
        case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:    return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ: return AS_OBJ(a) == AS_OBJ(b);
        default:
        return false; // Unreachable.
    }
}

void PrintFunction(ObjectFunction* function)
{
	if (function->name == NULL) {
        std::cout << "<script>";
		return;
	}

    std::cout << "<fn " << function->name->string << ">";
}

void printObject(Value value)
{
    switch (OBJ_TYPE(value))
    {
        case OBJ_STRING:
            std::cout << (AS_STRING(value))->string;
        	break;
		case OBJ_FUNCTION:
			PrintFunction(AS_FUNCTION(value));
			break;
		case OBJ_NATIVE:
            std::cout << "<native fn>";
			break;
        case OBJ_CLOSURE:
            PrintFunction(AS_CLOSURE(value)->function);
            break;
        case OBJ_UPVALUE:
            printf("upvalue");
            break;
		case OBJ_CLASS:
			std::cout << AS_CLASS(value)->name->string;
			break;
		case OBJ_INSTANCE:
			std::cout << AS_INSTANCE(value)->klass->name->string << " instance";
			break;
		case OBJ_BOUND_METHOD:
			PrintFunction(AS_BOUND_METHOD(value)->method->function);
			break;
    }
}

void PrintValue(Value value)
{
    switch (value.type)
    {
        case VAL_BOOL: std::cout << (AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL:    std::cout << "nil"; break;
        case VAL_NUMBER: std::cout << AS_NUMBER(value); break;
        case VAL_OBJ: printObject(value); break;
    }
}
