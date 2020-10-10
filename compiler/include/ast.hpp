#pragma once
#include <string>
#include <iostream>

class AstNode
{
public:
	AstNode* m_pNext;
	AstNode()
	{

	}

	virtual void visit(std::ostream& stream)
	{

	}
};

class Statement : public AstNode
{
public:
	void visit(std::ostream& stream)
    {
    }
};

class AstExpression : public AstNode
{
public:
	void visit(std::ostream& stream)
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

	void visit(std::ostream& stream)
    {
		stream << name;
    }
};

//
// Types
//
  
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

	void visit(std::ostream& stream)
    {
		stream << "Binary";
		left->visit(stream);
		right->visit(stream);
    }
};

class SumAstExpression : public BinaryAstExpression
{
public:
	SumAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "+";
		right->visit(stream);
    }
};

class SubtractionAstExpression : public BinaryAstExpression
{
public:
	SubtractionAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "-";
		right->visit(stream);
    }
};

class MultiplicationAstExpression : public BinaryAstExpression
{
public:
	MultiplicationAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "*";
		right->visit(stream);
    }
};

class DivisionAstExpression : public BinaryAstExpression
{
public:
	DivisionAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "/";
		right->visit(stream);
    }
};

class LessAstExpression : public BinaryAstExpression
{
public:
	LessAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "<";
		right->visit(stream);
    }
};

class LessEqualAstExpression : public BinaryAstExpression
{
public:
	LessEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "<=";
		right->visit(stream);
    }
};

class GreaterAstExpression : public BinaryAstExpression
{
public:
	GreaterAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << ">";
		right->visit(stream);
    }
};

class GreaterEqualAstExpression : public BinaryAstExpression
{
public:
	GreaterEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << ">=";
		right->visit(stream);
    }
};

class BangEqualAstExpression : public BinaryAstExpression
{
public:
	BangEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "!=";
		right->visit(stream);
    }
};

class EqualEqualAstExpression : public BinaryAstExpression
{
public:
	EqualEqualAstExpression(AstNode* l, AstNode* r) : BinaryAstExpression(l, r) {}

	void visit(std::ostream& stream)
    {
		left->visit(stream);
		stream << "==";
		right->visit(stream);
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

	void visit(std::ostream& stream)
    {
		stream << "(";
		value->visit(stream);
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

	void visit(std::ostream& stream)
    {
		stream << ".";
		expression->visit(stream);
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

	void visit(std::ostream& stream)
    {
		if (isRef)
			stream << "&";
		stream << name;

		for(std::list<AstNode*>::iterator it = children.begin(); it != children.end(); it++)
			(*it)->visit(stream);

		if (isAssign) {
			stream << " = ";
			expression->visit(stream);
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

	void visit(std::ostream& stream)
    {
		stream << type->name;
		for(int i = 0; i < type->m_pointerCount; i++)
		{
			stream << "*";
		}
		stream << " " << name;
		if (assign) {
			stream << " = ";
			expression->visit(stream);
		}
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

	void visit(std::ostream& stream)
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
		name = varName;
		returnType = NULL;
    }

    AstFuncDeclaration(std::string varName, Type* t)
    {
		name = varName;
		returnType = t;
    }

	void visit(std::ostream& stream)
    {
		stream << returnType->name << " " << name << "(";
		int i = declarationList.size() - 1;
		for(std::list<AstNode*>::iterator it = declarationList.begin(); it != declarationList.end(); it++)
		{
			(*it)->visit(stream);
			if (i > 0)
				stream << ", ";
			i--;
		}
		stream << ")\n{\n";

		for(auto& node : body)
			node->visit(stream);
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
		name = varName;
    }

	void visit(std::ostream& stream)
    {
		stream << name << "(";
		int i = declarationList.size() - 1;
		for(std::list<AstNode*>::iterator it = declarationList.begin(); it != declarationList.end(); it++)
		{
			(*it)->visit(stream);
			if (i > 0)
				stream << ", ";
			i--;
		}
		stream << ");\n";
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
		name = varName;
    }

	void visit(std::ostream& stream)
    {
		stream << "typedef struct " << name << " " << name << ";\n";
		stream << "struct " << name << "\n{\n";
		if (body)
			body->visit(stream);
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

	void visit(std::ostream& stream)
    {
		stream << "printf(\"";
		expression->visit(stream);
		stream << "\"";
		for(std::list<AstNode*>::iterator it = args.begin(); it != args.end(); it++)
		{
			stream << ", ";
			(*it)->visit(stream);
		}
		stream << ");\n";
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

	void visit(std::ostream& stream)
    {
		stream << "#include \"";
		file->visit(stream);
		stream << "\"\n";
    }
};

class Return : public Statement
{
public:
	AstNode* expression;
    Return(AstNode* e)
    {
		expression = e;
    }

	Return()
    {
		expression = NULL;
    }

	void visit(std::ostream& stream)
    {
		stream << "return ";
		if (expression)
			expression->visit(stream);
		stream << ";\n";
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
		condition = e;
    }

	AstIf()
    {
		condition = NULL;
		body = NULL;
		elseBody = NULL;
    }

	void visit(std::ostream& stream)
    {
		stream << "if (";
		condition->visit(stream);
		stream << ")\n{\n";
		body->visit(stream);
		stream << "}\n";
		if (elseBody) {
			stream << "else ";
			stream << "{\n";
			elseBody->visit(stream);
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
	AstNode* body;

    AstFor()
    {
		initialize = NULL;
		condition = NULL;
		increment = NULL;
		body = NULL;
    }

	void visit(std::ostream& stream)
    {
		stream << "for (";
		if (initialize)
			initialize->visit(stream);
		stream << " ";
		if (condition)
			condition->visit(stream);
		stream << "; ";
		if (increment)
			increment->visit(stream);
		stream << ")\n{\n";
		body->visit(stream);
		stream << "}\n";
    }
};