#include<urbandevelTrigger.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelTrigger, DynAlp)

urbandevelTrigger::urbandevelTrigger()
{
}

urbandevelTrigger::~urbandevelTrigger()
{
}

void urbandevelTrigger::init()
{
    city = DM::View("CITY", DM::NODE, DM::READ);
    superblock = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    cityblock = DM::View("CITYBLOCK", DM::FACE, DM::MODIFY);

    city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(superblock);
    views.push_back(cityblock);
    this->addData("data", views);
}

void urbandevelTrigger::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcity = cities[0];

    int cyclepopdiff  = static_cast<int>(currentcity->getAttribute("cyclepopdiff")->getDouble());

    //DM::Logger(DM::Warning) << "popdiff is " << cyclepopdiff;

    if (cyclepopdiff > 0)
    {
        //growth
        setdev();
    }
    else if (cyclepopdiff < 0)
    {
        setdec();
    }
}

void urbandevelTrigger::setdev()
{
    DM::System * sys = this->getData("data");
    std::vector<DM::Component *> sb = sys->getAllComponentsInView(superblock);

    //check free parcels

    DM::Logger(DM::Warning) << "in development mode";

    std::vector<double> rankvec;

    for (int i = 0; i < sb.size(); i++)
    {
        if (sb[i]->getAttribute("empty")->getDouble()) {
            DM::Logger(DM::Warning) << "empty";
            sb[i]->changeAttribute("selected", '1');
        }
        else {
            DM::Logger(DM::Warning) << "NOT empty";
        }

    }
    //check occ sb
    //check occ cb
    //check occ parcels
}


void urbandevelTrigger::setdec()
{
    //check occ sb
    //check occ cb
    //check occ parcels
    //remove population
    // ??? remove building ???
}
