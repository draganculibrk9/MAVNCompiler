#pragma once

#include "IR.h"

/*
Description:
	Function makes simplification stack.

Parameters:
	RegisterVariables& allVars: all register variables
	InterferenceGraph& ig: interference graph
	int registerNumber: number of available registers

Returns:
	Simplification stack.
*/

SimplificationStack doSimplification(RegisterVariables& allVars, InterferenceGraph& ig, int registerNumber);