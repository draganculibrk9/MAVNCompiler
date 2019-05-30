#include "InterferenceGraph.h"

InterferenceGraph buildInterferenceGraph(Instructions& instructions, RegisterVariables& regVars)
{
	InterferenceGraph ig;

	InterferenceMatrix interference_matrix;
	interference_matrix.resize(regVars.size());

	for (int i = 0; i < interference_matrix.size(); ++i)
	{
		interference_matrix.at(i).resize(interference_matrix.size());
	}

	//3. treba proci kroz instrukcije
	Instructions::iterator iter = instructions.begin();
	for (iter; iter != instructions.end(); ++iter)
	{
		//iteriramo po def
		RegisterVariables::iterator var_iter = (*iter)->getDef().begin();
		for (var_iter; var_iter != (*iter)->getDef().end(); ++var_iter)
		{
			//sad iteriramo kroz out
			RegisterVariables::iterator var_iter_ = (*iter)->getOut().begin();
			for (var_iter_; var_iter_ != (*iter)->getOut().end(); ++var_iter_)
			{
				//sad treba da proverimo da li ima smetnje izmedju varijabli
				if ((*var_iter)->getPosition() != (*var_iter_)->getPosition())
				{
					//dodajemo smetnju u matricu smetnji
					interference_matrix.at((*var_iter)->getPosition()).at((*var_iter_)->getPosition()) = __INTERFERENCE__;
					interference_matrix.at((*var_iter_)->getPosition()).at((*var_iter)->getPosition()) = __INTERFERENCE__;
				}
			}
		}
	}

	ig.matrix = interference_matrix;

	return ig;
}

void printInterferenceMatrix(InterferenceGraph & ig, RegisterVariables & regVars)
{
	if (ig.matrix.size() == 0)
	{
		cout << "There is nothing to print!" << endl;
		return;
	}

	cout << "==========================================" << endl;
	cout << "Interference matrix:" << endl;
	cout << "==========================================" << endl;

	// print existing variables in order to mark rows and columns
	for (auto varIt = regVars.begin(); varIt != regVars.end(); varIt++)
	{
		cout << "\t" << (*varIt)->getName();
	}
	cout << endl;

	auto varIt = regVars.begin();
	for (auto it1 = ig.matrix.begin(); it1 != ig.matrix.end(); it1++)
	{
		cout << (*varIt++)->getName();
		for (auto it2 = (*it1).begin(); it2 != (*it1).end(); it2++)
		{
			cout << "\t" << *it2;
		}
		cout << endl;
	}
	cout << "==========================================" << endl;
}
