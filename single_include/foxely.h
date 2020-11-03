#pragma once

// #define FOXELY_API __attribute__((visibility ("default")))
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "linenoise.hpp"

inline bool IsRepl;

class GC;
class Parser;
class Compiler;
class VM;
class Table;
class Object;
class ObjectFunction;
class ObjectArray;
class ObjectClosure;
class ObjectString;
class ObjectUpvalue;
class ObjectNative;
class ObjectNativeClass;
class ObjectLis;
class ObjectClass;
class ObjectInstance;
class ObjectNativeInstance;
class ObjectBoundMethod;


typedef Value (*NativeFn)(VM* oVM, int arg_count, Value *args);


// ---------------------------
// 		Plugin
// ---------------------------

// Shared library exports
#if defined(SCY_WIN) && defined(SCY_SHARED_LIBRARY)
    #if defined(Pluga_EXPORTS)
        #define Pluga_API __declspec(dllexport)
    #else
        #define Pluga_API __declspec(dllimport)
    #endif
#else
    #define Pluga_API // nothing
#endif


namespace fox {
namespace pluga {

	// Forward declare the plugin class which must be defined externally.
	class Pluga_API IModule;

	// Define the API version.
	// This value is incremented whenever there are ABI breaking changes.
	#define SCY_PLUGIN_API_VERSION 1

	#ifdef SCY_WIN
	#define SCY_PLUGIN_EXPORT __declspec(dllexport)
	#else
	#define SCY_PLUGIN_EXPORT // empty
	#endif

	// Define a type for the static function pointer.
	Pluga_API typedef IModule* (*GetPluginFunc)();

	// Plugin details structure that's exposed to the application.
	struct PluginDetails
	{
		int apiVersion;
		const char* fileName;
		const char* className;
		const char* pluginName;
		const char* pluginVersion;
		GetPluginFunc initializeFunc;
	};

	#define SCY_STANDARD_PLUGIN_STUFF SCY_PLUGIN_API_VERSION, __FILE__

	#define SCY_PLUGIN(classType, pluginName, pluginVersion)                       \
    extern "C" {                                                               \
		SCY_PLUGIN_EXPORT fox::pluga::IModule* getPlugin()                         \
		{                                                                          \
			static classType singleton;                                            \
			return &singleton;                                                     \
		}                                                                          \
		SCY_PLUGIN_EXPORT fox::pluga::PluginDetails exports = {                    \
			SCY_STANDARD_PLUGIN_STUFF,                                             \
			#classType,                                                            \
			pluginName,                                                            \
			pluginVersion,                                                         \
			getPlugin,                                                             \
		};                                                                         \
    }

		// NOTE: When using STL containers and other complex types you
		// will need to ensure plugins are always built using the same
		// compiler, since system libraries aren't ABI compatible.
		#define PLUGA_ENABLE_STL 1


		/// Virtual base class for plugins
		class IModule
		{
		public:
			NativeMethods m_oMethods;
			VM* oVM;

			IModule(VM* oVM) {};
			virtual ~IModule() {};

			virtual const char* GetClassName() const { return NULL; };
		};
    } // namespace pluga

	
    struct lib_t
    {
        void* handle;
        char* errmsg;
    };
    typedef lib_t lib_t;

    static inline int fox__dlerror(lib_t* lib);


    static inline int fox_dlopen(const char* filename, lib_t* lib)
    {
        dlerror(); /* Reset error status. */
        lib->errmsg = NULL;
        lib->handle = dlopen(filename, RTLD_LAZY);
        return lib->handle ? 0 : fox__dlerror(lib);
    }


    static inline void fox_dlclose(lib_t* lib)
    {
        free(lib->errmsg);
        lib->errmsg = NULL;

        if (lib->handle) {
            /* Ignore errors. No good way to signal them without leaking memory. */
            dlclose(lib->handle);
            lib->handle = NULL;
        }
    }


    static inline int fox_dlsym(lib_t* lib, const char* name, void** ptr)
    {
        dlerror(); /* Reset error status. */
        *ptr = dlsym(lib->handle, name);
        return fox__dlerror(lib);
    }


