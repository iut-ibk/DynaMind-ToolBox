#include <urbandevelRankArea.h>
#include <dm.h>
#include <tbvectordata.h>
#include <dmgeometry.h>
#include <dahelper.h>


DM_DECLARE_NODE_NAME(urbandevelRankArea, DynAlp)

urbandevelRankArea::urbandevelRankArea()
{
    // declare parameters
    rankview_name = "SUPERBLOCK";
    reduction = FALSE;
    rank_function = "linear";
    rank_function_factor = 1;
    rank_weight = 1;

    attribute_name = "devrank";

    this->addParameter("Rank view", DM::STRING, &this->rankview_name);
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
    rankview = DM::View(rankview_name, DM::FACE, DM::MODIFY);

    if (reduction) attribute_name = "redrank";
    rankview.addAttribute(attribute_name, DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> data;
    data.push_back(rankview);
    this->addData("data", data);
}

void urbandevelRankArea::run()
{
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> areas = sys->getAllComponentsInView(rankview);
    std::vector<double> area;
    std::vector<int> rank;
    std::vector<int> oldrank;
    bool rnk_exists = FALSE;

    for (int i = 0; i < areas.size(); i++)
    {
        area.push_back(1/(TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)areas[i])));
        oldrank.push_back((areas[i]->getAttribute(attribute_name)->getDouble()));
        if ( oldrank[i] > 0 ) { rnk_exists = TRUE; }
    }

    DAHelper::darank(area, rank, rank_function, rank_function_factor);
    if (rnk_exists) { DAHelper::daweight(oldrank, rank, rank_weight); }

    for (int i = 0; i < areas.size(); i++)
    {
        dynamic_cast<DM::Face*>(areas[i])->changeAttribute(attribute_name, rank[i]);
        DM::Logger(DM::Debug) << "area: " << area[i] << " oldrank: " << oldrank[i] << " rank: " << rank[i];

    }
}

std::string urbandevelRankArea::getHelpUrl()
{
    return "http://dynalp.com/documentation/urbandevel/RankArea.html";
}
