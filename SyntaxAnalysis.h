#pragma once

#include "LexicalAnalysis.h"
#include "IR.h"
#include "Token.h"
#include <iostream>
#include <assert.h>
#include <algorithm>

class SyntaxAnalysis
{
private:

	/*
		Signalizes which type of label is last declared.	
	*/

	enum  lastDeclared
	{
		FUN = 0,
		LAB = 1
	};

	/*
		Lexical analyser.
	*/

	LexicalAnalysis& lex;
	
	/*
		Token list iterator.
	*/

	TokenList::iterator tokenIterator;

	/*
		Current token.
	*/

	Token currentToken;

	/*
		Functions of recursive descent syntax analyzer.
	*/

	void q();
	void s();
	void l();
	void e();

	/*
		Signalizes if syntax error is found.
	*/

	bool syntaxErrorFound = false;

	/*
		Map of labels.
	*/

	Labels labels;

	/*
		Map of functions.
	*/

	Functions functions;
	
	/*
		All register variables.
	*/

	RegisterVariables allRegVars;

	/*
		All memory variables.
	*/

	MemoryVariables allMemVars;

	/*
		Name of last declared function.
	*/

	string lastFunctionDeclaration;
	
	/*
		Name of last declared label.
	*/

	string lastLabel;
	
	/*
		Type of last declared label.
	*/

	lastDeclared ld;

	/*
		Signalizes if instruction is given to last declared label.
	*/

	bool instructionGiven = false;

	/*
		All instructions.
	*/

	Instructions instructions;

	/*
		Types of semantic errors.
	*/

	enum semanticErrors
	{
		NOT_DECLARED,
		ALREADY_DECLARED,
		NOT_EXPECTED
	};

public:

	/*
		Constructor
	*/

	SyntaxAnalysis(LexicalAnalysis& lex)
		:lex(lex)
	{
		tokenIterator = lex.getTokenList().begin();
	}

	/*
		Returns labels.
	*/

	Labels& getLabels() { return labels; }
	
	/*
		Returns functions.
	*/

	Functions& getFunctions() { return functions; }
	
	/*
		Returns register variables.
	*/
	
	RegisterVariables& getRegisterVariables() { return allRegVars; }
	
	/*
		Returns memory variables.
	*/
	
	MemoryVariables& getMemoryVariables() { return allMemVars; }
	
	/*
		Returns instructions.
	*/
	
	Instructions& getInstructions() { return instructions; }

	/*
		Function does syntax analysis and returns true if analysis is successful, false otherwise.
	*/

	bool Do();

	/*
		Function prints syntax error.
	*/

	void printSyntaxError(Token& token);

	/*
		Function eats token and checks its type.
	*/

	void eat(TokenType t);

	/*
		Function gets next token.
	*/

	Token getNextToken();

	/*
		Function makes successor and predecessor list for every instruction.
	*/

	void makeSuccessorsAndPredecessors(Instructions& instructions);

	/*
		Marks instruction position.
	*/

	int position = 0;

	/*
		Returns true if memory variable with given name exists.
	*/

	bool existsMemVar(string memVar)
	{
		auto it = find_if(allMemVars.begin(), allMemVars.end(), MemoryVariable::MemoryVariableComp(memVar));

		if (it != allMemVars.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*
		Returns true if register variable with given name exists.
	*/

	bool existsRegVar(string regVar)
	{
		auto it = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(regVar));

		if (it != allRegVars.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*
		Returns true if function with given name exists.
	*/

	bool existsFun(const string fun)
	{
		if (functions.find(fun) != functions.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*
		Returns true if label with given name exists.
	*/

	bool existsLabel(const string lab)
	{
		if (labels.find(lab) != labels.end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*
		Prints semantic error on given token.
	*/

	void printSemanticsError(string token, semanticErrors se)
	{
		cout << "Semantic error: ";
		switch (se)
		{
		case semanticErrors::NOT_DECLARED:
			cout << token << " not declared!" << endl;
			break;
		case semanticErrors::ALREADY_DECLARED:
			cout << token << " already declared!" << endl;
			break;
		case semanticErrors::NOT_EXPECTED:
			cout << token << " not expected!" << endl;
		}
	}
};