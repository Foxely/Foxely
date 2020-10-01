
#pragma once
#ifndef fox_value_h
#define fox_value_h

#include <vector>
#include "common.h"

// typedef double Value;

class Object;

enum ValueType {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ
};

class Value
{
public:
    ValueType type;
    union {
        bool boolean;
        double number;
        Object* obj;
    } as;
};

class ValueArray
{
public:
    int m_iCapacity;
    int m_iCount;
    std::vector<Value> m_vValues;

    ValueArray();
    void WriteValueArray(Value value);
};

#define IS_BOOL(val)    ((val).type == VAL_BOOL)
#define IS_NIL(val)     ((val).type == VAL_NIL)
#define IS_NUMBER(val)  ((val).type == VAL_NUMBER)
#define IS_OBJ(val)     ((val).type == VAL_OBJ)

#define AS_OBJ(val)     ((val).as.obj)
#define AS_BOOL(val)    ((val).as.boolean)
#define AS_NUMBER(val)  ((val).as.number)

#define BOOL_VAL(val)   ((Value){ VAL_BOOL, { .boolean = val } })
#define NIL_VAL           ((Value){ VAL_NIL, { .number = 0 } })
#define NUMBER_VAL(val) ((Value){ VAL_NUMBER, { .number = val } })
#define OBJ_VAL(object)   ((Value){ VAL_OBJ, { .obj = (Object *)object } })

bool ValuesEqual(Value a, Value b);
void PrintValue(Value value);

#endif