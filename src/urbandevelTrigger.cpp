#include<urbandevelTrigger.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelTrigger, DynAlp)

urbandevelTrigger::urbandevelTrigger()
{
    this->yearcycle = 0;
}

urbandevelTrigger::~urbandevelTrigger()
{
}

void urbandevelTrigger::init()
{
    city = DM::View("CITY", DM::NODE, DM::MODIFY);

    city.addAttribute("yearcycle", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("wp_com", DM::Attribute::DOUBLE, DM::READ); //workplaces
    city.addAttribute("wp_ind", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("popdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::READ);
    city.addAttribute("cycle", DM::Attribute::DOUBLE, DM::WRITE);
    city.addAttribute("currentyear", DM::Attribute::DOUBLE, DM::WRITE);
    city.addAttribute("cycleBOOL", DM::Attribute::DOUBLE, DM::WRITE);
    city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
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

    std::vector<double> popdiffperyear = currentcity->getAttribute("popdiffperyear")->getDoubleVector();
    int startyear = static_cast<int>(currentcity->getAttribute("startyear")->getDouble());
    int endyear = static_cast<int>(currentcity->getAttribute("endyear")->getDouble());
    int currentyear = static_cast<int>(currentcity->getAttribute("currentyear")->getDouble());
    bool cyclebool = static_cast<bool>(currentcity->getAttribute("cycleBOOL")->getDouble());

    if ( currentyear <= startyear ) { currentyear = startyear + 1; }
    else if ( currentyear < endyear ){ currentyear++; }
    else if ( currentyear = endyear ){ cyclebool = 0; }

    int cycle = currentyear - startyear;
    int cyclepopdiff = popdiffperyear[cycle];

    DM::Logger(DM::Warning) << "year " << currentyear << "cycle " << cycle << "popdiff " << cyclepopdiff;

    DM::Attribute* set = currentcity->getAttribute("currentyear");
    set->setDouble(currentyear);
    set = currentcity->getAttribute("cycleBOOL");
    set->setDouble(cyclebool);
    set = currentcity->getAttribute("cyclepopdiff");
    set->setDouble(cyclepopdiff);
}
