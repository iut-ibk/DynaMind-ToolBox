#include <numeric>

#include<urbandevelRankYear.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>


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
    this->addParameter("ranking funcktion faktor", DM::DOUBLE, &this->rank_function_factor);
    this->addParameter("ranking weight", DM::DOUBLE, &this->rank_weight);
}

urbandevelRankYear::~urbandevelRankYear()
{
}

void urbandevelRankYear::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    rankview = DM::View(viewname, DM::FACE, DM::MODIFY);
    city = DM::View("CITY", DM::NODE, DM::READ);

    // attach new attributes to view
    rankview.addAttribute("develyear", DM::Attribute::DOUBLE, DM::READ);
    rankview.addAttribute("type", DM::Attribute::DOUBLE, DM::READ);
    rankview.addAttribute("develrank", DM::Attribute::DOUBLE, DM::WRITE);

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

    std::vector<double> year;
    std::vector<int> oldrank;
    std::vector<int> rank;
    bool rnk_exists = FALSE;

    for (int i = 0; i < areas.size(); i++)
    {
        int currentyear = static_cast<int>(areas[i]->getAttribute("develyear")->getDouble());
        if ( currentyear <= startyear ) { currentyear = startyear + 1; }
        if ( currentyear >= endyear ) { currentyear = endyear - 1; }
        year.push_back(currentyear);
        oldrank.push_back(static_cast<int>(areas[i]->getAttribute("devrank")->getDouble()));
        if ( oldrank[i] > 0 ) { rnk_exists = TRUE; }
    }
    DAHelper::darank(year, rank, rank_function, rank_function_factor);
    if (rnk_exists) { DAHelper::daweight(oldrank, rank, rank_weight); }

    for (int i = 0; i < areas.size(); i++)
    {
        dynamic_cast<DM::Face*>(areas[i])->changeAttribute("devrank", rank[i]);
    }
}
