#pragma once
#include <string>
#include <iostream>

enum AstType
{
	AST_None,
	AST_Return,
	AST_Variable,
	AST_Print,
	AST_Assignment,
	AST_Add,
	AST_Sub,
	AST_Div,
	AST_Mul,
	AST_UnaryMinus,
	AST_Negate,
	AST_Equal,
	AST_NotEqual,
	AST_Greater,
	AST_GreaterEqual,
	AST_Less,
	AST_LessEqual,
	AST_For,
	AST_While,
	AST_If,
	AST_Else,
	AST_FuncCall,
	AST_FuncDeclaration,


	AST_Int,
	AST_String,
	AST_Float,
	AST_Double,
	AST_Struct,
};

class AstNode
{
public:
	AstNode* m_pNext;
	AstType type;
	AstNode()
	{

	}

	virtual void visit(std::ostream& stream, int nested)
	{

	}
};


inline bool isBlock(AstNode* node)
{
	switch (node->type)
	{
	case AST_While:
	case AST_For:
	case AST_If:
	case AST_Else:
	case AST_FuncDeclaration:
		return true;
	
	default:
		return false;
	}
}

class Statement : public AstNode
{
public:
	void visit(std::ostream& stream, int nested)
    {
    }
};

class AstExpression : public AstNode
{
public:
	void visit(std::ostream& stream, int nested)
    {
    }
};

class Type : public AstNode
{
public:
	std::string name;
	int m_pointerCount;

    Type(std::string typeName)
	{
		name = typeName;
		m_pointerCount = 0;
	}

	Type(std::string typeName, int pointerCount)
	{
		name = typeName;
		m_pointerCount = pointerCount;
	}

	void visit(std::ostream& stream, int nested)
    {
		stream << name;
    }
};

//
// Types
//

class IntType : public Type
{
public:
    IntType(std::string typeName) : Type(typeName)
	{
		type = AST_Int;
		name = typeName;
		m_pointerCount = 0;
	}

	IntType(std::string typeName, int pointerCount) : Type(typeName, pointerCount)
	{
		type = AST_Int;
		name = typeName;
		m_pointerCount = pointerCount;
	}

	void visit(std::ostream& stream, int nested)
    {
		stream << name;
    }
};

class StringType : public Type
{
public:
    StringType(std::string typeName) : Type(typeName)
	{
		type = AST_String;
		name = typeName;
		m_pointerCount = 0;
	}

	StringType(std::string typeName, int pointerCount) : Type(typeName, pointerCount)
	{
		type = AST_String;
		name = typeName;
		m_pointerCount = pointerCount;
	}

	void visit(std::ostream& stream, int nested)
    {
		stream << name;
    }
};
  
// object IntType : Type
// object DecimalType : Type
  
//
// AstExpressions
//

class BinaryAstExpression : public AstExpression
{
public:
	AstNode* left;
	AstNode* right;
    BinaryAstExpression(AstNode* l, AstNode* r)
    {
        left = l;
        right = r;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "Binary";
		left->visit(stream, nested);
		right->visit(stream, nested);
    }
};

class SumAstExpression : public BinaryAstExpression
{
public:
	SumAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "+";
		right->visit(stream, nested);
    }
};

class SubtractionAstExpression : public BinaryAstExpression
{
public:
	SubtractionAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "-";
		right->visit(stream, nested);
    }
};

class MultiplicationAstExpression : public BinaryAstExpression
{
public:
	MultiplicationAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "*";
		right->visit(stream, nested);
    }
};

class DivisionAstExpression : public BinaryAstExpression
{
public:
	DivisionAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "/";
		right->visit(stream, nested);
    }
};

class LessAstExpression : public BinaryAstExpression
{
public:
	LessAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "<";
		right->visit(stream, nested);
    }
};

class LessEqualAstExpression : public BinaryAstExpression
{
public:
	LessEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "<=";
		right->visit(stream, nested);
    }
};

class GreaterAstExpression : public BinaryAstExpression
{
public:
	GreaterAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << ">";
		right->visit(stream, nested);
    }
};

class GreaterEqualAstExpression : public BinaryAstExpression
{
public:
	GreaterEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << ">=";
		right->visit(stream, nested);
    }
};