    static inline const char* fox_dlerror(const lib_t* lib)
    {
        return lib->errmsg ? lib->errmsg : "no error";
    }


    static int fox__dlerror(lib_t* lib)
    {
        const char* errmsg;

        free(lib->errmsg);

        errmsg = dlerror();

        if (errmsg)
        {
            lib->errmsg = strdup(errmsg);
            return -1;
        }
        else
        {
            lib->errmsg = NULL;
            return 0;
        }
    }


    struct SharedLibrary
    {
        /// Opens a shared library.
        /// The filename is in utf-8. Returns true on success and false on error.
        /// Call `SharedLibrary::error()` to get the error message.
        bool open(const std::string& path)
        {
            if (fox_dlopen(path.c_str(), &_lib)) {
                setError("Cannot load library");
                return false;
            }
            m_strLibname = path;
            return true;
        }

        /// Closes the shared library.
        void close()
        {
            fox_dlclose(&_lib);
        }

        /// Retrieves a data pointer from a dynamic library.
        /// It is legal for a symbol to map to nullptr.
        /// Returns 0 on success and -1 if the symbol was not found.
        bool sym(const char* name, void** ptr)
        {
            if (fox_dlsym(&_lib, name, ptr)) {
                setError("Symbol '%s' not found." + std::string(name));
                return false;
            }
            return true;
        }

        void setError(const std::string& prefix)
        {
            std::string err(fox_dlerror(&_lib));
            if (err.empty())
                return;
            _error = prefix + ": " + err;
            throw std::runtime_error(prefix + ": " + err);
        }

        std::string error() const { return _error; }

        bool operator==(const SharedLibrary& other) const
        {
            return m_strLibname == other.m_strLibname;
        }

    protected:
        lib_t _lib;
        std::string _error;
    
    public:
        std::string m_strLibname;
    };
} // namespace fox

// ---------------------------
// 		Chunk
// ---------------------------

typedef enum {
    OP_CONST,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    OP_GET_SUPER,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_IMPORT,
    OP_CALL,
    OP_INVOKE,
    OP_SUPER_INVOKE,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_CLASS,
    OP_INHERIT,
    OP_METHOD,
    OP_INTERFACE,
    OP_INTERFACE_PROCEDURE,

    OP_SUBSCRIPT,
    OP_ARRAY,
    OP_ADD_LIST,

    // CODES for Repl Mode
    OP_PRINT_REPL,
} OpCode;

class Chunk
{
public:
    int m_iCount;
    int m_iCapacity;
    std::vector<uint8_t> m_vCode;
    ValueArray m_oConstants;
    std::vector<int> m_vLines;

    Chunk();
    void WriteChunk(uint8_t byte, int line);
    int AddConstant(Value value);
};

// ---------------------------
// 		Value
// ---------------------------

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

#define IS_BOOL(val)    ((val).type == VAL_BOOL)
#define IS_NIL(val)     ((val).type == VAL_NIL)
#define IS_NUMBER(val)  ((val).type == VAL_NUMBER)
#define IS_INT(val)     ((val).type == VAL_INT)
#define IS_OBJ(val)     ((val).type == VAL_OBJ)

#define AS_OBJ(val)     ((val).as.obj)
#define AS_BOOL(val)    ((val).as.boolean)
#define AS_NUMBER(val)  ((val).as.number)
#define AS_INT(val)  ((val).as.integer)

#define BOOL_VAL(val)   ((Value){ VAL_BOOL, { .boolean = val } })
#define NIL_VAL           ((Value){ VAL_NIL, { .number = 0 } })
#define NUMBER_VAL(val) ((Value){ VAL_NUMBER, { .number = val } })
#define INT_VAL(val) ((Value){ VAL_INT, { .integer = val } })
#define OBJ_VAL(object)   ((Value){ VAL_OBJ, { .obj = (Object *)object } })


#define OBJ_TYPE(val)         (AS_OBJ(val)->type)


