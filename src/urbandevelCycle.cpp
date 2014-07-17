#include<urbandevelCycle.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelCycle, DynAlp)

urbandevelCycle::urbandevelCycle()
{
    this->yearcycle = 0;
}

urbandevelCycle::~urbandevelCycle()
{
}

void urbandevelCycle::init()
{
    cityview = DM::View("CITY", DM::NODE, DM::MODIFY);

    cityview.addAttribute("yearcycle", DM::Attribute::DOUBLE, DM::READ);
    cityview.addAttribute("wp_com", DM::Attribute::DOUBLE, DM::READ); //workplaces
    cityview.addAttribute("wp_ind", DM::Attribute::DOUBLE, DM::READ);
    cityview.addAttribute("popdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::READ);
    cityview.addAttribute("cycle", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("currentyear", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("cycleBOOL", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> data;
    data.push_back(cityview);
    this->addData("data", data);
}

void urbandevelCycle::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(cityview);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcityview = cities[0];

    std::vector<double> popdiffperyear = currentcityview->getAttribute("popdiffperyear")->getDoubleVector();
    int startyear = static_cast<int>(currentcityview->getAttribute("startyear")->getDouble());
    int endyear = static_cast<int>(currentcityview->getAttribute("endyear")->getDouble());
    int currentyear = static_cast<int>(currentcityview->getAttribute("currentyear")->getDouble());
    bool cyclebool = static_cast<bool>(currentcityview->getAttribute("cycleBOOL")->getDouble());

    if ( currentyear <= startyear ) { currentyear = startyear + 1; }
    else if ( currentyear < endyear ){ currentyear++; }
    else if ( currentyear = endyear ){ cyclebool = 0; }

    int cycle = currentyear - startyear;
    int cyclepopdiff = popdiffperyear[cycle];

    DM::Logger(DM::Warning) << "year " << currentyear << "cycle " << cycle << "popdiff " << cyclepopdiff;

    DM::Attribute* set = currentcityview->getAttribute("currentyear");
    set->setDouble(currentyear);
    set = currentcityview->getAttribute("cycleBOOL");
    set->setDouble(cyclebool);
    set = currentcityview->getAttribute("cyclepopdiff");
    set->setDouble(cyclepopdiff);

    DM::Logger(DM::Warning) << "CYCLE - year: " << currentyear << " popdiff: " << cyclepopdiff;
}

string urbandevelCycle::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelBuilding.md";
}
