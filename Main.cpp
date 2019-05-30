#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"
#include "InterferenceGraph.h"
#include "Simplification.h"
#include "ResourceAllocation.h"

#include <iostream>
#include <exception>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Invalid arguments number!" << endl;
		return -1;
	}

	string fileName = argv[1];
	LexicalAnalysis lex;

	bool retVal = false;
	try
	{	
		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");

		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
		return -1;
	}

	SyntaxAnalysis sa(lex);

	try {
		retVal = sa.Do();

		if (retVal)
		{
			cout << "Syntax analysis finished successfully!" << endl;
		}
		else
		{
			throw runtime_error("\nException! Syntax analysis failed!\n");
		}
	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
		return -1;
	}

	livenessAnalysis(sa.getInstructions());
	cout << "Liveness analysis finished successfully!" << endl;

	InterferenceGraph ig = buildInterferenceGraph(sa.getInstructions(), sa.getRegisterVariables());
	cout << "Interference graph built successfully!" << endl;
	printInterferenceMatrix(ig, sa.getRegisterVariables());
	SimplificationStack ss = doSimplification(sa.getRegisterVariables(), ig, __REG_NUMBER__);
	
	bool allocation;
	int regNum = __REG_NUMBER__;

	while (true)
	{
		allocation = doResourceAllocation(ss, ig, sa.getRegisterVariables(), regNum);

		if (allocation)
		{
			cout << "Resource allocation finished successfully!" << endl;
			break;
		}
		else
		{
			cout << "Resource allocation failed! Trying with additional register..." << endl;
			regNum++;
		}
	}

	printInstructions(sa.getInstructions());

	string name = "";

	size_t pos = fileName.find_last_of('\\', fileName.length()) + 1;

	while (fileName[pos] != '.')
	{
		name += fileName[pos];
		pos++;
	}

	writeToFile(sa.getFunctions(), sa.getLabels(), sa.getMemoryVariables(), sa.getInstructions(), name);
	
	cout << "File " << name << ".s created!" << endl;
	cout << "Done!" << endl;

	return 0;
}