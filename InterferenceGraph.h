#pragma once

#include "IR.h"

/*
Description:
	Function used for building interference graph.

Parameters:
	Instructions& instructions: all instructions
	RegisterVariables& regVars: all register variables

Returns:
	Built interference graph.
*/

InterferenceGraph buildInterferenceGraph(Instructions& instructions, RegisterVariables& regVars);

/*
Description:
	Function used for printing interference matrix.

Parameters:
	InterferenceGraph& ig: interference graph
	RegisterVariables& regVars: all register variables
*/

void printInterferenceMatrix(InterferenceGraph& ig, RegisterVariables& regVars);