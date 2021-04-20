
#ifndef fox_value_h
#define fox_value_h

#include <vector>
#include <string>
#include <variant>
#include "common.h"

// typedef double Value;

class Object;
class ObjectString;
class ObjectArray;
class ObjectClosure;
class ObjectFiber;
class ObjectFunction;
class ObjectModule;
class ObjectUpvalue;

class ObjectNative;
class ObjectLib;
class ObjectClass;
class ObjectInstance;
class ObjectBoundMethod;
class ObjectAbstract;
class ObjectMap;
class VM;

enum ValueType {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    // VAL_INT,
    VAL_OBJ,
    // VAL_STRING,
    // VAL_CLOSURE,
    // VAL_ARRAY,
    // VAL_FUNCTION,
    // VAL_FIBER,
    // VAL_MODULE,
    // VAL_NATIVE,
    // VAL_LIB,
    // VAL_CLASS,
    // VAL_INSTANCE,
    // VAL_BOUND_METHOD,
    // VAL_ABSTRACT,
    // VAL_MAP,
    // VAL_UPVALUE,
};

class Value
{
public:
    Value()
    {
        type = VAL_NIL;
    }

    Value(const Value& other) : type(other.type), val(other.val)
    {
    }

    Value(bool value)
    {
        type = VAL_BOOL;
        val.boolean = value;
    }

    Value(double value)
    {
        type = VAL_NUMBER;
        val.number = value;
    }

    Value(int value)
    {
        type = VAL_NUMBER;
        val.number = static_cast<double>(value);
        // type = VAL_INT;
    }

    Value(Object* value)
    {
        type = VAL_OBJ;
        val.obj = value;
    }

    ~Value() = default;
    
    ValueType type;
    union {
        bool boolean;
        double number;
        int integer;
        Object* obj;
        // ObjectString* string;
        // ObjectArray* array;
        // ObjectClosure* closure;
        // ObjectFiber* fiber;
        // ObjectFunction* func;
        // ObjectModule* module;
        // ObjectUpvalue* up_value;
        // ObjectNative* native;
        // ObjectLib* lib;
        // ObjectClass* klass;
        // ObjectInstance* instance;
        // ObjectBoundMethod* bound;
        // ObjectAbstract* abstract;
        // ObjectMap* map;
    } val;
    // std::variant<bool, double,
    //     Object*,
    //     ObjectString*,
    //     ObjectArray*,
    //     ObjectClosure*,
    //     ObjectFiber*,
    //     ObjectFunction*,
    //     ObjectModule*,
    //     ObjectUpvalue*,
    //     ObjectNative*,
    //     ObjectLib*,
    //     ObjectClass*,
    //     ObjectInstance*,
    //     ObjectBoundMethod*,
    //     ObjectAbstract*,
    //     ObjectMap*> val;

    // template <typename T>
    // ref<T>& as_ref()
    // {
    //     return std::get<ref<T>>(val);
    // }

    template <typename T>
    T* as_ptr()
    {
        if (type != VAL_OBJ)
            throw std::runtime_error("Invalid Type");
        return static_cast<T*>(val.obj);
    }

    template <typename T,
    std::enable_if_t<!std::is_base_of<Object, T>::value, bool> = true>
    T as();

    template <typename T,
    std::enable_if_t<std::is_base_of<Object, T>::value, bool> = true>
    T* as();

    template<typename T>
    bool is();
    // {
    //     return false;
    // }

    bool operator==(const Value& other) const;

    Value& operator=(const Value& other)
    {
        type = other.type;
        val = other.val;
        return *this;
    }

    Value& operator=(double value)
    {
        type = VAL_NUMBER;
        val.number = value;
        return *this;
    }

    Value& operator=(bool value)
    {
        type = VAL_BOOL;
        val.boolean = value;
        return *this;
    }

    Value& operator=(Object* value)
    {
        type = VAL_OBJ;
        val.obj = value;
        return *this;
    }
};

template <>
bool Value::as<bool>();

template <>
double Value::as<double>();

// template <>
// double Value::as<int>();

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
#define Fox_IsNumber(val)  ((val).type == VAL_NUMBER)
// #define Fox_IsInt(val)     ((val).type == VAL_INT)
#define Fox_IsObject(val)     ((val).type != VAL_BOOL &&    \
                                (val).type != VAL_NIL &&    \
                                (val).type != VAL_NUMBER )

#define Fox_AsObject(val)     ((val).as<Object>())
#define Fox_AsBool(val)    ((val).as<bool>())
#define Fox_AsNumber(val)  ((val).as<double>())
// #define Fox_AsInt(val)  ((val).as<int>())

#define Fox_Bool(val)           (Value(val))
#define Fox_Nil                 (Value())
#define Fox_Number(val)         (Value(val))
// #define Fox_Int(val)            (Value(val))
#define Fox_Object(object)      (Value(object))

#endif