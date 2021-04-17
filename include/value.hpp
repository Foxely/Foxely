
#ifndef fox_value_h
#define fox_value_h

#include <vector>
#include <string>
#include <variant>
#include "common.h"

// typedef double Value;

class Object;
class VM;

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
    Value()
    {
        type = VAL_NIL;
        val = 0;
    }

    Value(const Value& other) : type(other.type), val(other.val)
    {
    }

    Value(bool value) : val(value)
    {
        type = VAL_BOOL;
    }

    Value(double value) : val(value)
    {
        type = VAL_NUMBER;
    }

    Value(int value) : val(value)
    {
        type = VAL_INT;
    }

    Value(ref<Object> value) : val(value)
    {
        type = VAL_OBJ;
    }

    ~Value() = default;
    
    ValueType type;
    std::variant<bool, double, int, ref<Object>> val;

    template <typename T>
    ref<T> as_ref()
    {
        return std::dynamic_pointer_cast<T>(std::get<ref<Object>>(val));
    }

    template <typename T>
    T as()
    {
        return static_cast<T>(*std::get<ref<Object>>(val));
    }

    bool operator==(const Value& other) const;

    Value& operator=(const Value& other)
    {
        type = other.type;
        val = other.val;
        return *this;
    }
};

template <>
bool Value::as<bool>();

template <>
double Value::as<double>();

template <>
int Value::as<int>();


bool ValuesEqual(Value a, Value b);
void PrintValue(Value value, VM* pVm = nullptr);
std::string ValueToString(Value value, VM* pVm = nullptr);

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

#define Fox_AsObject(val)     ((val).as_ref<Object>())
#define Fox_AsBool(val)    ((val).as<bool>())
#define Fox_AsDouble(val)  ((val).as<double>())
#define Fox_AsInt(val)  ((val).as<int>())

#define Fox_Bool(val)           (Value(val))
#define Fox_Nil                 (Value())
#define Fox_Double(val)         (Value(val))
#define Fox_Int(val)            (Value(val))
#define Fox_Object(object)      (Value(object))

#endif