#define IS_ARRAY(val)        is_obj_type(val, OBJ_ARRAY)
#define IS_ABSTRACT(val)        is_obj_type(val, OBJ_ABSTRACT)
#define IS_LIB(val)           is_obj_type(val, OBJ_LIB)
#define IS_BOUND_METHOD(val)  is_obj_type(val, OBJ_BOUND_METHOD)
#define IS_CLASS(val)         is_obj_type(val, OBJ_CLASS)
#define IS_NATIVE_CLASS(val)         is_obj_type(val, OBJ_NATIVE_CLASS)
#define IS_LIB(val)         is_obj_type(val, OBJ_LIB)
#define IS_CLOSURE(val)       is_obj_type(val, OBJ_CLOSURE)
#define IS_INSTANCE(val)      is_obj_type(val, OBJ_INSTANCE)
#define IS_NATIVE_INSTANCE(val)      is_obj_type(val, OBJ_NATIVE_INSTANCE)
#define IS_INTERFACE(val)      is_obj_type(val, OBJ_INTERFACE)
#define IS_FUNCTION(val)      is_obj_type(val, OBJ_FUNCTION)
#define IS_NATIVE(val)        is_obj_type(val, OBJ_NATIVE)
#define IS_STRING(val)        is_obj_type(val, OBJ_STRING)

#define AS_ARRAY(val)           ((ObjectArray *)AS_OBJ(val))
#define AS_ABSTRACT(val)        ((ObjectAbstract *)AS_OBJ(val))
#define AS_LIB(val)           	((ObjectLib *)AS_OBJ(val))
#define AS_BOUND_METHOD(val)  	((ObjectBoundMethod *)AS_OBJ(val))
#define AS_CLASS(val)         	((ObjectClass *)AS_OBJ(val))
#define AS_NATIVE_CLASS(val)	((ObjectNativeClass *)AS_OBJ(val))
#define AS_LIB(val)         	((ObjectLib *)AS_OBJ(val))
#define AS_CLOSURE(val)       	((ObjectClosure *)AS_OBJ(val))
#define AS_FUNCTION(val)      	((ObjectFunction *)AS_OBJ(val))
#define AS_INSTANCE(val)        ((ObjectInstance *)AS_OBJ(val))
#define AS_NATIVE_INSTANCE(val) ((ObjectNativeInstance *)AS_OBJ(val))
#define AS_NATIVE(val)        	(((ObjectNative *)AS_OBJ(val))->function)
#define AS_INTERFACE(val)       ((ObjectInterface *)AS_OBJ(val))
#define AS_STRING(val)        	((ObjectString *)AS_OBJ(val))
#define AS_CSTRING(val)       	(((ObjectString *)AS_OBJ(val))->string.c_str())

typedef enum {
    OBJ_ARRAY,
    OBJ_ABSTRACT,
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE_INSTANCE,
    OBJ_NATIVE,
    OBJ_NATIVE_CLASS,
    OBJ_LIB,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_INTERFACE,
} ObjType;



class Object : public Traceable
{
public:
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

class ObjectFunction : public Object
{
public:
    int arity;
    int upValueCount;
    Chunk chunk;
    ObjectString *name;

	explicit ObjectFunction()
	{
		arity = 0;
		name = NULL;
		upValueCount = 0;
		type = OBJ_FUNCTION;
	}
};

class ObjectUpvalue : public Object
{
public:
    Value *location;
    Value closed;
    struct ObjectUpvalue *next;

    explicit ObjectUpvalue(Value* slot)
    {
        type = OBJ_UPVALUE;
        location = slot;
        next = NULL;
        closed = NIL_VAL;
    }
};

typedef Value (*NativeFn)(VM* oVM, int arg_count, Value *args);

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
    ObjectString *name;
    Table methods;

	explicit ObjectLib(ObjectString* n)
	{
		type = OBJ_LIB;
		name = NULL;
		name = n;
		methods = Table();
	}
};

class ObjectNativeClass : public Object
{
public:
    ObjectString *name;
    Table methods;

	explicit ObjectNativeClass(ObjectString* n)
	{
		type = OBJ_NATIVE_CLASS;
		name = n;
		methods = Table();
	}
};

class ObjectNativeInstance : public Object
{
public:
    ObjectNativeClass *klass;
    Table fields;
	void* cStruct;

