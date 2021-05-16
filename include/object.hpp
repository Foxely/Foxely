/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#ifndef FOX_OBJECT_HPP_
#define FOX_OBJECT_HPP_

#include <iostream>
#include <string>
#include <functional>
#include <tuple>

#include "chunk.hpp"
#include "value.hpp"
#include "Table.hpp"
#include "Map.hpp"
#include "gc.hpp"

class VM;
template<typename T>
class Klass;


#define Fox_ObjectType(val)         (Fox_AsObject(val)->type)

#define Fox_IsMap(val)        is_obj_type(val, OBJ_MAP)
#define Fox_IsArray(val)        is_obj_type(val, OBJ_ARRAY)
#define Fox_IsAbstract(val)        is_obj_type(val, OBJ_ABSTRACT)
#define Fox_IsLib(val)           is_obj_type(val, OBJ_LIB)
#define Fox_IsBoundMethod(val)  is_obj_type(val, OBJ_BOUND_METHOD)
#define Fox_IsClass(val)         is_obj_type(val, OBJ_CLASS)
#define Fox_IsClosure(val)       is_obj_type(val, OBJ_CLOSURE)
#define Fox_IsInstance(val)      is_obj_type(val, OBJ_INSTANCE)
#define Fox_IsFunction(val)      is_obj_type(val, OBJ_FUNCTION)
#define Fox_IsNative(val)        is_obj_type(val, OBJ_NATIVE)
#define Fox_IsString(val)        is_obj_type(val, OBJ_STRING)
#define Fox_IsModule(val)        is_obj_type(val, OBJ_MODULE)
#define Fox_IsFiber(val)        is_obj_type(val, OBJ_FIBER)

#define Fox_AsMap(val)              ((val).as<ObjectMap>())
#define Fox_AsArray(val)            ((val).as<ObjectArray>())
#define Fox_AsAbstract(val)         ((val).as<ObjectAbstract>())
#define Fox_AsLib(val)           	((val).as<ObjectLib>())
#define Fox_AsBoundMethod(val)  	((val).as<ObjectBoundMethod>())
#define Fox_AsClass(val)         	((val).as<ObjectClass>())
#define Fox_AsClosure(val)       	((val).as<ObjectClosure>())
#define Fox_AsFunction(val)      	((val).as<ObjectFunction>())
#define Fox_AsInstance(val)         ((val).as<ObjectInstance>())
#define Fox_AsNative(val)        	((val).as<ObjectNative>()->function)
#define Fox_AsString(val)        	((val).as<ObjectString>())
#define Fox_AsCString(val)       	((Fox_AsString(val))->string.c_str())
#define Fox_AsModule(val)       	((val).as<ObjectModule>())
#define Fox_AsFiber(val)       	    ((val).as<ObjectFiber>())

typedef enum {
    OBJ_UNKNOWN,
    OBJ_ARRAY,
    OBJ_MAP,
    OBJ_ABSTRACT,
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_USER,
    OBJ_NATIVE,
    OBJ_LIB,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_MODULE,
    OBJ_HANDLE,
    OBJ_FIBER,
} ObjType;

struct utils
{
    template <class T>
    static T arg(int ac, Value* av, const int i = 0)
	{
		if (i < 0 && i > ac)
			throw std::runtime_error("csvsdvdsv");
        // if (std::is_base_of<Object, T>::value)
		// {
        //     return *(T *)object(i);
        // } else {
        //     return *(T *)userdata(i);
        // }
        return T();
    }

    template <class T>
    static T* argp(int ac, Value* av, const int i = 0);

	template <typename T, typename T1, typename... Args>
    static std::tuple<T, T1, Args...> args(int ac, Value* av, const int i = 0)
	{
        T t = arg<T>(ac, av, i);
        return std::tuple_cat(std::make_tuple(t), args<T1, Args...>(ac, av, i + 1));
    }

    template <typename T>
    static std::tuple<T> args(int ac, Value* av, const int i = 0)
	{
        return std::make_tuple(arg<T>(ac, av, i));
    }

