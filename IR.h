#pragma once

#include "Types.h"

/*
Description:
	Class representing register variable.

Attributes:
	string rv_name: register variable name
	int rv_position: register variable position in interference matrix
	Regs rv_assignment: real register assigned to register variable

Methods:
	RegisterVariable(string name): constructor
	string getName() const: returns register variable name
	int getPosition(): returns register variable position in interference matrix
	void setPosition(int position): sets register variable position
	Regs getAssigment(): returns register variable assigment
	void setAssigment(Regs reg): sets register variable assigment to reg
*/

class RegisterVariable
{
private:
	string rv_name;
	int rv_position;
	Regs rv_assignment;
public:
	RegisterVariable(string name)
		:rv_name(name),
		rv_position(-1),
		rv_assignment(no_assign)
	{}

	string getName() const { return rv_name; }
	int getPosition() { return rv_position; }
	void setPosition(int position) { rv_position = position; }
	Regs getAssigment() { return rv_assignment; }
	void setAssigment(Regs reg) { rv_assignment = reg; }

	/*
	Description:
		Nested struct used as predicate in register variable comparison.

	Attributes:
		string p_name: name used for comparison

	Methods:
		explicit RegisterVariableComp(string name): constructor
		inline bool operator()(const RegisterVariable* rv) const: overriden () operator used for comparison
	*/

	struct RegisterVariableComp
	{
		explicit RegisterVariableComp(string name) : p_name(name) { }
		inline bool operator()(const RegisterVariable* rv) const { return rv->rv_name == p_name; }
	private:
		string p_name;
	};
};

/*
Description:
	Class representing memory variable.

Attributes:
	string mv_name: memory variable name
	int mv_value: memory variable value

Methods:
	MemoryVariable(string name, int value): constructor
	string getName() const: return memory variable name
	int getValue(): returns memory variable value
*/

class MemoryVariable
{
private:
	string mv_name;
	int mv_value;
public:
	MemoryVariable(string name, int value)
		:mv_name(name),
		mv_value(value)
	{}

	string getName() const { return mv_name; }
	int getValue() { return mv_value; }

	/*
	Description:
		Nested struct used as predicate in memory variable comparison.

	Attributes:
		string p_name: name used for comparison

	Methods:
		explicit MemoryVariableComp(string name): constructor
		inline bool operator()(const MemoryVariable* mv) const: overriden () operator used for comparison
	*/

	struct MemoryVariableComp
	{
		explicit MemoryVariableComp(string name) : p_name(name) { }
		inline bool operator()(const MemoryVariable* mv) const { return mv->mv_name == p_name; }
	private:
		string p_name;
	};
};

/*
	Vector containing pointers to register variables.
*/

typedef vector<RegisterVariable*> RegisterVariables;

/*
	Vector containing pointers to memory variables.
*/

typedef vector<MemoryVariable*> MemoryVariables;

/*
Description:
	Class representing instruction.

Attributes:
	InstructionType m_type: instruction type
	RegisterVariables m_dst: destination register variables
	RegisterVariables m_src: source register variables
	RegisterVariables m_use: use set
	RegisterVariables m_def: def set
	RegisterVariables m_in: in set
	RegisterVariables m_out: out set
	vector<Instruction*> m_succ: vector containing successor instructions
	vector<Instruction*> m_pred: vector containing predecessor instructions
	vector<int> constants: constants used in instruction
	MemoryVariables memVars: memory variables used in instruction
	string jumpLabel: name of label on which function jumps

Methods:
	Instruction(InstructionType type, RegisterVariables dst, RegisterVariables src): constructor
	InstructionType getInstructionType(): returns instruction type
	RegisterVariables& getDestination(): returns destination register variables
	RegisterVariables& getSource(): returns source register variables
	RegisterVariables& getUse(): return use set
	void setUse(RegisterVariables newUse): sets use set
	RegisterVariables& getDef(): returns def set
	void setDef(RegisterVariables newDef): sets def set
	RegisterVariables& getIn(): returns in set
	void setIn(RegisterVariables newIn): sets in set
	RegisterVariables& getOut(): returns out set
	void setOut(RegisterVariables newOut): sets out set
	vector<Instruction*>& getSuccessors(): returns vector of successor instructions
	vector<Instruction*>& getPredecessor(): returns vector of predecessor instructions
	MemoryVariables& getMemoryVariables(): returns memory variables
	vector<int>& getConstants(): returns vector of constants
	string getJumpLabel(): returns name of jump label
	void setJumpLabel(string label): sets name of jump label

Functions:
	friend ostream& operator <<(ostream& o, Instruction& i): overriden output operator
*/

