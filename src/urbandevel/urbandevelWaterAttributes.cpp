#include <urbandevelWaterAttributes.h>
#include <dm.h>

DM_DECLARE_NODE_NAME(urbandevelWaterAttributes, DynAlp)

urbandevelWaterAttributes::urbandevelWaterAttributes()
{
    paramfromCity = TRUE;

    dwfpp = 4.1;
    wsdpp = 200;

    this->addParameter("Parameters from City", DM::BOOL, &paramfromCity);

    this->addParameter("DWF per person", DM::DOUBLE, &dwfpp);
    this->addParameter("WSD per person", DM::DOUBLE, &wsdpp);
}

void urbandevelWaterAttributes::init()
{
    city = DM::View("CITY", DM::COMPONENT, DM::READ);
    buildingview = DM::View("BUILDING", DM::FACE, DM::MODIFY);

    buildingview.addAttribute("dwf", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("wsd", DM::Attribute::DOUBLE, DM::WRITE);

    if (paramfromCity)
    {
        city.addAttribute("dwfpp", DM::Attribute::DOUBLE, DM::READ);
        city.addAttribute("wsdpp", DM::Attribute::DOUBLE, DM::READ);
    }
    std::vector<DM::View> data;

    data.push_back(city);
    data.push_back(buildingview);
    this->addData("data", data);
}

void urbandevelWaterAttributes::run()
{
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);
    std::vector<DM::Component *> buildings = sys->getAllComponentsInView(buildingview);

    if (cities.size() != 1 && paramfromCity)
    {
        DM::Logger(DM::Warning) << "One component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcity = cities[0];

    if (paramfromCity)
    {
        dwfpp = currentcity->getAttribute("dwfpp")->getDouble();
        wsdpp = currentcity->getAttribute("wsdpp")->getDouble();
    }

    for (int i = 0; i < buildings.size(); i++)
    {
        DM::Face * currentbuilding = dynamic_cast<DM::Face *>(buildings[i]);
        int pop = static_cast<int>(currentbuilding->getAttribute("POP")->getDouble());

        if (pop < 1) continue;

        double dwf = pop*dwfpp;
        double wsd = pop*wsdpp;

        currentbuilding->addAttribute("dwf", dwf);
        currentbuilding->addAttribute("wsd", wsd);
    }
}

string urbandevelWaterAttributes::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelWaterAttributes.md";
}