	template <int N>
	struct apply_function
	{
        template <
			typename R,
			typename... FunctionArgs,
			typename... TupleArgs,
			typename... Args >
        static R apply(R (*function)(FunctionArgs...),
            std::tuple<TupleArgs...>& t, Args... args) {
            return
                apply_function<N-1>::apply(function, t, std::get<N-1>(t), args...);
        }
    };

    template <int N>
    struct apply_method
    {
        template <
            class T,
            typename R,
            typename... MethodArgs,
            typename... TupleArgs,
            typename... Args >
        static R apply(T* o, R (T::*method)(MethodArgs...),
            std::tuple<TupleArgs...>& t, Args... args) {
            return
                apply_method<N-1>::apply(o, method, t, std::get<N-1>(t), args...);
        }
    };
}; // namespace utils

struct CallFrame;


class Object : public Traceable
{
public:
    Object() { }
	virtual ~Object() {}
    ObjType type;

    bool operator==(const Object& other) const
    {
        return type == other.type;
    }
};

class ObjectString : public Object
{
public:
    explicit ObjectString(const std::string& v) : string(v) {}

    uint32_t hash;
    std::string string;
};

// A loaded module and the top-level variables it defines.
//
// While this is an Object and is managed by the GC, it never appears as a
// first-class object in Foxely.
class ObjectModule : public Object
{
public:
    explicit ObjectModule(VM& oVM, ObjectString* strName)
        : m_oVM(oVM), m_strName(strName)
	{
		type = OBJ_MODULE;
	}

    bool operator==(const ObjectModule& other) const
    {
        return m_strName == other.m_strName;
    }

    template <typename R, typename... Args>
    inline void func(const std::string& name, R (*callback)(Args...))
	{
		auto function = [this, callback] (VM* vm, int ac, Value* av) -> Value
		{
            auto tuple = utils::args<Args...>(ac, av);
            return Value(
                utils::apply_function<std::tuple_size<decltype(tuple)>::value>
                    ::apply(callback, tuple));
        };
        define_func(name, function);
    }

    template <typename... Args>
    inline void func(const std::string& name, void (*callback)(Args...))
	{
		auto function = [this, callback] (VM* vm, int ac, Value* av) -> Value
		{
            auto tuple = utils::args<Args...>(ac, av);
            utils::apply_function<std::tuple_size<decltype(tuple)>::value>::apply(callback, tuple);
			return Fox_Nil;
        };
        define_func(name, function);
    }

    template <typename R>
    inline void func(const std::string& name, R (*callback)())
	{
		auto function = [callback] (VM* vm, int ac, Value* av) -> Value
		{
            return Value((*callback)());
        };
        define_func(name, function);
    }

    inline void func(const std::string& name, void (*callback)())
	{
		auto function = [callback] (VM* vm, int ac, Value* av) -> Value
		{
			(*callback)();
			return Fox_Nil;
		};
        define_func(name, function);
	}

    void raw_func(const std::string& name, NativeFn func)
	{
        define_func(name, func);
    }

    template<typename T>
    inline Klass<T>* klass(const std::string& name);

    void raw_klass(const std::string& name, NativeMethods& methods);

private:

    void define_func(const std::string& name, NativeFn func);

private:
    VM& m_oVM;

public:
    Table m_vVariables;
    // The name of the module.
    ObjectString* m_strName;
};

class ObjectFunction : public Object
{
public:
    int arity;
    int iMinArity;
    int iMaxArity;
    int upValueCount;
    Chunk chunk;
    ObjectString* name;
    ObjectModule* module;

	explicit ObjectFunction()
	{
		arity = 0;
		name = NULL;
		upValueCount = 0;
		type = OBJ_FUNCTION;
        module = NULL;
        iMinArity = 0;
        iMaxArity = 0;
	}
};

class ObjectUpvalue : public Object
{
public:
    Value *location;
    Value closed;
    ObjectUpvalue* next;

