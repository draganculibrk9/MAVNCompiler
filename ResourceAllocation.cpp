#include "ResourceAllocation.h"

bool doResourceAllocation(SimplificationStack& ss, InterferenceGraph& ig, RegisterVariables& allVariables, int regNum)
{
	while (!ss.empty())
	{
		RegisterVariable* v = ss.top();
		ss.pop();
		
		if (v->getAssigment() != Regs::no_assign)
		{
			continue;
		}

		vector<int> neighbours = ig.matrix[v->getPosition()];

		for (int intReg = Regs::t0; intReg <= regNum; intReg++)
		{
			bool colorIsGood = true;

			for (RegisterVariable* neighbour : allVariables)
			{
				if (neighbours[neighbour->getPosition()] == __INTERFERENCE__ && neighbour->getAssigment() == intReg && neighbour->getAssigment() != Regs::no_assign)
				{
					colorIsGood = false;
					break;
				}
			}

			if (colorIsGood)
			{
				v->setAssigment(Regs(intReg));
				break;
			}
		}

		if (v->getAssigment() == Regs::no_assign)
		{
			ss.push(v);
			return false;
		}
	}

	return true;
}

void printInstructions(Instructions & instr)
{
	for (Instruction* instruction : instr)
	{
		cout << *instruction << endl;
	}
}

void writeToFile(Functions & f, Labels & l, MemoryVariables & memVars, Instructions & instr, string fileName)
{
	ofstream outputStream(fileName + ".s");

	for (auto function : f)
	{
		outputStream << ".globl " << function.first << endl;
	}

	outputStream << "\n.data" << endl;

	for (auto memVar : memVars)
	{
		outputStream << memVar->getName() << ":\t\t.word " << memVar->getValue() << endl;
	}

	outputStream << "\n.text" << endl;

	for (auto instruction : instr)
	{
		for (auto lab : l)
		{
			if (lab.second == instruction)
			{
				outputStream << lab.first << ":" << endl;
			}
		}

		for (auto fun : f)
		{
			if (fun.second == instruction)
			{
				outputStream << fun.first << ":" << endl;
			}
		}

		outputStream << "\t" << *instruction << endl;
	}
}
