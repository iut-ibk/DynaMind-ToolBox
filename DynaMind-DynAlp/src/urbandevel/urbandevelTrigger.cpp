#include <urbandevelTrigger.h>
#include <dm.h>

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

    city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cyclecomdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cycleinddiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("trigger", DM::Attribute::INT, DM::WRITE);
    city.addAttribute("triggertype", DM::Attribute::STRING, DM::WRITE);

    std::vector<DM::View> views;
    views.push_back(city);

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
    int cyclecomdiff  = static_cast<int>(currentcity->getAttribute("cyclecomdiff")->getDouble());
    int cycleinddiff  = static_cast<int>(currentcity->getAttribute("cycleinddiff")->getDouble());

    //DM::Logger(DM::Warning) << "[Trigger] popdiff = " << cyclepopdiff;

    cities[0]->changeAttribute("trigger",0);
    if (cyclepopdiff > 0 || cycleinddiff > 0 || cyclecomdiff > 0)
    {
        cities[0]->changeAttribute("trigger",1);
    }
    else cities[0]->changeAttribute("trigger",-1);
    cities[0]->changeAttribute("triggertype","");
}
