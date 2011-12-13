#include "comparenetworks.h"
#include <DMcomponent.h>
#include <DMsystem.h>
#include <DMattribute.h>
#include <DMnode.h>


VIBe_DECLARE_NODE_NAME( CompareNetworks,Modules )
CompareNetworks::CompareNetworks() {
    std::vector<DM::View> views;
    std::vector<DM::View> views2;

    DM::View network1 = DM::View("Shape");
    network1.getAttributes("Shapelist");
    network1.getAttributes("Type");

    DM::View network2 = DM::View("Shape");
    network2.getAttributes("Shapelist");
    network2.getAttributes("Type");

    views.push_back(network1);
    views2.push_back(network2);

    this->addData("Network1", views);
    this->addData("Network2", views2);

    n1i="";
    n2i="";
    this->addParameter("Network1-Type",VIBe2::STRING, &n1i);
    this->addParameter("Network2-Type",VIBe2::STRING, &n2i);
}



void CompareNetworks::run()
{
    n1 = this->getData("Network1");
    n2 = this->getData("Network2");
    Logger(Standard) << "Network1-Type: " << n1i;
    Logger(Standard) << "Network2-Type: " << n2i;
    Logger(Standard) << "Found type in Network1: " << n1->getAttribute("Type")->getString();
    Logger(Standard) << "Found type in Network2: " << n2->getAttribute("Type")->getString();


}

CompareNetworks::~CompareNetworks()
{
}
