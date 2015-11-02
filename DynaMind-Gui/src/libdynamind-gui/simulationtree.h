#ifndef SIMULATIONTREE_H
#define SIMULATIONTREE_H

#include "guisimulation.h"

class SimulationTree
{
public:
	SimulationTree(GUISimulation * sim);
	void update();


signals:

public slots:


private:
	GUISimulation * sim;
	void next(DM::Module *m, std::map<DM::Module*, DM::Link*>& start_node_links);

	void getPreviousLink(DM::Module * m, std::map<DM::Module *, DM::Link *> &end_node_links, std::set<DM::Module *> &end_nodes);

};

#endif // SIMULATIONTREE_H
