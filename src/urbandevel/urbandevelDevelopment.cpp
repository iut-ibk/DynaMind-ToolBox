#include <urbandevelDevelopment.h>
#include <dm.h>

DM_DECLARE_NODE_NAME(urbandevelDevelopment, DynAlp)

urbandevelDevelopment::urbandevelDevelopment()
{
}

urbandevelDevelopment::~urbandevelDevelopment()
{
}

void urbandevelDevelopment::init()
{
    city = DM::View("CITY", DM::NODE, DM::READ);
    superblock = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    cityblock = DM::View("CITYBLOCK", DM::FACE, DM::MODIFY);
    //parcel = DM::View("PARCEL", DM::FACE, DM::MODIFY);

    city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cyclecomdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cycleinddiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("trigger", DM::Attribute::INT, DM::MODIFY);

    superblock.addAttribute("status", DM::Attribute::STRING, DM::READ);
    superblock.addAttribute("type", DM::Attribute::STRING, DM::READ);

    cityblock.addAttribute("status", DM::Attribute::STRING, DM::READ);
    cityblock.addAttribute("type", DM::Attribute::STRING, DM::READ);

    //parcel.addAttribute("status", DM::Attribute::STRING, DM::READ);
    //parcel.addAttribute("type", DM::Attribute::STRING, DM::READ);

    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(superblock);
    views.push_back(cityblock);
    //views.push_back(parcel);

    this->addData("data", views);
}

void urbandevelDevelopment::run()
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

    std::vector<std::string> type;
    type.push_back("res");
    type.push_back("com");
    type.push_back("ind");

    int cyclepopdiff  = static_cast<int>(currentcity->getAttribute("cyclepopdiff")->getDouble());
    int cyclecomdiff  = static_cast<int>(currentcity->getAttribute("cyclecomdiff")->getDouble());
    int cycleinddiff  = static_cast<int>(currentcity->getAttribute("cycleinddiff")->getDouble());

    cities[0]->changeAttribute("trigger",0);
    if (cyclepopdiff > 0)
    {
        cyclepopdiff-=150;
        cities[0]->changeAttribute("cyclepopdiff",cyclepopdiff);

        cities[0]->changeAttribute("trigger",1);
        DM::Logger(DM::Warning) << "developing, population = " << cyclepopdiff;
        for (int i = 0; i < type.size() ; ++i)
        {

        }
    }
}
