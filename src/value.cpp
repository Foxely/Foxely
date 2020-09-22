
#include <stdio.h>

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
        case VAL_OBJ:
        {
            ObjectString* aString = AS_STRING(a);
            ObjectString* bString = AS_STRING(b);
            return aString->string == bString->string;
        }
        default:
        return false; // Unreachable.
    }
}

void printObject(Value value)
{
    switch (OBJ_TYPE(value))
    {
        case OBJ_STRING:
        printf("%s", AS_CSTRING(value));
        break;
    }
}

void PrintValue(Value value)
{
    switch (value.type)
    {
        case VAL_BOOL:   printf(AS_BOOL(value) ? "true" : "false"); break;
        case VAL_NIL:    printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
        case VAL_OBJ: printObject(value); break;
    }
}