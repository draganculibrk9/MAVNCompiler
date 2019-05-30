#pragma once

#include "IR.h"
#include <algorithm>

/*
Description:
	Function for liveness analysis.

Parameters:
	Instructions& instructions: vector of instructions
*/

void livenessAnalysis(Instructions& instructions);

/*
Description:
	Function checks if register variable exists.

Parameters:
	RegisterVariable* rv: register variable for check
	RegisterVariables& variables: all register variables
*/

bool variableExists(RegisterVariable* rv, RegisterVariables& variables);