    explicit ObjectUpvalue(Value* slot)
    {
        type = OBJ_UPVALUE;
        location = slot;
        next = NULL;
        closed = Fox_Nil;
    }
};


class ObjectNative : public Object
{
public:
    NativeFn function;

	explicit ObjectNative(NativeFn func)
	{
		function = func;
		type = OBJ_NATIVE;
	}

    explicit ObjectNative(NativeFn func, int a)
	{
		function = func;
		type = OBJ_NATIVE;
	}
};

class ObjectLib : public Object
{
public:
    ObjectString* name;
    Table methods;

	explicit ObjectLib(ObjectString* n)
	{
		type = OBJ_LIB;
		name = NULL;
		name = n;
		methods = Table();
	}
};

class ObjectClosure : public Object
{
public:
    ObjectFunction* function;
    std::vector<ObjectUpvalue*> upValues;
    int upvalueCount;

    ObjectClosure(VM* oVM, ObjectFunction* func);
};

class ObjectClass : public Object
{
public:
    ObjectClass* superClass;
    ObjectString* name;
    Table methods;
    Table operators;
    Table getters;
    Table setters;
    Table fields;
    int derivedCount;

	explicit ObjectClass(ObjectString* n)
	{
		type = OBJ_CLASS;
		name = n;
		methods = Table();
        superClass = NULL;
        derivedCount = 0;
	}

    bool operator==(const ObjectClass& other) const
    {
        ObjectClass* cl = (ObjectClass*) this;
        ObjectClass* end = (ObjectClass*) &other;
        if (derivedCount < other.derivedCount)
        {
            cl = (ObjectClass*) &other;
            end = (ObjectClass*) this;
        }
        while (cl && !(cl == end))
            cl = cl->superClass;
        return cl != NULL;
    }
};

class ObjectInstance : public Object
{
public:
    ObjectClass* klass;
    Table fields;
    VM* m_pVm;
	void* user_type;

	explicit ObjectInstance(VM* pVm, ObjectClass* k)
	{
		type = OBJ_INSTANCE;
		klass = k;
		fields.AddAll(klass->fields);
        m_pVm = pVm;
        user_type = nullptr;
	}

    void on_destroy() override;

    bool operator==(const ObjectInstance& other) const
    {
        return *klass == *other.klass && fields.m_vEntries == other.fields.m_vEntries;
    }
};

template <typename T>
class Klass : public ObjectClass
{
public:
    VM& m_oVM;
    ObjectModule& m_oModule;

	explicit Klass(VM& oVM, ObjectString* n, ObjectModule& module) : ObjectClass(n), m_oVM(oVM), m_oModule(module)
	{
        ctor();

        dtor();
	}

    ~Klass()
    {
    }

    /* --------------  DEFINE METHODS API ------------------ */
    template <typename R, typename... Args>
    inline void func(const std::string& name, R (T::*callback)(Args...))
	{
		auto function = [callback] (VM* vm, int ac, Value* av) -> Value
		{
            auto tuple = utils::args<Args...>(ac, av);
            return Value(
                utils::apply_method<std::tuple_size<decltype(tuple)>::value>
                    ::apply(utils::argp<T>(ac, av, -1), callback, tuple));
        };
        define_method(name, function);
    }

    template <typename... Args>
    inline void func(const std::string& name, void (T::*callback)(Args...))
	{
		auto function = [callback] (VM* vm, int ac, Value* av) -> Value
		{
            auto tuple = utils::args<Args...>(ac, av);
            utils::apply_method<std::tuple_size<decltype(tuple)>::value>::apply(utils::argp<T>(ac, av, -1), callback, tuple);
			return Fox_Nil;
        };
        define_method(name, function);
    }

    template <typename R>
    inline void func(const std::string& name, R (T::*callback)())
	{
		auto function = [callback] (VM* vm, int ac, Value* av) -> Value
		{
            return Value((utils::argp<T>(ac, av, -1)->*callback)());
        };
        define_method(name, function);
    }

