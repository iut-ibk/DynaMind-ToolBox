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
    parcel = DM::View("PARCEL", DM::FACE, DM::MODIFY);

    city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cyclecomdiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("cycleinddiff", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("trigger", DM::Attribute::INT, DM::MODIFY);
    city.addAttribute("triggertype", DM::Attribute::STRING, DM::MODIFY);

    superblock.addAttribute("status", DM::Attribute::STRING, DM::READ);
    superblock.addAttribute("type", DM::Attribute::STRING, DM::READ);

    cityblock.addAttribute("status", DM::Attribute::STRING, DM::READ);
    cityblock.addAttribute("type", DM::Attribute::STRING, DM::READ);

    parcel.addAttribute("status", DM::Attribute::STRING, DM::READ);
    parcel.addAttribute("type", DM::Attribute::STRING, DM::READ);

    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(superblock);
    views.push_back(cityblock);
    views.push_back(parcel);

    this->addData("data", views);
}

void urbandevelDevelopment::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");
    std::vector<DM::Component *> sb = sys->getAllComponentsInView(superblock);
    std::vector<DM::Component *> cb = sys->getAllComponentsInView(cityblock);
    std::vector<DM::Component *> prcl = sys->getAllComponentsInView(parcel);

    std::vector<int> sbrankvec;

    std::string rankfieldname = "devrank";

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
    int cyclediff = 0;
    std::string triggertype = "";

    for (int j = 0; j < type.size() ; ++j)
    {
        if (type[j] == "res" ) cyclediff = cyclepopdiff;
        else if (type[j] == "com" ) cyclediff = cyclecomdiff;
        else if (type[j] == "ind" ) cyclediff = cycleinddiff;
        triggertype = type[j];
        if ( cyclediff > 0 ) break;
    }

    bool found_empty = 0;

    cities[0]->changeAttribute("trigger",0);
    if (cyclediff > 0)
    {
        cities[0]->changeAttribute("trigger",1);
        cities[0]->changeAttribute("triggerype",triggertype);

        //check empty parcels
        DM::Logger(DM::Warning) << "[DEVELOPMENT] type: " << triggertype << "pop: " << cyclediff;

        for (int i = 0; i < prcl.size(); i++)
        {
            std::string status = prcl[i]->getAttribute("status")->getString();
            std::string prcltype = prcl[i]->getAttribute("type")->getString();

            if (prcltype.compare(triggertype) != 0 || status.compare("populated") == 0 ) continue;

            if (status.compare("empty") == 0 ) {
                prcl[i]->changeAttribute("status", "process");
                return;
            }

        }

        //check empty cityblocks

        DM::Logger(DM::Debug) << "no empty parcels found, checking for empty cityblocks, type: " << triggertype;

        for (int i = 0; i < cb.size(); i++)
        {
            std::string status = cb[i]->getAttribute("status")->getString();
            std::string cbtype = cb[i]->getAttribute("type")->getString();

            //DM::Logger(DM::Warning) << "analyzing cityblock, status: " << status << "; type: " << cbtype;

            if (cbtype.compare(triggertype) != 0 || status.compare("populated") == 0 ) continue;

            if (status == "empty") {
                cb[i]->changeAttribute("status", "process");
                DM::Logger(DM::Warning) << "setting cityblock to develop, type: " << cbtype;
                return;
            }


        }

        //check empty superblocks

        DM::Logger(DM::Debug) << "no empty parcels or cityblocks found, checking for empty superblocks, type: " << triggertype;

        for (int i = 0; i < sb.size(); i++)
        {
            std::string status = sb[i]->getAttribute("status")->getString();\
            std::string sbtype = sb[i]->getAttribute("type")->getString();

            //DM::Logger(DM::Warning) << "analyzing superblock, status: " << status << "; type: " << sbtype;

            if (sbtype.compare(triggertype) != 0 || status.compare("populated") == 0 ) continue;

            if (status == "empty") {
                sb[i]->changeAttribute("status", "process");
                DM::Logger(DM::Warning) << "setting superblock to develop, type: " << sbtype;

                return;
            }
        }
    }
}
