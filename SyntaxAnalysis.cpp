#include "SyntaxAnalysis.h"

bool SyntaxAnalysis::Do()
{
	// get the first token
	currentToken = getNextToken();
	// begin analysis from start nonterminal symbol
	q();

	// check if anything has been analyzed:
	if (--tokenIterator == lex.getTokenList().begin())
		return false;
	else
		return !syntaxErrorFound;
}


void SyntaxAnalysis::printSyntaxError(Token& token)
{
	cout << "Syntax error! Token: " << token.getValue() << " unexpected" << endl;
}


void SyntaxAnalysis::eat(TokenType t)
{
	if (currentToken.getType() == t)
	{
		currentToken.printTokenValue();
		if (t != T_END_OF_FILE)
			currentToken = getNextToken();
	}
	else
	{
		printSyntaxError(currentToken);
		syntaxErrorFound = true;
	}
}


Token SyntaxAnalysis::getNextToken()
{
	if (tokenIterator == lex.getTokenList().end())
		throw runtime_error("End of input file reached");
	return *tokenIterator++;
}

void SyntaxAnalysis::makeSuccessorsAndPredecessors(Instructions& instructions)
{
	for (int i = 0; i < instructions.size(); i++)
	{
		if (instructions[i]->getInstructionType() == InstructionType::I_B)
		{
			if (!existsLabel(instructions[i]->getJumpLabel()))
			{
				printSemanticsError(instructions[i]->getJumpLabel(), semanticErrors::NOT_DECLARED);
				throw runtime_error("");
			}

			instructions[i]->getSuccessors().push_back(labels[instructions[i]->getJumpLabel()]);
			instructions[i]->getSuccessors().front()->getPredecessor().push_back(instructions[i]);
		}
		else if (instructions[i]->getInstructionType() == InstructionType::I_BLTZ)
		{
			if (!existsLabel(instructions[i]->getJumpLabel()))
			{
				printSemanticsError(instructions[i]->getJumpLabel(), semanticErrors::NOT_DECLARED);
				throw runtime_error("");
			}

			instructions[i]->getSuccessors().push_back(labels[instructions[i]->getJumpLabel()]);

			if (i != instructions.size() - 1)
			{
				instructions[i]->getSuccessors().push_back(instructions[i + 1]);
			}

			for (auto instr : instructions[i]->getSuccessors())
			{
				instr->getPredecessor().push_back(instructions[i]);
			}
		}
		else
		{
			if (i != instructions.size() - 1)
			{
				instructions[i]->getSuccessors().push_back(instructions[i + 1]);
				instructions[i + 1]->getPredecessor().push_back(instructions[i]);
			}
		}
	}
}

void SyntaxAnalysis::q()
{
	if (syntaxErrorFound)
	{
		return;
	}

	s();
	eat(T_SEMI_COL);
	l();
}

void SyntaxAnalysis::s()
{
	if (syntaxErrorFound)
	{
		return;
	}

	if (currentToken.getType() == T_MEM)
	{
		eat(T_MEM);
		string varName = currentToken.getValue();

		if (existsMemVar(currentToken.getValue()))
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::ALREADY_DECLARED);
			throw runtime_error("");
		}

		eat(T_M_ID);
		int value;
		if (currentToken.getType() == T_NUM)
		{
			value = stoi(currentToken.getValue());
		}
		else
		{
			syntaxErrorFound = true;
			return;
		}
		 
		eat(T_NUM);
		MemoryVariable* memVar = new MemoryVariable(varName, value);
		allMemVars.push_back(memVar);
	}
	else if (currentToken.getType() == T_REG)
	{
		eat(T_REG);
		
		if (existsRegVar(currentToken.getValue()))
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::ALREADY_DECLARED);
			throw runtime_error("");
		}

		RegisterVariable* regVar = new RegisterVariable(currentToken.getValue());
		regVar->setPosition(position++);
		allRegVars.push_back(regVar);
		eat(T_R_ID);
	}
	else if (currentToken.getType() == T_FUNC)
	{
		eat(T_FUNC);
		
		if (existsFun(currentToken.getValue()))
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::ALREADY_DECLARED);
			throw runtime_error("");
		}

		lastFunctionDeclaration = currentToken.getValue();
		functions[lastFunctionDeclaration] = nullptr;
		instructionGiven = false;
		ld = lastDeclared::FUN;
		eat(T_ID);
	}
	else if (currentToken.getType() == T_ID)
	{
		if (existsLabel(currentToken.getValue()))
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::ALREADY_DECLARED);
			throw runtime_error("");
		}

		labels[currentToken.getValue()] = nullptr;
		lastLabel = currentToken.getValue();
		instructionGiven = false;
		ld = lastDeclared::LAB;
		eat(T_ID);
		eat(T_COL);
		e();
	}
	else
	{
		e();
	}
}

void SyntaxAnalysis::l()
{
	if (syntaxErrorFound)
	{
		return;
	}

	if (currentToken.getType() == T_END_OF_FILE)
	{
		eat(T_END_OF_FILE);
		makeSuccessorsAndPredecessors(instructions);
	}
	else
	{
		q();
	}
}

