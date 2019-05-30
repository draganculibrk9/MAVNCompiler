#pragma once

#include "IR.h"
#include "Types.h"
#include <fstream>

/*
Description:
	Function does resource allocation.

Parameters:
	SimplificationStack& ss: simplification stack
	InterferenceGraph& ig: interference graph
	RegisterVariables& allVariables: all register variables
	int regNum: number of available registers

Returns:
	True if allocations is successful, false otherwise.
*/

bool doResourceAllocation(SimplificationStack& ss, InterferenceGraph& ig, RegisterVariables& allVariables, int regNum);

/*
Description:
	Prints intructions.

Parameters:
	Instructions& instr: vector of instructions
*/

void printInstructions(Instructions& instr);

/*
Description:
	Function writes assembly code to file.

Parameters:
	Functions& f: all functions
	Labels& l: all labels
	MemoryVariables& memVars: all memory variables
	Instructions& instr: all instructions
	string fileName: name of output file
*/

void writeToFile(Functions& f, Labels& l, MemoryVariables& memVars, Instructions& instr, string fileName);