#include<urbandevelMarkov.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelMarkov, DynAlp)

urbandevelMarkov::urbandevelMarkov()
{
    this->dummy = 0;
}

urbandevelMarkov::~urbandevelMarkov()
{
}

void urbandevelMarkov::init()
{
    cityview = DM::View("CITY", DM::NODE, DM::MODIFY);

    cityview.addAttribute("dummy", DM::Attribute::DOUBLE, DM::READ);

    std::vector<DM::View> data;
    data.push_back(cityview);
    this->addData("data", data);
}

void urbandevelMarkov::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(cityview);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }
}

string urbandevelMarkov::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelBuilding.md";
}