    inline void func(const std::string& name, void (T::*callback)())
	{
		auto function = [callback] (VM* vm, int ac, Value* av) -> Value
		{
            (utils::argp<T>(ac, av, -1)->*callback)();
			return Fox_Nil;
		};
        define_method(name, function);
	}

    void raw_func(const std::string& name, NativeFn func)
	{
        define_method(name, func);
    }

    template<typename varType>
    void prop(const std::string& name, varType (T::*gettter)() const);
    template<typename TVar>
    void prop(const std::string& name, TVar (T::*gettter)() const, void (T::*settter)(TVar));
    template<typename varType>
    void var(const std::string& name, varType T::*variable);

    /* ------------------------------------------------------ */

    // template <typename... Args>
    // void init(ObjectInstance* pInstance, Args... args)
    // {
    //     std::cout << "Init Args" << std::endl;
    //     pInstance->user_type = new T(args...);
    // }

    // template <typename... Args>
    // void ctor()
	// {
    //     auto callback = [this](ObjectInstance* pInstance, Args... args)
	// 	{
    //         init(pInstance, args...);
    //     };
    //     auto function = [this, callback](VM* vm, int ac, Value* av) -> Value
	// 	{
    //         std::cout << "Init Args..." << std::endl;
    //         auto tuple = utils::args<ObjectInstance*, Args...>(ac, av, -1);
    //         utils::apply_function<std::tuple_size<decltype(tuple)>::value>::apply(init, tuple);
    //         return av[-1];
    //     };
    //     define_method("init", function);
    // }

    // bool operator==(const Klass& other) const
    // {
    //     return user_type == other.user_type;
    // }

private:
    void define_method(const std::string& name, NativeFn func);

    void ctor(); // default constructor
    void dtor(); // default destructor
};

class ObjectBoundMethod : public Object
{
public:
    Value receiver;
    ObjectClosure* method;

	explicit ObjectBoundMethod(Value r, ObjectClosure* m)
	{
		type = OBJ_BOUND_METHOD;
		receiver = r;
		method = m;
	}
};

struct ObjectAbstractType
{
    const char *name;
};

class ObjectAbstract : public Object
{
public:
    ObjectAbstractType* abstractType;
    void* data;

    explicit ObjectAbstract()
	{
		type = OBJ_ABSTRACT;
        abstractType = NULL;
        data = NULL;
	}

    explicit ObjectAbstract(void* d, ObjectAbstractType* aType)
	{
		type = OBJ_ABSTRACT;
        abstractType = aType;
        data = d;
	}

    bool operator==(const ObjectAbstract& other) const
    {
        return data == other.data && abstractType == other.abstractType;
    }
};

class ObjectArray : public Object
{
public:
    std::vector<Value> m_vValues;

    explicit ObjectArray()
	{
		type = OBJ_ARRAY;
        m_vValues = std::vector<Value>();
	}

    bool operator==(const ObjectArray& other) const
    {
        return m_vValues == other.m_vValues;
    }
};

class ObjectMap : public Object
{
public:
    MapTable m_vValues;
    Table m_oMethods;

    explicit ObjectMap()
	{
		type = OBJ_MAP;
        m_vValues = MapTable();
	}

    bool operator==(const ObjectMap& other) const
    {
        return m_vValues.m_vEntries == other.m_vValues.m_vEntries;
    }
};

static inline bool is_obj_type(Value val, ObjType type)
{
    return Fox_IsObject(val) &&
            Fox_AsObject(val)->type == type;
}

#define UINT8_COUNT (UINT8_MAX + 1)
#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

struct CallFrame
{
	ObjectClosure* closure;
	std::vector<uint8_t>::iterator ip;
	Value* slots;
};

