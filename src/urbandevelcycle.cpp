#include<urbandevelcycle.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(UrbanDevelCycle, DynAlp)

UrbanDevelCycle::UrbanDevelCycle()
{
    // declare parameters
    yearcycle = 5;
    wp_com = 10;
    wp_ind = 60;
    this->addParameter("Start year", DM::INT, &this->startyear); // if not set first year of data will be used
    this->addParameter("End year", DM::INT, &this->endyear); // if not set last year of data will be used
    this->addParameter("Years per Cycle", DM::INT, &this->yearcycle);
    this->addParameter("Share of commercial workplaces", DM::INT, &this->wp_com);
    this->addParameter("Share of industrial workplaces", DM::INT, &this->wp_ind);
}

UrbanDevelCycle::~UrbanDevelCycle()
{
}

void UrbanDevelCycle::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    city = DM::View("CITY", DM::COMPONENT, DM::MODIFY);
    // attach new attributes to view
    city.getAttribute("population");
    city.addAttribute("yearcycle");
    city.addAttribute("wp_com"); //workplaces
    city.addAttribute("wp_ind");

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    this->addData("data", views);
}

void UrbanDevelCycle::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::map<std::string,DM::Component *> cities = sys->getAllComponentsInView(city);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    mforeach(DM::Component* currentcity, cities)
    {
        //year->2000,2010,2020
        //pop->x,y,z
        QString year = QString::fromStdString(currentcity->getAttribute("year")->getString()).simplified();
        QString pop = QString::fromStdString(currentcity->getAttribute("population")->getString()).simplified();

        DM::Logger(DM::Debug) << "year: " << year << "  population: " << pop;

        QStringList yrlist = year.split(",");
        QStringList poplist = pop.split(",");

        if (yrlist.size() != poplist.size())
            DM::Logger(DM::Warning) << "no of years = " << yrlist.size() << "no of popdata = " << poplist.size() << "... must be the same";

        int sy = startyear;
        int ey = endyear;
        if (startyear == 0) int sy = yrlist.at(0).toInt();
        if (endyear == 0) int ey = yrlist.at(yrlist.size()).toInt();

        if (sy > ey)
            DM::Logger(DM::Warning) << "start year = " << sy << ">" << "end year = " << ey;

        std::map<int,int> popdifflist;

        for (int i=sy; i <= ey; i=i+yearcycle) // cycle through the years from startyear to endyear
        {
            for (int j=0; j < (yrlist.size()-1); j++) // cycle through the given population data
            {
                int yr1 = yrlist.at(j).toInt();
                int yr2 = yrlist.at(j+1).toInt();

                if (i > yr1 && i < yr2)
                {
                    int yrdiff = yrlist.at(j+1).toInt() - yrlist.at(j).toInt();
                    int popdiff = poplist.at(j+1).toInt() - poplist.at(j).toInt();
                    int cyclediff = popdiff/yrdiff*yearcycle;
                    DM::Logger(DM::Debug) << "year: " << i << "cyclediff: " << cyclediff;

                    popdifflist[i]=cyclediff;
                }
            }

        }


    }

    // be sure to destruct any objects allocated with malloc or new!
}
