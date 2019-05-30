#include "LivenessAnalysis.h"

void livenessAnalysis(Instructions& instructions)
{
	bool doItAgain = true;
	while (doItAgain)
	{
		doItAgain = false;

		Instructions::reverse_iterator rit;
		for (rit = instructions.rbegin(); rit != instructions.rend(); rit++)
		{
			RegisterVariables& out = (*rit)->getOut();
			RegisterVariables& in = (*rit)->getIn();
			RegisterVariables newOut;
			RegisterVariables newIn;

			// 1.
			// newOut = U succIn;
			Instructions& successors = (*rit)->getSuccessors();
			Instructions::iterator succIt;
			for (succIt = successors.begin(); succIt != successors.end(); succIt++)
			{
				RegisterVariables& succIn = (*succIt)->getIn();
				newOut.insert(newOut.end(), succIn.begin(), succIn.end());
			}

			sort(newOut.begin(), newOut.end(), [](RegisterVariable* a, RegisterVariable* b) { return a->getName() < b->getName(); });
			unique(newOut.begin(), newOut.end(), [](RegisterVariable* a, RegisterVariable* b) { return a->getName() == b->getName(); });

			// 2.
			// newIn = use U (out / def)
			RegisterVariables& use = (*rit)->getUse();
			RegisterVariables& def = (*rit)->getDef();

			RegisterVariables outMinusDef;
			RegisterVariables::iterator varIt;
			for (varIt = out.begin(); varIt != out.end(); varIt++)
			{
				if (!variableExists(*varIt, def))
				{
					outMinusDef.push_back(*varIt);
				}
			}

			newIn = use;
			newIn.insert(newIn.end(), outMinusDef.begin(), outMinusDef.end());
			sort(newIn.begin(), newIn.end(), [](RegisterVariable* a, RegisterVariable* b) { return a->getName() < b->getName(); });
			unique(newIn.begin(), newIn.end(), [](RegisterVariable* a, RegisterVariable* b) { return a->getName() == b->getName(); });

			if (newIn != in || newOut != out)
			{
				doItAgain = true;
			}

			in = newIn;
			out = newOut;
		}
	}
}

bool variableExists(RegisterVariable* rv, RegisterVariables& variables)
{
	if (find(variables.begin(), variables.end(), rv) != variables.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}
