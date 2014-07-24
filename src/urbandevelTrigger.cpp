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
    parcel = DM::View("PARCEL", DM::FACE, DM::MODIFY);

    city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cyclecomdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cycleinddiff", DM::Attribute::DOUBLE, DM::READ);

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

    std::vector<std::string> type;
    type.push_back("res");
    type.push_back("com");
    type.push_back("ind");

    int cyclepopdiff  = static_cast<int>(currentcity->getAttribute("cyclepopdiff")->getDouble());
    int cyclecomdiff  = static_cast<int>(currentcity->getAttribute("cyclecomdiff")->getDouble());
    int cycleinddiff  = static_cast<int>(currentcity->getAttribute("cycleinddiff")->getDouble());

    bool dev = 1;

    for (int i = 0; i < type.size() ; ++i)
    {
        if (cyclepopdiff < 0) dev = 0;
        setdev(type[i],dev);
    }
}

void urbandevelTrigger::setdev(std::string type, bool dev)
{
    DM::System * sys = this->getData("data");
    std::vector<DM::Component *> sb = sys->getAllComponentsInView(superblock);
    std::vector<DM::Component *> cb = sys->getAllComponentsInView(cityblock);
    std::vector<DM::Component *> prcl = sys->getAllComponentsInView(parcel);

    //check free parcels

    DM::Logger(DM::Debug) << "in development mode";

    std::vector<double> rankvec;

    // develop all available parcels, no matter which ranking (fill up existing superblocks)

    for (int i = 0; i < prcl.size(); i++)
    {
        std::string status = prcl[i]->getAttribute("status")->getString();

        if (status == "empty") {
            DM::Logger(DM::Warning) << "empty";
            prcl[i]->changeAttribute("status", "develop");
            return;
        }
    }

    // develop all abvailable cityblocks, no matter which ranking (fill up existing superblocks))

    for (int i = 0; i < cb.size(); i++)
    {
        std::string status = cb[i]->getAttribute("status")->getString();

        if (status == "empty") {
            DM::Logger(DM::Warning) << "empty";
            cb[i]->changeAttribute("status", "develop");
            return;
        }
    }

    for (int i = 0; i < sb.size(); i++)
    {
        std::string status = sb[i]->getAttribute("status")->getString();

        if (status == "empty") {
            DM::Logger(DM::Warning) << "empty";
            sb[i]->changeAttribute("status", "develop");
            return;
        }
    }
    return;
}
