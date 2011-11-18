#include "inoutmodule.h"
#include <DMcomponent.h>
#include <DMsystem.h>
#include <DMnode.h>


VIBe_DECLARE_NODE_NAME( InOut,Modules )
InOut::InOut() {

    Logger(Debug) << "Create InOut";

    this->addParameter("Inport", VIBe2::SYSTEM_IN, &this->sys_in);



    //this->addParameter("Inport", VIBe2::VECTORDATA_IN, & this->vec_in);
    //this->addParameter("Outport", VIBe2::VECTORDATA_OUT, & this->vec_out);

}



void InOut::run() {
    Logger(Debug) << "Run InOut";
    std::map<std::string, DM::Node*> all_nodes = sys_in->getAllNodes();

    for (std::map<std::string, DM::Node*>::const_iterator it = all_nodes.begin(); it != all_nodes.end(); ++it) {
        DM::Node * n = it->second;
       Logger(Debug) << n->getName() << n->getX() << n->getY() << n->getZ();
    }



    /*std::vector<std::string> names = vec_in->getPointsNames();

    foreach (std::string name, names) {
        Logger(Debug) << name;
        std::vector<DM::Node> points = this->vec_in->getPoints(name);
        foreach (DM::Node p, points) {
            //Logger(Debug) << p;
        }

    };

    names = vec_in->getEdgeNames();

    foreach (std::string name, names) {
        Logger(Debug) << name;
        std::vector<DM::Node> points = this->vec_in->getPoints(name);
        std::vector<Edge> edges = this->vec_in->getEdges(name);
        foreach (Edge e, edges) {
            Logger(Debug) << e.getID1() << "/" << e.getID2();
            //Logger(Debug) << points[e.getID1()] << "-" <<  points[e.getID2()];
        }



    };*/

}

InOut::~InOut() {
    Logger(Debug) << "Destructor InOut";
}