void SyntaxAnalysis::e()
{
	if (syntaxErrorFound)
	{
		return;
	}

	if (currentToken.getType() == T_ADD)
	{
		RegisterVariables src;
		RegisterVariables dest;

		eat(T_ADD);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));

		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);

		Instruction* instr = new Instruction(InstructionType::I_ADD, dest, src);
		instructions.push_back(instr);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_ADDI)
	{
		RegisterVariables src;
		RegisterVariables dest;

		eat(T_ADDI);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		if (currentToken.getType() != T_NUM)
		{
			syntaxErrorFound = true;
			return;
		}

		int num = stoi(currentToken.getValue());

		eat(T_NUM);

		Instruction* instr = new Instruction(InstructionType::I_ADDI, dest, src);
		instructions.push_back(instr);
		instr->getConstants().push_back(num);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_SUB)
	{
		RegisterVariables src;
		RegisterVariables dest;

		eat(T_SUB);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);

		Instruction* instr = new Instruction(InstructionType::I_SUB, dest, src);
		instructions.push_back(instr);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_LA)
	{
		RegisterVariables dest;
		RegisterVariables src;

		eat(T_LA);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		auto mem = find_if(allMemVars.begin(), allMemVars.end(), MemoryVariable::MemoryVariableComp(currentToken.getValue()));

		if (mem == allMemVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}

		eat(T_M_ID);

		Instruction* instr = new Instruction(InstructionType::I_LA, dest, src);
		instructions.push_back(instr);
		instr->getMemoryVariables().push_back(*mem);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_LW)
	{
		RegisterVariables dest;
		RegisterVariables src;

		eat(T_LW);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		if (currentToken.getType() != T_NUM)
		{
			syntaxErrorFound = true;
			return;
		}

		int num = stoi(currentToken.getValue());

		eat(T_NUM);
		eat(T_L_PARENT);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);
		eat(T_R_PARENT);

		Instruction* instr = new Instruction(InstructionType::I_LW, dest, src);
		instructions.push_back(instr);
		instr->getConstants().push_back(num);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_LI)
	{
		RegisterVariables dest;
		RegisterVariables src;

		eat(T_LI);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		if (currentToken.getType() != T_NUM)
		{
			syntaxErrorFound = true;
			return;
		}

		int num = stoi(currentToken.getValue());

		eat(T_NUM);

		Instruction* instr = new Instruction(InstructionType::I_LI, dest, src);
		instructions.push_back(instr);
		instr->getConstants().push_back(num);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_SW)
	{
		RegisterVariables dest;
		RegisterVariables src;

		eat(T_SW);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		if (currentToken.getType() != T_NUM)
		{
			syntaxErrorFound = true;
			return;
		}

		int num = stoi(currentToken.getValue());

		eat(T_NUM);
		eat(T_L_PARENT);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);
		eat(T_R_PARENT);

		Instruction* instr = new Instruction(InstructionType::I_SW, dest, src);
		instructions.push_back(instr);
		instr->getConstants().push_back(num);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_B)
	{
		RegisterVariables dest;
		RegisterVariables src;

		eat(T_B);

		string label = currentToken.getValue();
		eat(T_ID);

		Instruction* instr = new Instruction(InstructionType::I_B, dest, src);
		instructions.push_back(instr);
		instr->setJumpLabel(label);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_BLTZ)
	{
		RegisterVariables dest;
		RegisterVariables src;

		eat(T_BLTZ);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		string label = currentToken.getValue();
		eat(T_ID);

		Instruction* instr = new Instruction(InstructionType::I_BLTZ, dest, src);
		instructions.push_back(instr);
		instr->setJumpLabel(label);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_NOP)
	{
		RegisterVariables dest;
		RegisterVariables src;

		eat(T_NOP);

		Instruction* instr = new Instruction(InstructionType::I_NOP, dest, src);
		instructions.push_back(instr);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else if (currentToken.getType() == T_AND)
	{
		RegisterVariables src;
		RegisterVariables dest;

		eat(T_AND);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);

		Instruction* instr = new Instruction(InstructionType::I_AND, dest, src);
		instructions.push_back(instr);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}

	}
	else if (currentToken.getType() == T_OR)
	{
		RegisterVariables src;
		RegisterVariables dest;

		eat(T_OR);

		auto reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		dest.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);
		eat(T_COMMA);

		reg = find_if(allRegVars.begin(), allRegVars.end(), RegisterVariable::RegisterVariableComp(currentToken.getValue()));
		if (reg == allRegVars.end())
		{
			printSemanticsError(currentToken.getValue(), semanticErrors::NOT_DECLARED);
			throw runtime_error("");
		}
		src.push_back(*reg);

		eat(T_R_ID);

		Instruction* instr = new Instruction(InstructionType::I_OR, dest, src);
		instructions.push_back(instr);

		if (!instructionGiven)
		{
			if (ld == lastDeclared::LAB)
			{
				labels[lastLabel] = instr;
				instructionGiven = true;
			}
			else
			{
				functions[lastFunctionDeclaration] = instr;
				instructionGiven = true;
			}
		}
	}
	else
	{
		syntaxErrorFound = true;
	}
}