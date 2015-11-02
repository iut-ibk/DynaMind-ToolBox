#include "simulationtree.h"
#include "dmsimulation.h"
#include "dmmodule.h"
#include "dmlogger.h"


SimulationTree::SimulationTree(GUISimulation * sim): sim(sim)
{

}

void SimulationTree::update()
{
	std::list<DM::Link *> links = sim->getLinks();
	std::list<DM::Module *> modules = sim->getModules();

	// Find root nodes
	//    l1      l2
	// m1 --- > m2 ---- > m3

	// l1 [m1, m2]
	// l2 [m2, m3]

	// end_node_links[m2, l1]
	// end_node_links[m3, l2]

	//

	std::map<DM::Module*, DM::Link*> end_node_links;

	std::map<DM::Module*, DM::Link*> start_node_links;

	std::set<DM::Module*> end_nodes;

	foreach(DM::Link *l, links) {
		DM::Logger(DM::Standard) << l->src->getClassName() << " " << l->dest->getClassName();
		if(l->src->getOwner() == l->dest){ // Break Loop;
			DM::Logger(DM::Standard) << "remove "<< l->src->getClassName() << " " << l->dest->getClassName();
			continue;
		}
		start_node_links[l->src] = l;
		end_node_links[l->dest] = l;
	}

	foreach(DM::Module * m, modules)
		getPreviousLink(m, end_node_links, end_nodes);


	DM::Logger(DM::Standard) << "run";
	foreach (DM::Module * m, end_nodes) {
		DM::Logger(DM::Standard) << m->getName() << " " << m->getClassName();
		this->next(m, start_node_links);
	}
	DM::Logger(DM::Standard) << "done";


}



void SimulationTree::next(DM::Module * m, std::map<DM::Module*, DM::Link*>& start_node_links)
{
	DM::Logger(DM::Standard) << m->getClassName();
	if (start_node_links.find(m) == start_node_links.end()) { // start found
		DM::Logger(DM::Standard) << "END";
		return;
	}

	DM::Module * next_m = start_node_links[m]->dest;

	if (m->getOwner() == next_m) {
		DM::Logger(DM::Standard) << "Level Down";
		//DM::Module * next_m = nd_node_links[m]->src;
	}
	this->next(start_node_links[m]->dest, start_node_links);
}

void SimulationTree::getPreviousLink(DM::Module * m, std::map<DM::Module*, DM::Link*> & end_node_links, std::set<DM::Module*> & end_nodes)
{
	if (end_node_links.find(m) == end_node_links.end()) { // start found
		end_nodes.insert(m);
		return;
	}



	this->getPreviousLink(end_node_links[m]->src, end_node_links, end_nodes);

}



