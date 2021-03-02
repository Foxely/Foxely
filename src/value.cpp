
#include <stdio.h>
#include <iostream>

#include "memory.h"
#include "value.hpp"
#include "object.hpp"
#include "Parser.h"
#include "vm.hpp"

ValueArray::ValueArray()
{
    m_vValues = std::vector<Value>();
    m_iCapacity = 0;
    m_iCount = 0;
}

void ValueArray::WriteValueArray(Value value)
{
    m_iCount++;
    m_vValues.push_back(value);
}

bool CompareObject(Value a, Value b)
{
    if (Fox_AsObject(a)->type != Fox_AsObject(b)->type)
        return false;
    
    switch (Fox_AsObject(a)->type)
    {
    case OBJ_INSTANCE:
        return *Fox_AsInstance(a) == *Fox_AsInstance(b);
    case OBJ_STRING:
        return Fox_AsString(a) == Fox_AsString(b);
    case OBJ_ARRAY:
        return *Fox_AsArray(a) == *Fox_AsArray(b);
    case OBJ_ABSTRACT:
        return *Fox_AsAbstract(a) == *Fox_AsAbstract(b);
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
    if (a.type == VAL_BOOL)   return Fox_AsBool(a) == Fox_AsBool(b);
    return a.as.obj == b.as.obj;
}

bool ValuesEqual(Value a, Value b)
{
    if (ValuesSame(a, b)) return true;

    // If we get here, it's only possible for two heap-allocated immutable objects
  // to be equal.
    if (!Fox_IsObject(a) || !Fox_IsObject(b)) return false;

    Object* aObject = Fox_AsObject(a);
    Object* bObject = Fox_AsObject(b);

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

std::string ObjectToString(Value value, VM* pVm)
{
    std::string string = "";
    switch (Fox_ObjectType(value))
    {
        case OBJ_STRING:
            string += (Fox_AsString(value))->string;
        	break;
		case OBJ_FUNCTION:
			string += FunctionToString(Fox_AsFunction(value));
			break;
		case OBJ_NATIVE:
            string += "<native fn>";
			break;
        case OBJ_CLOSURE:
            string += FunctionToString(Fox_AsClosure(value)->function);
            break;
        case OBJ_UPVALUE:
            string += "upvalue";
            break;
		case OBJ_CLASS:
			string += "<class ";
            string += Fox_AsClass(value)->name->string;
            string += ">";
			break;
		case OBJ_INSTANCE:
            string += Fox_AsInstance(value)->klass->name->string;
            string += " instance";
			break;
		case OBJ_BOUND_METHOD:
			string += FunctionToString(Fox_AsBoundMethod(value)->method->function);
			break;
        case OBJ_ABSTRACT:
			string += Fox_AsAbstract(value)->abstractType->name;
            string += " Abstract";
			break;
        case OBJ_LIB:
			string += Fox_AsLib(value)->name->string;
            string += " Lib";
			break;
        case OBJ_MODULE:
			string += Fox_AsModule(value)->m_strName->string;
            string += " Module";
			break;
        case OBJ_ARRAY:
        {
            ObjectArray* pArray = Fox_AsArray(value);
            string += "[";
            int size = pArray->m_vValues.size();
            for (auto& it : pArray->m_vValues)
            {
                --size;
                string += ValueToString(it, pVm);
                if (size > 0)
                    string += ", ";
            }
            string += "]";
			break;
        }
        case OBJ_MAP:
        {
            ObjectMap* pMap = Fox_AsMap(value);
            string += "{";
            int size = pMap->m_vValues.Size();
            for (auto& it : pMap->m_vValues.m_vEntries)
            {
                if (!(it.m_oKey == Fox_Nil) && !(it.m_oValue == Fox_Nil))
                {
                    size--;
                    string += ValueToString(it.m_oKey, pVm);
                    string += ": ";
                    string += ValueToString(it.m_oValue, pVm);
                    if (size > 0)
                        string += ", ";
                }
            }
            string += "}";
			break;
        }
        case OBJ_HANDLE:
            string += "Handle";
			break;
    }

    return string;
}

std::string ValueToString(Value value, VM* pVm)
{
    std::string string = "";
    switch (value.type)
    {
        case VAL_BOOL: string += (Fox_AsBool(value) ? "true" : "false"); break;
        case VAL_NIL:    string +=  "nil"; break;
        case VAL_NUMBER: string +=  std::to_string(Fox_AsDouble(value)); break;
        case VAL_INT: string +=  std::to_string(Fox_AsInt(value)); break;
        case VAL_OBJ: string += ObjectToString(value, pVm); break;
    }
    return string;
}

void PrintValue(Value value, VM* pVm)
{
    std::cout << ValueToString(value, pVm);
}