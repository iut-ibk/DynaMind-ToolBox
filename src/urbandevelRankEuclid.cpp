#include <numeric>

#include<urbandevelRankEuclid.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>


DM_DECLARE_NODE_NAME(urbandevelRankEuclid, DynAlp)

urbandevelRankEuclid::urbandevelRankEuclid()
{
    gravityview_name = "GRAVITYPOINT";
    rankview_name = "SUPERBLOCK";
    reduction = FALSE;
    rank_function = "linear";
    rank_function_factor = 1;
    rank_weight = 1;
    nodeweight = TRUE;

    this->addParameter("rank view", DM::STRING, &this->gravityview_name);
    this->addParameter("rank view", DM::STRING, &this->rankview_name);
    this->addParameter("reduction", DM::BOOL, &this->reduction);
    this->addParameter("ranking function", DM::STRING, &this->rank_function);
    this->addParameter("ranking function faktor", DM::DOUBLE, &this->rank_function_factor);
    this->addParameter("ranking weight", DM::DOUBLE, &this->rank_weight);
    this->addParameter("gravity weights", DM::BOOL, &this->nodeweight);
}

urbandevelRankEuclid::~urbandevelRankEuclid()
{

}

void urbandevelRankEuclid::init()
{
    gravityview = DM::View(gravityview_name, DM::NODE, DM::READ);
    rankview = DM::View(rankview_name, DM::FACE, DM::MODIFY);
    rankview_centroids = DM::View(rankview_name+"_CENTROIDS", DM::NODE, DM::READ);

    if (nodeweight) gravityview.addAttribute("weight",DM::Attribute::DOUBLE, DM::READ);

    std::vector<DM::View> data;
    data.push_back(gravityview);
    data.push_back(rankview);
    data.push_back(rankview_centroids);
    this->addData("data", data);
}

void urbandevelRankEuclid::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> areas = sys->getAllComponentsInView(rankview);
    std::vector<DM::Component *> areas_centroids = sys->getAllComponentsInView(rankview_centroids);
    std::vector<DM::Component *> gravitynodes = sys->getAllComponentsInView(gravityview);

    if (gravitynodes.size() < 1)
    {
        DM::Logger(DM::Warning) << "At least one gravity node expected. Zarro found";
        return;
    }

    std::string rankfieldname = "devrank";
    if (reduction) rankfieldname = "redrank";
    std::vector<double> distance;
    std::vector<int> rank;
    std::vector<int> oldrank;
    bool rnk_exists = FALSE;

    for (int i = 0; i < areas.size(); i++)
    {
        std::vector<DM::Component*> link = areas[i]->getAttribute(rankview_name+"_CENTROIDS")->getLinkedComponents();

        if (link.size() < 1)
        {
            DM::Logger(DM::Error) << "no area - centroid link";
            return;
        }

        DM::Node * centroid = dynamic_cast<DM::Node*>(link[0]);

        double dist = 0;

        for (int j = 0; j < gravitynodes.size(); j++)
        {
            DM::Node* currentnode = dynamic_cast<DM::Node*>(gravitynodes[j]);

            double currentdist = TBVectorData::calculateDistance((DM::Node*)currentnode, (DM::Node*)centroid);
            int gravityweight = 1;

            if (nodeweight) gravityweight = static_cast<int>(currentnode->getAttribute("gravity")->getDouble());
            currentdist*=gravityweight;
            dist+=currentdist;
        }

        distance.push_back(dist);

        oldrank.push_back(areas[i]->getAttribute(rankfieldname)->getDouble());
        if ( oldrank[i] > 0 ) { rnk_exists = TRUE; }
    }

    DAHelper::darank(distance, rank, rank_function, rank_function_factor);
    if (rnk_exists) { DAHelper::daweight(oldrank, rank, rank_weight); }

    for (int i = 0; i < areas.size(); i++)
    {
        dynamic_cast<DM::Face*>(areas[i])->changeAttribute(rankfieldname, rank[i]);
    }
}