class Instruction
{
public:
	Instruction(InstructionType type, RegisterVariables dst, RegisterVariables src)
		:m_type(type),
		m_dst(dst),
		m_src(src),
		m_use(src),
		m_def(dst)
	{}

	InstructionType getInstructionType() { return m_type; }

	RegisterVariables& getDestination() { return m_dst; }
	RegisterVariables& getSource() { return m_src; }

	RegisterVariables& getUse() { return m_use; }
	void setUse(RegisterVariables newUse) { m_use = newUse; }

	RegisterVariables& getDef() { return m_def; }
	void setDef(RegisterVariables newDef) { m_use = newDef; }

	RegisterVariables& getIn() { return m_in; }
	void setIn(RegisterVariables newIn) { m_use = newIn; }

	RegisterVariables& getOut() { return m_out; }
	void setOut(RegisterVariables newOut) { m_use = newOut; }

	vector<Instruction*>& getSuccessors() { return m_succ; }
	vector<Instruction*>& getPredecessor() { return m_pred; }

	MemoryVariables& getMemoryVariables() { return memVars; }
	vector<int>& getConstants() { return constants; }

	string getJumpLabel() { return jumpLabel; }
	void setJumpLabel(string label) { jumpLabel = label; }

	friend ostream& operator <<(ostream& o, Instruction& i)
	{
		switch (i.getInstructionType())
		{
		case InstructionType::I_ADD:
			o << "add\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", $t" << i.getSource()[0]->getAssigment() - 1 << ", $t" << i.getSource()[1]->getAssigment() - 1;
			break;
		case InstructionType::I_ADDI:
			o << "addi\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", $t" << i.getSource()[0]->getAssigment() - 1 << ", " << i.getConstants()[0];
			break;
		case InstructionType::I_SUB:
			o << "sub\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", $t" << i.getSource()[0]->getAssigment() - 1 << ", $t" << i.getSource()[1]->getAssigment() - 1;
			break;
		case InstructionType::I_LA:
			o << "la\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", " << i.getMemoryVariables()[0]->getName();
			break;
		case InstructionType::I_LW:
			o << "lw\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", " << i.getConstants()[0] << "($t" << i.getSource()[0]->getAssigment() - 1 << ")";
			break;
		case InstructionType::I_LI:
			o << "li\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", " << i.getConstants()[0];
			break;
		case InstructionType::I_SW:
			o << "sw\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", " << i.getConstants()[0] << "($t" << i.getSource()[0]->getAssigment() - 1 << ")";
			break;
		case InstructionType::I_B:
			o << "b\t\t " << i.getJumpLabel();
			break;
		case InstructionType::I_BLTZ:
			o << "bltz\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", " << i.getJumpLabel();
			break;
		case InstructionType::I_NOP:
			o << "nop\t\t";
			break;
		case InstructionType::I_AND:
			o << "and\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", $t" << i.getSource()[0]->getAssigment() - 1 << ", $t" << i.getSource()[1]->getAssigment() - 1;
			break;
		case InstructionType::I_OR:
			o << "or\t\t $t" << i.getDestination()[0]->getAssigment() - 1 << ", $t" << i.getSource()[0]->getAssigment() - 1 << ", $t" << i.getSource()[1]->getAssigment() - 1;
			break;
		}

		return o;
	}
private:
	InstructionType m_type;

	RegisterVariables m_dst;
	RegisterVariables m_src;

	RegisterVariables m_use;
	RegisterVariables m_def;
	RegisterVariables m_in;
	RegisterVariables m_out;

	vector<Instruction*> m_succ;
	vector<Instruction*> m_pred;

	vector<int> constants;

	MemoryVariables memVars;

	string jumpLabel;
};

/*
	Vector of pointers to instructions.
*/

typedef vector<Instruction*> Instructions;

/*
	Map storing label names as keys and referenced instructions as values.
*/

typedef map<string, Instruction*> Labels;

/*
	Map storing function names as keys and referenced instructions as values.
*/

typedef map<string, Instruction*> Functions;

/*
	Vector of vectors containing int representing interference matrix.
*/

typedef std::vector<std::vector<int>> InterferenceMatrix;

/*
Description:
	Interference graph struct.

Attributes:
	InterferenceMatrix matrix: interference matrix

Methods:
	InterferenceGraphStruct(): constructor
*/

typedef struct InterferenceGraphStruct
{
	InterferenceMatrix matrix;
	InterferenceGraphStruct(){}

} InterferenceGraph;

/*
	Register variable stack.
*/

typedef std::stack<RegisterVariable*> SimplificationStack;
