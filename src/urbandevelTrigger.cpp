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
    sb = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    cb = DM::View("CITYBLOCK", DM::FACE, DM::MODIFY);
    bd = DM::View("BUILDING", DM::FACE, DM::MODIFY);

    city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(sb);
    views.push_back(cb);
    views.push_back(bd);
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

    if (cyclepopdiff > 0)
    {
        //growth
        bool ret = setdev();
    }
    else if (cyclepopdiff < 0)
    {
        //decline
    }
}

bool urbandevelTrigger::setdev()
{
    //check free parcels



    //check free cb
    //check free sb
    //foreach areas
}

bool urbandevelTrigger::setdec()
{
    //check occ sb
    //check occ cb
    //check occ parcels
    //remove population
    // ??? remove building ???
}
