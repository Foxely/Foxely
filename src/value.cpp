
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
    case OBJ_STRING:
        return AS_STRING(a) == AS_STRING(b);
    case OBJ_ARRAY:
        return *AS_ARRAY(a) == *AS_ARRAY(b);
    case OBJ_ABSTRACT:
        return *AS_ABSTRACT(a) == *AS_ABSTRACT(b);
    default:
        break;
    }
    return false;
}

// Returns true if [a] and [b] are strictly the same value. This is identity
// for object values, and value equality for unboxed values.
bool ValuesSame(Value a, Value b)
{
    if (a.type != b.type)     return false;
    if (a.type == VAL_NIL)    return true;
    if (a.type == VAL_NUMBER) return a.as.number == b.as.number;
    if (a.type == VAL_INT) return a.as.integer == b.as.integer;
    if (a.type == VAL_BOOL)   return AS_BOOL(a) == AS_BOOL(b);
    return a.as.obj == b.as.obj;
}

bool ValuesEqual(Value a, Value b)
{
    if (ValuesSame(a, b)) return true;

    // If we get here, it's only possible for two heap-allocated immutable objects
  // to be equal.
    if (!IS_OBJ(a) || !IS_OBJ(b)) return false;

    Object* aObject = AS_OBJ(a);
    Object* bObject = AS_OBJ(b);

    // Must be the same type.
    if (aObject->type != bObject->type) return false;

    return CompareObject(a, b);
}

bool Value::operator==(const Value& other) const
{
    return ValuesEqual(*this, other);
}



std::string FunctionToString(ObjectFunction* function)
{
    std::string string = "";
	if (function->name == NULL) {
        string += "<script>";
		return string;
	}

    string += "<fn ";
    string += function->name->string;
    string += ">";

    return string;
}

std::string ObjectToString(Value value)
{
    std::string string = "";
    switch (OBJ_TYPE(value))
    {
        case OBJ_STRING:
            string += (AS_STRING(value))->string;
        	break;
		case OBJ_FUNCTION:
			string += FunctionToString(AS_FUNCTION(value));
			break;
		case OBJ_NATIVE:
            string += "<native fn>";
			break;
        case OBJ_CLOSURE:
            string += FunctionToString(AS_CLOSURE(value)->function);
            break;
        case OBJ_UPVALUE:
            string += "upvalue";
            break;
		case OBJ_CLASS:
			string += "<class ";
            string += AS_CLASS(value)->name->string;
            string += ">";
			break;
		case OBJ_INSTANCE:
			string += AS_INSTANCE(value)->klass->name->string;
            string += " instance";
			break;
		case OBJ_BOUND_METHOD:
			string += FunctionToString(AS_BOUND_METHOD(value)->method->function);
			break;
        // case OBJ_NATIVE_CLASS:
		// 	string += "<native class ";
        //     string += AS_NATIVE_CLASS(value)->name->string;
        //     string += ">";
		// 	break;
        // case OBJ_NATIVE_INSTANCE:
		// 	string += AS_NATIVE_INSTANCE(value)->klass->name->string;
        //     string += " native instance";
		// 	break;
        case OBJ_ABSTRACT:
			string += AS_ABSTRACT(value)->abstractType->name;
            string += " Abstract";
			break;
        case OBJ_LIB:
			string += AS_LIB(value)->name->string;
            string += " Lib";
			break;
    }

    return string;
}

std::string ValueToString(Value value)
{
    std::string string = "";
    switch (value.type)
    {
        case VAL_BOOL: string += (AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL:    string +=  "nil"; break;
        case VAL_NUMBER: string +=  std::to_string(AS_NUMBER(value)); break;
        case VAL_INT: string +=  std::to_string(AS_INT(value)); break;
        case VAL_OBJ: string += ObjectToString(value); break;
    }
    return string;
}

void PrintValue(Value value)
{
    std::cout << ValueToString(value);
}