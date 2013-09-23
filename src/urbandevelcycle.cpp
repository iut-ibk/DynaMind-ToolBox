#include<urbandevelcycle.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(UrbanDevelCycle, DynAlp)

UrbanDevelCycle::UrbanDevelCycle()
{
    // declare parameters
    yearcycle = 5;
    wp_com = 10;
    wp_ind = 60;
    this->addParameter("Years to Cycle", DM::INT, &this->yearcycle);
    this->addParameter("Share of commercial workplaces (incl. desk jobs & scholars)", DM::INT, &this->wp_com);
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

        DM::Logger(DM::Warning) << "year: " << year << "  population: " << pop;
        QStringList yrlist = year.split(",");
        QStringList poplist = pop.split(",");

        QStringList popcycle;

        for (int i = 0; i < yrlist.size(); i++)
        {
            int yrdiff = yrlist.at(i+1).toInt() - yrlist.at(i).toInt();
            int popdiff = poplist.at(i+1).toInt() - poplist.at(i).toInt();
            popcycle.append(QString::number(popdiff/yrdiff));
            DM::Logger(DM::Warning) << "popcycle: " << popdiff/yrdiff;
        }
    }

    // be sure to destruct any objects allocated with malloc or new!
}
