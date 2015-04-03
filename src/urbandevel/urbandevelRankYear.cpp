#include <urbandevelRankYear.h>
#include <dm.h>
#include <tbvectordata.h>
#include <dmgeometry.h>
#include <dahelper.h>

DM_DECLARE_NODE_NAME(urbandevelRankYear, DynAlp)

urbandevelRankYear::urbandevelRankYear()
{
    // declare parameters
    viewname = "SUPERBLOCK";
    reduction = 0;
    rank_function = "linear";
    rank_function_factor = 1;
    rank_weight = 1;
    this->addParameter("view", DM::STRING, &this->viewname);
    this->addParameter("rank reduction", DM::BOOL, &this->reduction);
    this->addParameter("ranking function", DM::STRING, &this->rank_function); // ranking function
    this->addParameter("ranking function faktor", DM::DOUBLE, &this->rank_function_factor);
    this->addParameter("ranking weight", DM::DOUBLE, &this->rank_weight);

    yearfieldname = "devyear";
    rankfieldname = "devrank";
}

urbandevelRankYear::~urbandevelRankYear()
{
}

void urbandevelRankYear::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    rankview = DM::View(viewname, DM::FACE, DM::MODIFY);
    city = DM::View("CITY", DM::NODE, DM::READ);

    city.addAttribute("startyear", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("endyear", DM::Attribute::DOUBLE, DM::READ);
    city.addAttribute("currentyear", DM::Attribute::DOUBLE, DM::READ);

    if (reduction) {
        yearfieldname = "redyear";
        rankfieldname = "redrank";
    }

    rankview.addAttribute(yearfieldname, DM::Attribute::DOUBLE, DM::READ);
    rankview.addAttribute("type", DM::Attribute::DOUBLE, DM::READ);
    rankview.addAttribute(rankfieldname, DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(rankview);
    this->addData("data", views);
}

void urbandevelRankYear::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> areas = sys->getAllComponentsInView(rankview);
    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);

    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component* currentcity = cities[0];

    int startyear = static_cast<int>(currentcity->getAttribute("startyear")->getDouble());
    int endyear = static_cast<int>(currentcity->getAttribute("endyear")->getDouble());
    int actualyear = static_cast<int>(currentcity->getAttribute("currentyear")->getDouble());

    std::vector<int> oldrank;
    std::vector<int> rank;
    std::vector<int> year;
    std::vector<double> rankval;
    bool rnk_exists = false;

    for (int i = 0; i < areas.size(); i++)
    {
        int areayear = static_cast<int>(areas[i]->getAttribute(yearfieldname)->getDouble());
        if ( areayear < startyear && areayear > startyear-10 ) areayear = startyear + 1;
        else if ( areayear <= startyear-10 ) areayear = endyear;

        int diffyear = abs(areayear - actualyear);
        if (diffyear == 0) diffyear = 1;

        double rankvalue;
        if (!reduction) rankvalue = 1/diffyear;
        else rankvalue = diffyear;

        year.push_back(diffyear);
        rankval.push_back(rankvalue);

        oldrank.push_back(static_cast<int>(areas[i]->getAttribute(rankfieldname)->getDouble()));
        if ( oldrank[i] > 0 ) rnk_exists = true;
    }

    DAHelper::darank(rankval, rank, rank_function, rank_function_factor);
    if (rnk_exists) DAHelper::daweight(oldrank, rank, rank_weight);

    for (int i = 0; i < areas.size(); i++)
    {
        dynamic_cast<DM::Face*>(areas[i])->changeAttribute(rankfieldname, rank[i]);
        DM::Logger(DM::Debug) << "year: " << year[i] << " rank: " << rank[i] << " oldrank: " << oldrank[i];
    }
}
