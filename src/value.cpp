
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
    m_iCount++;
    m_vValues.push_back(value);
}

bool CompareObject(Value a, Value b)
{
    if (AS_OBJ(a)->type != AS_OBJ(b)->type)
        return false;
    
    switch (AS_OBJ(a)->type)
    {
    case OBJ_INSTANCE:
        return *AS_INSTANCE(a) == *AS_INSTANCE(b);
    case OBJ_NATIVE_INSTANCE:
        return *AS_NATIVE_INSTANCE(a) == *AS_NATIVE_INSTANCE(b);
    case OBJ_STRING:
        return AS_STRING(a) == AS_STRING(b);
    case OBJ_ARRAY:
        return *AS_ARRAY(a) == *AS_ARRAY(b);
    case OBJ_ABSTRACT:
        return *AS_ABSTRACT(a) == *AS_ABSTRACT(b);
    default:
        break;
    }
    // std::cout << AS_OBJ(a)->type;
    return false;
}

bool ValuesEqual(Value a, Value b)
{
    if (a.type != b.type) return false;

    switch (a.type)
    {
        case VAL_BOOL:   return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:    return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ:
            return CompareObject(a, b);
        default:
            return false; // Unreachable.
    }
}


// bool operator==(const Value& a, const Value& b)
// {
//     return ValuesEqual(a, b);
// }

bool Value::operator==(const Value& other) const
{
    return ValuesEqual(*this, other);
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
			std::cout << "<class " << AS_CLASS(value)->name->string << ">";
			break;
		case OBJ_INSTANCE:
			std::cout << AS_INSTANCE(value)->klass->name->string << " instance";
			break;
		case OBJ_BOUND_METHOD:
			PrintFunction(AS_BOUND_METHOD(value)->method->function);
			break;
        case OBJ_NATIVE_CLASS:
			std::cout << "<native class " << AS_NATIVE_CLASS(value)->name->string << ">";
			break;
        case OBJ_NATIVE_INSTANCE:
			std::cout << AS_NATIVE_INSTANCE(value)->klass->name->string << " native instance";
			break;
        case OBJ_ABSTRACT:
			std::cout << AS_ABSTRACT(value)->abstractType->name << " Abstract";
			break;
        case OBJ_LIB:
			std::cout << AS_LIB(value)->name->string << " Lib";
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
