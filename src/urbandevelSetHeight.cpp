#include<urbandevelSetHeight.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>
#include<algorithm>
#include<iterator>

DM_DECLARE_NODE_NAME(urbandevelSetHeight, DynAlp)

urbandevelSetHeight::urbandevelSetHeight()
{
    numbernearest = 3;
    this->addParameter("number of buildings", DM::DOUBLE, &this->numbernearest);
}

urbandevelSetHeight::~urbandevelSetHeight()
{
}

void urbandevelSetHeight::init()
{
    city = DM::View("CITY", DM::NODE, DM::READ);
    sb = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    bd = DM::View("BUILDING", DM::FACE, DM::READ);
    sb_cent = DM::View("SUPERBLOCK_CENTROIDS", DM::NODE, DM::READ);
    bd_cent = DM::View("BUILDING_CENTROIDS", DM::NODE, DM::READ);

    sb.addAttribute("height", DM::Attribute::DOUBLE, DM::WRITE);
    bd.addAttribute("height", DM::Attribute::DOUBLE, DM::READ);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(sb);
    views.push_back(bd);
    views.push_back(sb_cent);
    views.push_back(bd_cent);
    this->addData("data", views);
}

void urbandevelSetHeight::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> superblocks = sys->getAllComponentsInView(sb);
    std::vector<DM::Component *> superblocks_centroids = sys->getAllComponentsInView(sb_cent);
    std::vector<DM::Component *> buildings = sys->getAllComponentsInView(bd);
    std::vector<DM::Component *> buildings_centroids = sys->getAllComponentsInView(bd_cent);

    int max = static_cast<int>(numbernearest);

    if ( buildings.size() < numbernearest )
        max = buildings.size();

    std::vector<int> distance;
    std::vector<int> height;
    std::map<int, int> distheight;

    for (int i = 0; i < superblocks.size(); i++)
    {
        std::vector<DM::Component*> sblink = superblocks[i]->getAttribute("SUPERBLOCK_CENTROIDS")->getLinkedComponents();

        if( sblink.size() < 1 )
        {
            DM::Logger(DM::Error) << "no area - centroid link";
            return;
        }

        DM::Node * sbcentroid = dynamic_cast<DM::Node*>(sblink[0]);

        for (int j = 0; j < buildings.size(); j++)
        {
            std::vector<DM::Component*> bdlink = buildings[j]->getAttribute("BUILDING_CENTROIDS")->getLinkedComponents();

            if( bdlink.size() < 1 )
            {
                DM::Logger(DM::Error) << "no area - centroid link";
                return;
            }

            DM::Node * bdcentroid = dynamic_cast<DM::Node*>(bdlink[0]);

            distance.push_back(static_cast<int>( TBVectorData::calculateDistance((DM::Node*)sbcentroid,(DM::Node*)bdcentroid) ));
            height.push_back(static_cast<int>( buildings[j]->getAttribute("height")->getDouble())) ;

        }

        if (distance.size() != height.size() )
        {
            DM::Logger(DM::Error) << "distance and height vector lengths differ";
            return;
        }
        for (int k = 0; k < distance.size(); k++)
        {
            distheight[distance[k]] = height[k];

        }
        int avgheight = 0;

        std::map<int,int>::iterator element = distheight.begin();


        for (int k = 0; k < max; k++)
        {
            std::advance(element,k);
            avgheight = avgheight + element->second;
            DM::Logger(DM::Debug) << "heightinc " << avgheight;
        }

        avgheight = avgheight/max;
        DM::Logger(DM::Debug) << "avgheight " << avgheight;
        superblocks[i]->changeAttribute("height", avgheight);
    }
}