class BangEqualAstExpression : public BinaryAstExpression
{
public:
	BangEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "!=";
		right->visit(stream, nested);
    }
};

class EqualEqualAstExpression : public BinaryAstExpression
{
public:
	EqualEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream, int nested)
    {
		left->visit(stream, nested);
		stream << "==";
		right->visit(stream, nested);
    }
};

class GroupingAstExpression : public AstNode
{
public:
	AstNode* value;
    GroupingAstExpression(AstNode* v)
    {
        value = v;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "(";
		value->visit(stream, nested);
		stream << ")";
    }
};

class AstDot : public AstNode
{
public:
	AstNode* expression;
	AstDot()
	{
		expression = NULL;
	}

	void visit(std::ostream& stream, int nested)
    {
		stream << "->";
		expression->visit(stream, nested);
    }
};

// UnaryMinusAstExpression :: class < AstExpression
// {
//     init(expresion)
//     {

//     }
// }

// TypeConversion :: class < AstExpression
// {
//     init(expresion, targetType)
//     {

//     }
// }

// VarReference :: class < AstExpression
// {
//     init(varName)
//     {

//     }
// }

// IntLit :: class < AstExpression
// {
//     init(value)
//     {

//     }
// }

// DecLit :: class < AstExpression
// {
//     init(value)
//     {

//     }
// }
  
// //
// // Statements
// //


class AstVar : public Statement
{
public:
	std::string name;
	std::list<AstNode*> children;
	bool isRef;
	bool isAssign;
	AstNode* expression;

    AstVar(std::string varName, bool ref = false, bool assign = false)
    {
		name = varName;
		isRef = ref;
		isAssign = assign;
    }

	void visit(std::ostream& stream, int nested)
    {
		if (isRef)
			stream << "&";
		stream << name;

		for(std::list<AstNode*>::iterator it = children.begin(); it != children.end(); it++)
			(*it)->visit(stream, nested);

		if (isAssign) {
			stream << " = ";
			expression->visit(stream, nested + 1);
		}
    }
};

class AstVarDeclaration : public Statement
{
public:
	std::string name;
	Type* type;
	bool assign;
	AstNode* expression;

    AstVarDeclaration(std::string varName, bool a = false, AstNode* e = NULL)
    {
		name = varName;
		assign = a;
		expression = e;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << type->name;
		for(int i = 0; i < type->m_pointerCount; i++)
		{
			stream << "*";
		}
		stream << " " << name;
		if (assign) {
			stream << " = ";
			expression->visit(stream, nested + 1);
		}
		if (nested == 0)
			stream << ";\n";
    }
};

class AstDeclaration : public Statement
{
public:
	std::string name;
	Type* type;

    AstDeclaration(std::string varName)
    {
		name = varName;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << type->name;
		for(int i = 0; i < type->m_pointerCount; i++)
		{
			stream << "*";
		}
		stream << " " << name;
    }
};

class AstFuncDeclaration : public Statement
{
public:
	std::string name;
    std::list<AstNode*> declarationList;
    std::list<AstNode*> body;
	Type* returnType;

	AstFuncDeclaration(std::string varName)
    {
		type = AST_FuncDeclaration;
		name = varName;
		returnType = NULL;
    }

    AstFuncDeclaration(std::string varName, Type* t)
    {
		type = AST_FuncDeclaration;
		name = varName;
		returnType = t;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << returnType->name << " " << name << "(";
		int i = declarationList.size() - 1;
		for(std::list<AstNode*>::iterator it = declarationList.begin(); it != declarationList.end(); it++)
		{
			(*it)->visit(stream, nested);
			if (i > 0)
				stream << ", ";
			i--;
		}
		stream << ")\n{\n";

		std::list<AstNode*>::iterator it;

		for (it = body.begin(); it != body.end() && (*it)->type != AST_Return; it++)
		{
			(*it)->visit(stream, nested + 1);
			if (!isBlock(*it))
				stream << ";\n";
		}
		if (name == "main")
			stream << "gc_stop(get_garbage());\n";

		if (it != body.end()) {
			(*it)->visit(stream, nested + 1);
			stream << ";\n";
		}

		stream << "}\n";
    }
};

