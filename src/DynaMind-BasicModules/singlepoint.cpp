#include "singlepoint.h"

DM_DECLARE_NODE_NAME(SinglePoint, Modules)
SinglePoint::SinglePoint()
{
    x = 0;
    y = 0;
    z = 0;
    viewname = "";
    appendToExisting = false;

    this->addParameter("x", DM::DOUBLE, &x);
    this->addParameter("y", DM::DOUBLE, &y);
    this->addParameter("z", DM::DOUBLE, &z);

    this->addParameter("ViewName", DM::STRING, &viewname);
    this->addParameter("AppendToStream", DM::BOOL, &appendToExisting);
}

void SinglePoint::init() {
    if (viewname.empty())
        return;
    std::vector<DM::View> datastream;

    datastream.push_back(DM::View(viewname, DM::NODE, DM::WRITE));

    if (appendToExisting)
        datastream.push_back(DM::View("dummy", DM::SUBSYSTEM, DM::READ));

    this->addData("sys", datastream);

}

void SinglePoint::run() {
    DM::System * sys = this->getData("sys");
    sys->addNode(x,y,z, DM::View(viewname, DM::READ, DM::WRITE));

}