	explicit ObjectNativeInstance(ObjectNativeClass *k)
	{
		type = OBJ_NATIVE_INSTANCE;
		klass = k;
		fields = Table();
		cStruct = NULL;
	}

	explicit ObjectNativeInstance(ObjectNativeClass *k, void* c)
	{
		type = OBJ_NATIVE_INSTANCE;
		klass = k;
		fields = Table();
		cStruct = c;
	}

    bool operator==(const ObjectNativeInstance& other) const
    {
        return klass == other.klass && fields.m_vEntries == other.fields.m_vEntries;
    }
};

class ObjectClosure : public Object
{
public:
    ObjectFunction *function;
    ObjectUpvalue **upValues;
    int upvalueCount;

    ObjectClosure(VM* oVM, ObjectFunction *func);
};

class ObjectClass : public Object
{
public:
    ObjectClass* superClass;
    ObjectString *name;
    Table methods;
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
    ObjectClass *klass;
    Table fields;

	explicit ObjectInstance(ObjectClass *k)
	{
		type = OBJ_INSTANCE;
		klass = k;
		fields = Table();
	}

    bool operator==(const ObjectInstance& other) const
    {
        return *klass == *other.klass && fields.m_vEntries == other.fields.m_vEntries;
    }
};

class ObjectInterface : public Object
{
public:
    ObjectString *name;
    Table methods;

	explicit ObjectInterface(ObjectString* n)
	{
		type = OBJ_INTERFACE;
		name = n;
		methods = Table();
	}

    // bool operator==(const ObjectClass& other) const
    // {
    //     ObjectClass* cl = (ObjectClass*) this;
    //     ObjectClass* end = (ObjectClass*) &other;
    //     if (derivedCount < other.derivedCount)
    //     {
    //         cl = (ObjectClass*) &other;
    //         end = (ObjectClass*) this;
    //     }
    //     while (cl && !(cl == end))
    //         cl = cl->superClass;
        
    //     return cl != NULL;
    // }
};

class ObjectBoundMethod : public Object
{
public:
    Value receiver;
    ObjectClosure *method;

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
    Table methods;

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

static inline bool is_obj_type(Value val, ObjType type)
{
    return IS_OBJ(val) && AS_OBJ(val)->type == type;
}

// ---------------------------
// 		Parser
// ---------------------------

class ParserHelper
{
private:

    Token oCurrentToken;

    enum token_advance_mode
    {
        e_hold    = 0,
        e_advance = 1
    };

protected:
    Lexer oLexer;
public:

    Token oPreviousToken;
    bool hadError;
    bool panicMode;

    bool Init(const std::string& str);
    void Define(const std::string& strId, const std::string& strValue, bool bAddInTrash = false);

    Lexer& GetLexer();
    const Lexer& GetLexer() const;

    void NextToken();

    const Token& CurrentToken() const;
    const Token& PreviousToken() const;

    void AdvanceToken(token_advance_mode mode);

	bool IsToken(int iType, bool bAdvance = true);
    bool IsToken(const std::string& strValue, bool bAdvance = true);
    bool IsToken(const std::string& strType, const std::string& strValue, bool bAdvance = true);
    bool IsToken(const std::string& strType, const char *strValue, bool bAdvance = true);
	bool IsToken(int iType, const std::string& strValue, bool bAdvance = true);
	bool IsToken(int iType, const char *strValue, bool bAdvance = true);
    bool IsTokenType(const std::string &strType, bool bAdvance = true);

    bool IsTokenThenAssign(const std::string& strType, Token& oToken, bool bAdvance = true);
    bool IsTokenThenAssign(const std::string& strType, const char *strValue, Token& oToken, bool bAdvance);
    template <typename Allocator, template <typename, typename> class Container>
    bool IsTokenThenAssign(const std::string& strType, Container<Token&, Allocator>& oTokenList, bool bAdvance = true);
    template <typename Allocator, template <typename, typename> class Container>
    bool IsTokenThenAssign(const std::string& strType, const char *strValue, Container<Token&, Allocator>& oTokenList, bool bAdvance);

