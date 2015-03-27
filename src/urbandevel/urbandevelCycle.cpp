#include <urbandevelCycle.h>
#include <dm.h>

DM_DECLARE_NODE_NAME(urbandevelCycle, DynAlp)

urbandevelCycle::urbandevelCycle()
{
}

urbandevelCycle::~urbandevelCycle()
{
}

void urbandevelCycle::init()
{
    cityview = DM::View("CITY", DM::NODE, DM::MODIFY);

    cityview.addAttribute("popdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::READ);
    cityview.addAttribute("comdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::READ);
    cityview.addAttribute("inddiffperyear", DM::Attribute::DOUBLEVECTOR, DM::READ);
    cityview.addAttribute("cycle", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("currentyear", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("cyclebool", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("cyclecomdiff", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("cycleinddiff", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("triggerbool", DM::Attribute::DOUBLE, DM::WRITE);

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
    std::vector<double> comdiffperyear = currentcityview->getAttribute("comdiffperyear")->getDoubleVector();
    std::vector<double> inddiffperyear = currentcityview->getAttribute("inddiffperyear")->getDoubleVector();
    int startyear = static_cast<int>(currentcityview->getAttribute("startyear")->getDouble());
    int endyear = static_cast<int>(currentcityview->getAttribute("endyear")->getDouble());
    int currentyear = static_cast<int>(currentcityview->getAttribute("currentyear")->getDouble());
    bool cyclebool = 1;

    if ( currentyear <= startyear ) currentyear = startyear;
    currentyear++;
    if ( currentyear == endyear ) cyclebool = 0;

    int cycle = currentyear - startyear -1;
    int cyclepopdiff = popdiffperyear[cycle];
    int cyclecomdiff = comdiffperyear[cycle];
    int cycleinddiff = inddiffperyear[cycle];

    DM::Attribute* set = currentcityview->getAttribute("currentyear");
    set->setDouble(currentyear);
    set = currentcityview->getAttribute("cyclebool");
    set->setDouble(cyclebool);
    set = currentcityview->getAttribute("cyclepopdiff");
    set->setDouble(cyclepopdiff);
    set = currentcityview->getAttribute("cyclecomdiff");
    set->setDouble(cyclecomdiff);
    set = currentcityview->getAttribute("cycleinddiff");
    set->setDouble(cycleinddiff);
    set = currentcityview->getAttribute("triggerbool");
    set->setDouble(1);

    DM::Logger(DM::Warning) << "CYCLE - year: " << currentyear << " popdiff: " << cyclepopdiff
                            << " comdiff: " << cyclecomdiff << " inddiff: " << cycleinddiff;
}

string urbandevelCycle::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/tree/master/doc/output/html/urbandevelCycle.html";
}
