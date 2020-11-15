
#pragma once
#ifndef fox_value_h
#define fox_value_h

#include <vector>
#include <string>
#include "common.h"

// typedef double Value;

class Object;

enum ValueType {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_INT,
    VAL_OBJ
};

class Value
{
public:
    ValueType type;
    union {
        bool boolean;
        double number;
        int integer;
        Object* obj;
    } as;

    bool operator==(const Value& other) const;
};


bool ValuesEqual(Value a, Value b);
void PrintValue(Value value);
std::string ValueToString(Value value);

// inline bool operator==(const Value& a, const Value& b)
// {
//     return ValuesEqual(a, b);
// }

class ValueArray
{
public:
    int m_iCapacity;
    int m_iCount;
    std::vector<Value> m_vValues;

    ValueArray();
    void WriteValueArray(Value value);
};

#define Fox_IsBool(val)    ((val).type == VAL_BOOL)
#define Fox_IsNil(val)     ((val).type == VAL_NIL)
#define Fox_IsDouble(val)  ((val).type == VAL_NUMBER)
#define Fox_IsInt(val)     ((val).type == VAL_INT)
#define Fox_IsObject(val)     ((val).type == VAL_OBJ)

#define Fox_AsObject(val)     ((val).as.obj)
#define Fox_AsBool(val)    ((val).as.boolean)
#define Fox_AsDouble(val)  ((val).as.number)
#define Fox_AsInt(val)  ((val).as.integer)

#define Fox_Bool(val)   ((Value){ VAL_BOOL, { .boolean = val } })
#define Fox_Nil           ((Value){ VAL_NIL, { .number = 0 } })
#define Fox_Double(val) ((Value){ VAL_NUMBER, { .number = val } })
#define Fox_Int(val) ((Value){ VAL_INT, { .integer = val } })
#define Fox_Object(object)   ((Value){ VAL_OBJ, { .obj = (Object *)object } })

#endif