	bool IsTokenThenAssign(int iType, Token& oToken, bool bAdvance = true);
    bool IsTokenThenAssign(int iType, const char *strValue, Token& oToken, bool bAdvance);
    template <typename Allocator, template <typename, typename> class Container>
    bool IsTokenThenAssign(int iType, Container<Token&, Allocator>& oTokenList, bool bAdvance = true);
    template <typename Allocator, template <typename, typename> class Container>
    bool IsTokenThenAssign(int iType, const char *strValue, Container<Token&, Allocator>& oTokenList, bool bAdvance);

    bool PeekTokenIsType(const std::string& strType);
    bool PeekTokenIsType(int iType);
    bool PeekNextTokenIsType(int iType);
    bool PeekTokenIs(const std::string& strValue);
    void ErrorAtCurrent(const char* message);
    void Error(const char* message);
    void ErrorAt(const Token& token, const char* message);
};


#define UINT8_COUNT (UINT8_MAX + 1)

// class ObjectFunction;
class VM;
class ObjectString;
class ObjectFunction;

typedef enum {
    // Single-character tokens.
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // One or two character tokens.
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

	TOKEN_COLON, TOKEN_DOUBLE_COLON,

    // Literals.
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
    TOKEN_INT,

    // Keywords.
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE, TOKEN_IMPORT,
    TOKEN_INTERFACE, TOKEN_FOREIGN,

    TOKEN_SINGLE_COMMENT,
    TOKEN_MULTI_COMMENT,
    TOKEN_WS,
    TOKEN_MAX,
    TOKEN_EOF = -1,
    TOKEN_NEW_LINE = 83,
    TOKEN_ERROR = 84,
    TOKEN_SHEBANG,
} TokenType;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY,
} Precedence;

typedef struct {
    Token name;
    int depth;
    bool isCaptured;
} Local;

struct Upvalue {
    uint8_t index;
    bool isLocal;
};

typedef enum {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_SCRIPT
} FunctionType;

typedef void (*parse_fn)(Parser& parser, bool can_assign);

struct ParseRule
{
    parse_fn prefix;
    parse_fn infix;
    Precedence precedence;
};

struct ClassCompiler
{
	explicit ClassCompiler(Token& n) : name(n) {}
   	ClassCompiler *enclosing;
    Token& name;
    bool hasSuperclass;
};

class Parser : public ParserHelper
{
public:
	Chunk* compilingChunk;
    ParseRule rules[TOKEN_MAX];
	VM* m_pVm;
    Compiler *currentCompiler;
	ClassCompiler* currentClass;

    Parser();
	bool IsEnd();
    void Advance();
    void MarkInitialized();
    void Consume(int oType, const char* message);
	Chunk* GetCurrentChunk();
	void SetCurrentChunk(Chunk& chunk);

	ObjectString* CopyString(const std::string& value);
	ObjectString* TakeString(const std::string& value);
	ObjectString* AllocateString(const std::string& str, uint32_t hash);
	uint32_t hashString(const std::string& str);


	void EmitByte(uint8_t byte);
	void EmitBytes(uint8_t byte1, uint8_t byte2);
	void EmitConstant(Value value);
	void EmitReturn();
	int EmitJump(uint8_t instruction);
	void PatchJump(int offset);
	void EmitLoop(int loopStart);
	uint8_t MakeConstant(Value value);

	ObjectFunction* EndCompiler();


    bool Match(int type);
    ParseRule *GetRule(int type);

	uint8_t IdentifierConstant(const Token& name);

	void BeginScope();
	void EndScope();
};

struct Compiler
{
public:
	Compiler (Parser& parser, FunctionType eType, const std::string& name);

    Compiler* enclosing;
    ObjectFunction* function;
    FunctionType type;
    Local locals[UINT8_COUNT];
    int localCount;
    Upvalue upvalues[UINT8_COUNT];
    int scopeDepth;
};

