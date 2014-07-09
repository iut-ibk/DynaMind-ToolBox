#include <numeric>

#include<urbandevelRankArea.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>


DM_DECLARE_NODE_NAME(urbandevelRankArea, DynAlp)

urbandevelRankArea::urbandevelRankArea()
{
    // declare parameters
    viewname = "SUPERBLOCK";
    reduction = 0;
    rank_function = "linear";
    rank_function_factor = 1;
    rank_weight = 1;

    this->addParameter("View", DM::STRING, &this->viewname);
    this->addParameter("reduction", DM::BOOL, &this->reduction);
    this->addParameter("ranking function", DM::STRING, &this->rank_function); // ranking function
    this->addParameter("ranking function faktor", DM::DOUBLE, &this->rank_function_factor);
    this->addParameter("rank_weight", DM::DOUBLE, &this->rank_weight);
}

urbandevelRankArea::~urbandevelRankArea()
{
}

void urbandevelRankArea::init()
{
    rankview = DM::View(viewname, DM::FACE, DM::MODIFY);
    city = DM::View("CITY", DM::NODE, DM::READ);

    if (reduction) {rankview.addAttribute("redrank", DM::Attribute::DOUBLE, DM::WRITE);}
    else {rankview.addAttribute("devrank", DM::Attribute::DOUBLE, DM::WRITE);}

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(rankview);
    this->addData("data", views);
}

void urbandevelRankArea::run()
{
    // get data from stream/port
    std::string rankfieldname = "devrank";
    if (reduction) rankfieldname = "redrank";

    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> areas = sys->getAllComponentsInView(rankview);
    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);

    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component* city = cities[0];

    std::vector<double> area;
    std::vector<int> rank;
    std::vector<int> oldrank;
    bool rnk_exists = FALSE;

    for (int i = 0; i < areas.size(); i++)
    {
        area.push_back(1/(TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)areas[i])));
        oldrank.push_back((areas[i]->getAttribute(rankfieldname)->getDouble()));
        if ( oldrank[i] > 0 ) { rnk_exists = TRUE; }
    }

    DAHelper::darank(area, rank, rank_function, rank_function_factor);
    if (rnk_exists) { DAHelper::daweight(oldrank, rank, rank_weight); }

    for (int i = 0; i < areas.size(); i++)
    {
        dynamic_cast<DM::Face*>(areas[i])->changeAttribute(rankfieldname, rank[i]);
    }
}
