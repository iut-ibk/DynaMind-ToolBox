#include <urbandevelControl.h>
#include <dm.h>

DM_DECLARE_NODE_NAME(urbandevelControl, DynAlp)

urbandevelControl::urbandevelControl()
{
    startyear = 2000;
    endyear = 2030;
    wpfromcity = FALSE;
    workplaces = 60;
    shareindwp = 10;

    this->addParameter("Start year", DM::INT, &this->startyear); // if not set first year of data will be used
    this->addParameter("End year", DM::INT, &this->endyear); // if not set last year of data will be used
    this->addParameter("Fetch workplace information from CITY", DM::BOOL, &this->wpfromcity);
    this->addParameter("Workplaces per 100 inh.", DM::INT, &this->workplaces);
    this->addParameter("Share industrial workplaces", DM::INT, &this->shareindwp);
}

urbandevelControl::~urbandevelControl()
{

}

void urbandevelControl::init()
{
    cityview = DM::View("CITY", DM::NODE, DM::MODIFY);
    cityview.addAttribute("year", DM::Attribute::STRING, DM::READ);
    cityview.addAttribute("population", DM::Attribute::STRING, DM::READ);
    cityview.addAttribute("startyear", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("endyear", DM::Attribute::DOUBLE, DM::WRITE);

    cityview.addAttribute("popdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
    cityview.addAttribute("comdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
    cityview.addAttribute("inddiffperyear", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
    cityview.addAttribute("cyclebool", DM::Attribute::DOUBLE, DM::WRITE);

    if (wpfromcity)
    {
        cityview.addAttribute("workplaces", DM::Attribute::STRING, DM::READ);
        cityview.addAttribute("shareindwpwp", DM::Attribute::STRING, DM::READ);
    }

    std::vector<DM::View> data;
    data.push_back(cityview);
    this->addData("data", data);
}

void urbandevelControl::run()
{
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(cityview);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcity = cities[0];

    // Read CSV Strings from CITY.year and CITY.population; must be the same length

    QStringList yearlist = QString::fromStdString(currentcity->getAttribute("year")->getString()).simplified().split(",");
    QStringList poplist = QString::fromStdString(currentcity->getAttribute("population")->getString()).simplified().split(",");

    QStringList wplist;
    QStringList indlist;

    if (wpfromcity)
    {
        wplist = QString::fromStdString(currentcity->getAttribute("workplaces")->getString()).simplified().split(",");
        indlist = QString::fromStdString(currentcity->getAttribute("shareindwp")->getString()).simplified().split(",");
    }

    if (yearlist.size() != poplist.size())
    {
        DM::Logger(DM::Error) << "no of years = " << yearlist.size() << "no of popdata = " << poplist.size() << "... must be the same";
        return;
    }

    if (wpfromcity)
    {
        if (yearlist.size() != wplist.size())
        {
            DM::Logger(DM::Error) << "no of years = " << yearlist.size() << "no of workplaces = " << wplist.size() << "... must be the same";
        }
        else if (yearlist.size() != indlist.size())
        {
            DM::Logger(DM::Error) << "no of years = " << yearlist.size() << "no of share of industrial worplaces = " << indlist.size() << "... must be the same";
        }
    }

    int sy = startyear;
    int ey = endyear;

    if (startyear == 0) int sy = yearlist.at(0).toInt();
    if (endyear == 0) int ey = yearlist.at(yearlist.size()).toInt();

    if (sy > ey) DM::Logger(DM::Warning) << "start year = " << sy << ">" << "end year = " << ey;

    std::vector<double> popdiffvector;
    std::vector<double> comdiffvector;
    std::vector<double> inddiffvector;

    for (int i=sy; i <= ey; i++) // cycle through the years from startyear to endyear
    {
        for (int j=0; j < yearlist.size()-1; j++) // second cycle through all given population data (vector)
        {
            int yr1 = yearlist.at(j).toInt(); // get actual year
            int yr2 = yearlist.at(j+1).toInt(); // get next year

            //DM::Logger(DM::Warning) << "actual year = " << yr1 << ">" << "next year = " << yr2;

            if (i > yr1 && i <= yr2)
            {
                int yrdiff = yearlist.at(j+1).toInt() - yearlist.at(j).toInt();
                int popdiff = poplist.at(j+1).toInt() - poplist.at(j).toInt();
                int popdiff_per_year = popdiff/yrdiff;

                int inddiff_per_year = popdiff_per_year * workplaces * shareindwp / 10000;
                int comdiff_per_year = (popdiff_per_year * workplaces / 100) - inddiff_per_year;

                if (wpfromcity)
                {
                    int totwpdiff = popdiff_per_year * wplist.at(j).toInt() / 100;
                    inddiff_per_year = popdiff_per_year*indlist.at(j).toInt() / 100;
                    comdiff_per_year = (popdiff_per_year * totwpdiff / 100 ) - inddiff_per_year;
                }

                popdiffvector.push_back(popdiff_per_year);
                comdiffvector.push_back(comdiff_per_year);
                inddiffvector.push_back(inddiff_per_year);

                DM::Logger(DM::Warning) << "year: " << i << " pop: " << popdiff_per_year << " com: " << comdiff_per_year << " ind: " << inddiff_per_year;
            }
        }
    }


    DM::Attribute* dmatt = currentcity->getAttribute("popdiffperyear");
    dmatt->setDoubleVector(popdiffvector);

    dmatt = currentcity->getAttribute("startyear");
    dmatt->setDouble(startyear);

    dmatt = currentcity->getAttribute("endyear");
    dmatt->setDouble(endyear);

    dmatt = currentcity->getAttribute("comdiffperyear");
    dmatt->setDoubleVector(comdiffvector);

    dmatt = currentcity->getAttribute("inddiffperyear");
    dmatt->setDoubleVector(inddiffvector);

    dmatt = currentcity->getAttribute("cyclebool");
    dmatt->setDouble(1);
}

string urbandevelControl::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/tree/master/doc/output/html/urbandevelControl.html";
}