ObjectFunction* Compile(Parser& parser, const std::string &strText, Chunk* chunk);
void Expression(Parser& parser);
void ParsePrecedence(Parser& parser, Precedence preced);
void Number(Parser& parser, bool can_assign = false);
void IntNumber(Parser& parser, bool can_assign = false);
void Grouping(Parser& parser, bool can_assign = false);
void Unary(Parser& parser, bool can_assign = false);
void Binary(Parser& parser, bool can_assign = false);
void Literal(Parser& parser, bool can_assign = false);
void RuleAnd(Parser& parser, bool can_assign = false);
void RuleOr(Parser& parser, bool can_assign = false);
void Dot(Parser& parser, bool can_assign = false);
void RuleThis(Parser& parser, bool can_assign = false);
void RuleSuper(Parser& parser, bool can_assign = false);
void Variable(Parser& parser, bool can_assign = false);
void String(Parser& parser, bool can_assign = false);
void CallCompiler(Parser& parser, bool can_assign = false);
void Subscript(Parser& parser, bool canAssign = false);
void List(Parser& parser, bool canAssign = false);

void Block(Parser& parser);
void Declaration(Parser& parser);
void Synchronize(Parser& parser);
void VarDeclaration(Parser& parser, Token name);
// void VarDeclaration(Parser& parser);
void NamedVariable(Parser& parser, Token name, bool can_assign);
void AddLocal(Parser& parser, Token name);
bool IdentifiersEqual(Token& a, Token& b);
int ResolveLocal(Parser& parser, Compiler *comp, Token& name);

void Function(Parser& parser, FunctionType type, const Token& name);
void FuncDeclaration(Parser& parser, Token name);

void InterfaceDeclaration(Parser& parser, Token name);
void Interface(Parser& parser, FunctionType type, const Token& name);

int ResolveUpvalue(Parser& parser, Compiler *comp, Token& name);
void ClassDeclaration(Parser& parser, Token& name);
uint8_t ArgumentList(Parser& parser);
void DeclareVariable(Parser& parser, Token name);
void DefineVariable(Parser& parser, uint8_t global);



// ---------------------------
// 		Table
// ---------------------------

#define TABLE_MAX_LOAD (0.75)

struct Entry
{
    ObjectString* m_pKey;
    Value m_oValue;

	bool operator==(const Entry& other) const
    {
        return m_pKey == other.m_pKey && m_oValue == other.m_oValue;
    }
};

class Table
{
public:
	Table();
    int m_iCount;
    int m_iCapacity;
	std::vector<Entry> m_vEntries;

	void AdjustCapacity(int capacity);
	Entry& FindEntry(ObjectString* pKey);
	bool Set(ObjectString *key, Value value);
	void AddAll(Table& from);
	bool Get(ObjectString *key, Value& value);
	bool Delete(ObjectString *key);
	ObjectString *FindString(const char *chars, int length, uint32_t hash);
	ObjectString *FindString(const std::string& string, uint32_t hash);
	void MarkTable();
	void RemoveWhite();
	void Print();
};

// ---------------------------
// 		Statements
// ---------------------------

void Statement(Parser& parser);
void PrintStatement(Parser& parser);
void ExpressionStatement(Parser& parser);
void ForStatement(Parser& parser);
void WhileStatement(Parser& parser);
void for_loop(Parser& parser, int *exit_jump);
void for_increment(Parser& parser, int *loop_start);
void ImportStatement(Parser& parser);

// ---------------------------
// 		VM
// ---------------------------

#define UINT8_COUNT (UINT8_MAX + 1)
#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

struct CallFrame {
	ObjectClosure* closure;
	std::vector<uint8_t>::iterator ip;
	Value* slots;
};

