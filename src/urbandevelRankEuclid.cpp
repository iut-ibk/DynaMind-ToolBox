#include <numeric>

#include<urbandevelRankEuclid.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>


DM_DECLARE_NODE_NAME(urbandevelRankEuclid, DynAlp)

urbandevelRankEuclid::urbandevelRankEuclid()
{
    // declare parameters
    viewname = "SUPERBLOCK";
    reduction = 0;
    rank_function = "linear";
    rank_function_factor = 1;
    rank_weight = 1;
    this->addParameter("view", DM::STRING, &this->viewname);
    this->addParameter("reduction", DM::BOOL, &this->reduction);
    this->addParameter("ranking function", DM::STRING, &this->rank_function); // ranking function
    this->addParameter("ranking function faktor", DM::DOUBLE, &this->rank_function_factor);
    this->addParameter("ranking weight", DM::DOUBLE, &this->rank_weight);
}

urbandevelRankEuclid::~urbandevelRankEuclid()
{
}

void urbandevelRankEuclid::init()
{
    // create a view - this one modifies an existing view 'myviewname'
    rankview = DM::View(viewname, DM::FACE, DM::MODIFY);
    rankview_centroids = DM::View(viewname+"_CENTROIDS", DM::NODE, DM::READ);
    city = DM::View("CITY", DM::NODE, DM::READ);

    if (reduction) {rankview.addAttribute("redrank", DM::Attribute::DOUBLE, DM::WRITE);}
    else {rankview.addAttribute("devrank", DM::Attribute::DOUBLE, DM::WRITE);}

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(rankview);
    this->addData("data", views);
}

void urbandevelRankEuclid::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> areas = sys->getAllComponentsInView(rankview);
    std::vector<DM::Component *> areas_centroids = sys->getAllComponentsInView(rankview_centroids);
    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);

    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component* city = cities[0];

    std::vector<double> distance;
    std::vector<int> rank;
    std::vector<int> oldrank;
    bool rnk_exists = FALSE;

    for (int i = 0; i < areas.size(); i++)
    {
        std::vector<DM::Component*> link = areas[i]->getAttribute(viewname+"_CENTROIDS")->getLinkedComponents();

        if(link.size() < 1)
        {
            DM::Logger(DM::Error) << "no area - centroid link";
            return;
        }

        DM::Node * centroid = dynamic_cast<DM::Node*>(link[0]);

        distance.push_back(TBVectorData::calculateDistance((DM::Node*)city, (DM::Node*)centroid));
        oldrank.push_back(areas[i]->getAttribute("devrank")->getDouble());
        if ( oldrank[i] > 0 ) { rnk_exists = TRUE; }
    }

    DAHelper::darank(distance, rank, rank_function, rank_function_factor);
    if (rnk_exists) { DAHelper::daweight(oldrank, rank, rank_weight); }

    for (int i = 0; i < areas.size(); i++)
    {
        dynamic_cast<DM::Face*>(areas[i])->changeAttribute("devrank", rank[i]);
    }
}