class ObjectFiber : public Object
{
public:
    explicit ObjectFiber(ObjectClosure* pClosure)
	{
		type = OBJ_FIBER;
        m_pStackTop = m_vStack;
        m_iFrameCount = 0;

        m_vOpenUpvalues = nullptr;
        m_pCaller = nullptr;
        m_oError = Fox_Nil;
        // fiber->state = FIBER_OTHER;
        
        if (pClosure != nullptr)
        {
            // Initialize the first call frame.
            CallFrame *pFrame = &m_vFrames[m_iFrameCount++];
            pFrame->closure = pClosure;
            pFrame->ip = pClosure->function->chunk.m_vCode.begin();

            // The first slot always holds the closure.
            *m_pStackTop = pClosure;
            m_pStackTop++;
            pFrame->slots = m_pStackTop - pClosure->function->arity - 1;
        }
	}

    void SetSlot(int iArgCount)
    {
        CallFrame *pFrame = &m_vFrames[m_iFrameCount];
        pFrame->slots = m_pStackTop - iArgCount - 1;
    }

    // The stack of value slots. This is used for holding local variables and
    // temporaries while the fiber is executing. It is heap-allocated and grown
    // as needed.
    Value m_vStack[STACK_MAX];
    
    // A pointer to one past the top-most value on the stack.
    Value* m_pStackTop;
    
    // The stack of call frames. This is a dynamic array that grows as needed but
    // never shrinks.
    CallFrame m_vFrames[FRAMES_MAX];
    
    // The number of frames currently in use in [frames].
    int m_iFrameCount;
    
    // Pointer to the first node in the linked list of open upvalues that are
    // pointing to values still on the stack. The head of the list will be the
    // upvalue closest to the top of the stack, and then the list works downwards.
    ObjectUpvalue* m_vOpenUpvalues;
    
    // The fiber that ran this one. If this fiber is yielded, control will resume
    // to this one. May be `NULL`.
    ObjectFiber* m_pCaller;
    
    // If the fiber failed because of a runtime error, this will contain the
    // error object. Otherwise, it will be null.
    Value m_oError;
    
    // FiberState state;
};



/* --------- Utils Impl---------------------------------------------- */

template <>
struct utils::apply_function<0> {
    template <typename R, typename... FuncArgs, typename... TupleArgs, typename... Args>
    static R apply(R (*func)(FuncArgs...), std::tuple<TupleArgs...>& t, Args... args)
    {    
		return (*func)(args...);
    }
};

template <>
struct utils::apply_method<0> {
    template <class T, typename R, typename... MethodArgs, typename... TupleArgs, typename... Args>
    static R apply(T* o, R (T::*method)(MethodArgs...),
        std::tuple<TupleArgs...>& t, Args... args) {
        return (o->*method)(args...);
    }
};

template <>
std::string utils::arg<std::string>(int ac, Value* av, const int i);

template <>
double utils::arg<double>(int ac, Value* av, const int i);

template <>
bool utils::arg<bool>(int ac, Value* av, const int i);

template <>
int utils::arg<int>(int ac, Value* av, const int i);

template <class T>
T* utils::argp(int ac, Value* av, const int i)
{
    if (i < -1 && i > ac)
        throw std::runtime_error("csvsdvdsv");
    
    if (!Fox_IsInstance(av[i]))
        throw std::runtime_error("Invalid Type");
    ObjectInstance* user = Fox_AsInstance(av[i]);
    return static_cast<T*>(user->user_type);
}

/* ------------------------------------------------------------------- */

/* --------- Value Impl---------------------------------------------- */
template <typename T,
    std::enable_if_t<!std::is_base_of<Object, T>::value, bool>>
T Value::as()
{
    if (type != VAL_OBJ)
        throw std::runtime_error("Invalid Type");
    if (!is_obj_type(*this, OBJ_INSTANCE))
        throw std::runtime_error("Invalid Type");
    ObjectInstance* user = static_cast<ObjectInstance*>(val.obj);
    return *static_cast<T*>(user->user_type);
}

template <typename T,
    std::enable_if_t<std::is_base_of<Object, T>::value, bool>>
T* Value::as()
{
    // if (type != VAL_OBJ)
    //     throw std::runtime_error("Invalid Type");
    return static_cast<T*>(val.obj);
}

template<typename T>
bool Value::is()
{
    return false;
}
/* ------------------------------------------------------------------- */

#endif