typedef enum
{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;

using NativeMethods = std::map<std::string, NativeFn>;

class VM
{
public:
	CallFrame frames[FRAMES_MAX];
  	int frameCount;

    Chunk m_oChunk;
    std::vector<uint8_t>::iterator ip;
    Value stack[STACK_MAX];
    Value* stackTop;
    Parser m_oParser;
	Table strings;
	Table globals;
	Table imports;
	ObjectUpvalue* openUpvalues;
	ObjectString* initString;
	GC gc;

    Table arrayMethods;

	int argc;
	char** argv;

    VM();
	~VM();

	// static VM* GetInstance();

	void Load();
	void LoadStandard(const std::string& name);

    InterpretResult Interpret(const char* module, const char* source);

    void ResetStack();
    void Push(Value value);
    Value Pop();
    Value Peek(int distance);
	Value PeekStart(int distance);
    void RuntimeError(const char* format, ...);

    void EmitByte(uint8_t byte);
    void Concatenate();
	bool CallValue(Value callee, int argCount);
	bool Call(ObjectClosure* closure, int argCount);
	void DefineNative(const std::string& name, NativeFn function);
	void DefineNativeClass(const std::string& name, NativeMethods& functions);
	void DefineLib(const std::string &name, NativeMethods &functions);
	void DefineBuiltIn(Table& methods, NativeMethods &functions);

	ObjectUpvalue* CaptureUpvalue(Value* local);
	void CloseUpvalues(Value* last);

	void AddToRoots();
	void AddTableToRoot(Table& table);
	void AddValueToRoot(Value value);
	void AddObjectToRoot(Object* object);
	void AddCompilerToRoots();
	void AddArrayToRoot(ValueArray* array);
	void BlackenObject(Object* object);
	void DefineMethod(ObjectString* name);
	bool BindMethod(ObjectClass* klass, ObjectString* name);
	bool Invoke(ObjectString* name, int argCount);
	bool InvokeFromClass(ObjectClass* klass, ObjectString* name, int argCount);
	bool InvokeFromNativeClass(ObjectNativeClass *klass, ObjectString *name, int argCount);

private:
    InterpretResult run();
	static VM m_oInstance;

	InterpretResult result;
	bool isInit;
};

static Value clockNative(VM* oVM, int argCount, Value* args)
{
	return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}


// ---------------------------
// 		VM
// ---------------------------

class GC
{
	typedef std::set<Traceable*> ObjectSet;
	ObjectSet m_oRoots;
	/**
	* Objects tracing information: allocation pointer to header.
	*/
	ObjectSet m_oHeap;

	template <typename... T>
	void print(const T &... t);
	template <typename T>
	void PrintVector(std::vector<T> const &input);
	void Mark();
	void Sweep();

public:
	int bytesAllocated;
	int nextGC;
	VM* pVm;


	GC();
	~GC();
	void Dump(const char *label);
	void Collect();
	void AddObject(Traceable* o);
	void RemoveObject(Traceable* o);
	void AddRoot(Traceable* obj);
	void RemoveRoot(Traceable* obj);
	void ClearRoots();

	template <class T, class... Args>
	T* New(Args&&... args);

	template <class T>
	T* NewArray(size_t count);
};

/**
 * The `Traceable` struct is used as a base class
 * for any object which should be managed by GC.
 */
class Traceable
{
public:
	bool mMarked;

	Traceable();
	virtual ~Traceable();

	// Mark the object and all its children as live
  	void mark();

	// Overridden by derived classes to call mark()
	// on objects referenced by this object. The default
	// implemention does nothing.
	virtual void markChildren();
};

extern "C"
{
#define FOX_MODULE(name) void name##_entry()
#define FOX_MODULE_CALL(name) name##_entry()

#define Fox_RuntimeError(oVM, msg, ...) oVM->RuntimeError(msg, ##__VA_ARGS__)
#define Fox_PanicIfNot(oVM, cond, msg, ...) if (!(cond)) { Fox_RuntimeError(oVM, msg, ##__VA_ARGS__); return NIL_VAL; }

	static inline Value Fox_StringToValue(VM* oVM, const char* str)
	{
		return OBJ_VAL(oVM->m_oParser.TakeString(str));
	}

    static const char* Fox_ValueToCString(Value val)
    {
        return AS_CSTRING(val);
    }

    static double Fox_ValueToNumber(Value val)
    {
        return AS_NUMBER(val);
    }

    static inline ObjectAbstract* Fox_ValueToAbstract(Value value)
	{
		return AS_ABSTRACT(value);
	}

    static inline ObjectArray* Fox_ValueToArray(Value value)
	{
		return AS_ARRAY(value);
	}

	static inline bool Fox_ValueToBool(Value value)
	{
		return AS_BOOL(value);
	}


    static inline Value Fox_Abstract(VM* oVM, void* data, ObjectAbstractType* type)
	{
		return OBJ_VAL(oVM->gc.New<ObjectAbstract>(data, type));
	}

    static inline Value Fox_AbstractToValue(ObjectAbstract* abstract)
	{
		return OBJ_VAL(abstract);
	}

    static inline void* Fox_AbstractGetData(ObjectAbstract* abstract)
	{
		return abstract->data;
	}

    static inline Value Fox_DefineClass(VM* oVM, const char* klassName, NativeMethods methods)
	{
        oVM->DefineNativeClass(klassName, methods);
        return NIL_VAL;
	}

    static inline Value Fox_DefineInstanceOf(VM* oVM, const char* klassName)
	{
        Value klass;
        Value name = Fox_StringToValue(oVM, klassName);
        if (!oVM->globals.Get(AS_STRING(name), klass))
            return NIL_VAL;

		return OBJ_VAL(oVM->gc.New<ObjectNativeInstance>(AS_NATIVE_CLASS(klass)));
	}

	static inline Value Fox_DefineInstanceOfCStruct(VM* oVM, const char* klassName, void* cStruct)
	{
        Value klass;
        Value name = Fox_StringToValue(oVM, klassName);
        if (!oVM->globals.Get(AS_STRING(name), klass))
            return NIL_VAL;

		return OBJ_VAL(oVM->gc.New<ObjectNativeInstance>(AS_NATIVE_CLASS(klass), cStruct));
	}

    static inline void Fox_CallMethod(VM* oVM, Value instance, const char* methodName, int argCount, Value* params)
	{
        Value method;
        Value methodNameValue = Fox_StringToValue(oVM, methodName);
        if (AS_NATIVE_INSTANCE(instance)->klass->methods.Get(AS_STRING(methodNameValue), method))
        {
            AS_NATIVE(method)(oVM, argCount, params);
        }
	}

    static inline Value Fox_SetInstanceField(VM* oVM, Value instance, const char* fieldName, Value value)
	{
        Value name = Fox_StringToValue(oVM, fieldName);
        AS_NATIVE_INSTANCE(instance)->fields.Set(AS_STRING(name), value);

		return NIL_VAL;
	}

    static inline Value Fox_GetInstanceField(VM* oVM, Value instance, const char* fieldName)
	{
        Value value;
        Value name = Fox_StringToValue(oVM, fieldName);
        if (!AS_NATIVE_INSTANCE(instance)->fields.Get(AS_STRING(name), value))
        {
            return NIL_VAL;
        }
		return value;
	}

	static inline void* Fox_GetInstanceCStruct(Value instance)
	{
        return AS_NATIVE_INSTANCE(instance)->cStruct;
	}

	static inline void* Fox_SetInstanceCStruct(Value instance, void* data)
	{
		AS_NATIVE_INSTANCE(instance)->cStruct = data;
        return AS_NATIVE_INSTANCE(instance)->cStruct;
	}

    static inline void Fox_Arity(VM* oVM, int argCount, int min, int max)
	{
        if (argCount < min && argCount > max)
            Fox_RuntimeError(oVM, "Expected [%d-%d] arguments but got %d.", min, max, argCount);
	}

    static inline void Fox_FixArity(VM* oVM, int argCount, int min)
	{
        if (argCount != min)
            Fox_RuntimeError(oVM, "Expected %d arguments but got %d.", min, argCount);
	}

    static inline bool Fox_Is(Value value, ValueType type)
	{
        return value.type == type;
	}

    static inline bool Fox_IsObject(Value value)
	{
        return IS_OBJ(value);
	}

    static inline bool Fox_IsString(Value value)
	{
        return IS_STRING(value);
	}

    static inline bool Fox_IsNativeInstance(Value value)
	{
        return IS_NATIVE_INSTANCE(value);
	}

    static inline bool Fox_IsNumber(Value value)
	{
        return IS_NUMBER(value);
	}

	static inline bool Fox_IsBool(Value value)
	{
        return IS_BOOL(value);
	}

    static inline Value Fox_Array(VM* oVM)
	{
		return OBJ_VAL(oVM->gc.New<ObjectArray>());
	}
}