class AstFuncCall : public Statement
{
public:
	std::string name;
    std::list<AstNode*> declarationList;

	AstFuncCall(std::string varName)
    {
		type = AST_FuncCall;
		name = varName;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << name << "(";
		int i = declarationList.size() - 1;
		for(std::list<AstNode*>::iterator it = declarationList.begin(); it != declarationList.end(); it++)
		{
			(*it)->visit(stream, nested);
			if (i > 0)
				stream << ", ";
			i--;
		}
		stream << ")";
		if (nested == 0)
			stream << ";\n";
    }
};

class AstStructDeclaration : public Statement
{
public:
	std::string name;
    std::list<AstNode*> declarationList;
	AstNode* body;

	AstStructDeclaration(std::string varName)
    {
		type = AST_Struct;
		name = varName;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "typedef struct " << name << " " << name << ";\n";
		stream << "struct " << name << "\n{\n";
		if (body)
			body->visit(stream, nested);
		stream << "};\n";
    }
};


// Assignment :: class < Statement
// {
//     init(varName, AstExpression)
//     {

//     }
// }

class Print : public Statement
{
public:
	std::list<AstNode*> args;
	Type* expression;
    Print(Type* e)
    {
		expression = e;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "printf(\"";
		expression->visit(stream, nested);
		stream << "\"";
		for(std::list<AstNode*>::iterator it = args.begin(); it != args.end(); it++)
		{
			stream << ", ";
			(*it)->visit(stream, nested + 1);
		}
		stream << ")";
    }
};

class Import : public Statement
{
public:
	Type* file;
    Import(Type* e)
    {
		file = e;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "#include \"";
		file->visit(stream, nested);
		stream << "\"\n";
    }
};

class Return : public Statement
{
public:
	AstNode* expression;
    Return(AstNode* e)
    {
		type = AST_Return;
		expression = e;
    }

	Return()
    {
		type = AST_Return;
		expression = NULL;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "return ";
		if (expression)
			expression->visit(stream, nested + 1);
    }
};

class AstIf : public Statement
{
public:
	AstNode* condition;
	AstNode* body;
	AstNode* elseBody;
    AstIf(AstNode* e)
    {
		type = AST_If;
		condition = e;
    }

	AstIf()
    {
		condition = NULL;
		body = NULL;
		elseBody = NULL;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "if (";
		condition->visit(stream, nested);
		stream << ")\n{\n";
		body->visit(stream, nested);
		stream << "}\n";
		if (elseBody) {
			stream << "else ";
			stream << "{\n";
			elseBody->visit(stream, nested);
			stream << "}\n";
		}
    }
};

class AstFor : public Statement
{
public:
	AstNode* initialize;
	AstNode* condition;
	AstNode* increment;
	std::list<AstNode*> body;

    AstFor()
    {
		type = AST_For;
		initialize = NULL;
		condition = NULL;
		increment = NULL;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "for (";
		if (initialize)
			initialize->visit(stream, nested + 1);
		stream << "; ";
		if (condition)
			condition->visit(stream, nested);
		stream << "; ";
		if (increment)
			increment->visit(stream, nested);
		stream << ")\n{\n";

		for (auto& it : body)
		{
			it->visit(stream, nested);
			if (!isBlock(it))
				stream << ";\n";
		}
		stream << "}\n";
    }
};

class AstWhile : public Statement
{
public:
	AstNode* condition;
	std::list<AstNode*> body;

    AstWhile()
    {
		type = AST_While;
		condition = NULL;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "while (";
		if (condition)
			condition->visit(stream, nested);
		stream << ")\n{\n";
		
		for (auto& it : body)
		{
			it->visit(stream, nested);
			if (!isBlock(it))
				stream << ";\n";
		}
		stream << "}\n";
    }
};

class AstNew : public Statement
{
public:
	std::string objectName;
	AstNode* args;

    AstNew()
    {
		args = NULL;
    }

	void visit(std::ostream& stream, int nested)
    {
		stream << "gc_malloc(get_garbage(), sizeof(" << objectName << "))";
		if (nested == 0)
			stream << ";\n";
    }
};