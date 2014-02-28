#include<urbandevelMarkov.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelMarkov, DynAlp)

urbandevelMarkov::urbandevelMarkov()
{
    this->yearcycle = 0;
}

urbandevelMarkov::~urbandevelMarkov()
{
}

void urbandevelMarkov::init()
{
    city = DM::View("CITY", DM::NODE, DM::MODIFY);

    city.addAttribute("yearcycle", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("wp_com", DM::Attribute::DOUBLE, DM::READ); //workplaces
    city.addAttribute("wp_ind", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("popdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::READ);
    city.addAttribute("cycle", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    this->addData("data", views);
}

void urbandevelMarkov::run()
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

}
