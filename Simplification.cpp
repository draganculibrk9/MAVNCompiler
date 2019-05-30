#include "Simplification.h"

SimplificationStack doSimplification(RegisterVariables& allVars, InterferenceGraph& ig, int numberOfRegs)
{
	SimplificationStack ss;

	map<string, int> numberOfNeighbours;

	for (RegisterVariable* rv : allVars)
	{
		vector<int> neighbours = ig.matrix.at(rv->getPosition());
		int neighbourCount = count(neighbours.begin(), neighbours.end(), __INTERFERENCE__);
		numberOfNeighbours[rv->getName()] = neighbourCount;
	}

	vector<RegisterVariable*> unpushedVariables;

	for (RegisterVariable* rv : allVars)
	{
		if (numberOfNeighbours[rv->getName()] < numberOfRegs)
		{
			ss.push(rv);

			for (RegisterVariable* neighbour : allVars)
			{
				if (ig.matrix[rv->getPosition()][neighbour->getPosition()] == __INTERFERENCE__)
				{
					numberOfNeighbours[neighbour->getName()]--;
				}
			}
		}
		else
		{
			unpushedVariables.push_back(rv);
		}
	}

	for (RegisterVariable* rv : unpushedVariables)
	{
		ss.push(rv);
	}

	return ss;
}