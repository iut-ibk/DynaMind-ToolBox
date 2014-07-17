#include<urbandevelControl.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelControl, DynAlp)

urbandevelControl::urbandevelControl()
{
    // declare parameters
    startyear = 2000;
    endyear = 2030;
    yearcycle = 5;
    wp_com = 60;
    wp_ind = 10;

    this->addParameter("Start year", DM::INT, &this->startyear); // if not set first year of data will be used
    this->addParameter("End year", DM::INT, &this->endyear); // if not set last year of data will be used
    this->addParameter("Years per cycle", DM::INT, &this->yearcycle);
    this->addParameter("Share of commercial workplaces", DM::INT, &this->wp_com);
    this->addParameter("Share of industrial workplaces", DM::INT, &this->wp_ind);
}

urbandevelControl::~urbandevelControl()
{
}

void urbandevelControl::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    cityview = DM::View("CITY", DM::NODE, DM::MODIFY);
    // attach new attributes to view
    cityview.addAttribute("year", DM::Attribute::DOUBLE, DM::READ);
    cityview.addAttribute("population", DM::Attribute::DOUBLE, DM::READ);
    cityview.addAttribute("wp_com", DM::Attribute::DOUBLE, DM::WRITE); //workplaces
    cityview.addAttribute("wp_ind", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("startyear", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("endyear", DM::Attribute::DOUBLE, DM::WRITE);
    cityview.addAttribute("yearcycle", DM::Attribute::DOUBLE, DM::WRITE);

    cityview.addAttribute("popdiffperyear", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
    cityview.addAttribute("cycleBOOL", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> data;
    data.push_back(cityview);
    this->addData("data", data);
}

void urbandevelControl::run()
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
        //year->2000,2010,2020
        //pop->10000,15000,12000
    QString year = QString::fromStdString(currentcityview->getAttribute("year")->getString()).simplified();
    QString pop = QString::fromStdString(currentcityview->getAttribute("population")->getString()).simplified();

        DM::Logger(DM::Warning) << "year: " << year << "\npopulation: " << pop;

        QStringList yrlist = year.split(",");
        QStringList poplist = pop.split(",");

        if (yrlist.size() != poplist.size())
        {
            DM::Logger(DM::Error) << "no of years = " << yrlist.size() << "no of popdata = " << poplist.size() << "... must be the same";
            return;
        }

        int sy = startyear;
        int ey = endyear;

        if (startyear == 0) int sy = yrlist.at(0).toInt();
        if (endyear == 0) int ey = yrlist.at(yrlist.size()).toInt();

        if (sy > ey)
            DM::Logger(DM::Warning) << "start year = " << sy << ">" << "end year = " << ey;

        std::vector<double> popdiffvector;

        for (int i=sy; i <= ey; i++) // cycle through the years from startyear to endyear
        {
            for (int j=0; j < yrlist.size()-1; j++) // cycle through the given population data
            {
                int yr1 = yrlist.at(j).toInt();
                int yr2 = yrlist.at(j+1).toInt();

                if (i > yr1 && i < yr2)
                {
                    int yrdiff = yrlist.at(j+1).toInt() - yrlist.at(j).toInt();
                    int popdiff = poplist.at(j+1).toInt() - poplist.at(j).toInt();
                    popdiffvector.push_back(popdiff/yrdiff);
                    //DM::Logger(DM::Warning) << "year: " << i << "popdiffperyear: " << popdiff/yrdiff;
                }
            }
        }
        DM::Attribute* dmatt = currentcityview->getAttribute("popdiffperyear");
        dmatt->setDoubleVector(popdiffvector);

        dmatt = currentcityview->getAttribute("startyear");
        dmatt->setDouble(startyear);

        dmatt = currentcityview->getAttribute("endyear");
        dmatt->setDouble(endyear);

        dmatt = currentcityview->getAttribute("yearcycle");
        dmatt->setDouble(yearcycle);

        dmatt = currentcityview->getAttribute("wp_com");
        dmatt->setDouble(wp_com);

        dmatt = currentcityview->getAttribute("wp_ind");
        dmatt->setDouble(wp_ind);

        dmatt = currentcityview->getAttribute("cycleBOOL");
        dmatt->setDouble(1);
}

string urbandevelControl::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelBuilding.md";
}
