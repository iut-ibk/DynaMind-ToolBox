#include<urbandevelSetType.h>
#include<dm.h>
#include<tbvectordata.h>
#include<dmgeometry.h>
#include<dahelper.h>
#include<algorithm>
#include<iterator>

DM_DECLARE_NODE_NAME(urbandevelSetType, DynAlp)

urbandevelSetType::urbandevelSetType()
{
}

urbandevelSetType::~urbandevelSetType()
{
}

void urbandevelSetType::init()
{
    city = DM::View("CITY", DM::NODE, DM::READ);
    sb = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    sb_cent = DM::View("SUPERBLOCK_CENTROIDS", DM::NODE, DM::READ);

    sb.addAttribute("type", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(sb);
    views.push_back(sb_cent);
    this->addData("data", views);
}

void urbandevelSetType::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> superblocks = sys->getAllComponentsInView(sb);
    std::vector<DM::Component *> superblocks_centroids = sys->getAllComponentsInView(sb_cent);

    std::vector<int> distance;
    std::vector<std::string> type;
    std::map<int, std::string> disttype;

    for (int active = 0; active < superblocks.size(); active++)
    {

        std::string actualtype = superblocks[active]->getAttribute("type")->getString();

        if ( actualtype != "")
            continue;

        std::vector<DM::Component*> sblinkactive = superblocks[active]->getAttribute("SUPERBLOCK_CENTROIDS")->getLinkedComponents();

        if( sblinkactive.size() < 1 )
        {
            DM::Logger(DM::Error) << "no area - centroid link";
            return;
        }

        DM::Node * sbcentroidactive = dynamic_cast<DM::Node*>(sblinkactive[0]);

        for (int compare = 0; compare < superblocks.size(); compare++)
        {
            std::string comparetype = superblocks[compare]->getAttribute("type")->getString();

            if ( active == compare || comparetype.empty() )
                continue;

            std::vector<DM::Component*> sblinkcompare = superblocks[compare]->getAttribute("SUPERBLOCK_CENTROIDS")->getLinkedComponents();

            if( sblinkcompare.size() < 1 )
            {
                DM::Logger(DM::Error) << "no area - centroid link";
                return;
            }

            DM::Node * sbcentroidcompare = dynamic_cast<DM::Node*>(sblinkcompare[0]);

            distance.push_back(static_cast<int>( TBVectorData::calculateDistance((DM::Node*)sbcentroidactive,(DM::Node*)sbcentroidcompare) ));
            type.push_back(static_cast<string>( superblocks[compare]->getAttribute("type")->getString()));
            DM::Logger(DM::Warning) << "distance " << distance[compare] << "type " << type[compare];
        }

        if (distance.size() != type.size() )
        {
            DM::Logger(DM::Error) << "distance and height vector lengths differ";
            return;
        }
        for (size_t k = 0; k < distance.size(); ++k)
            disttype[distance[k]] = type[k];

        int avgheight = 0;

        std::map<int,std::string>::iterator element = disttype.begin();

        for (int k = 0; k < distance.size(); k++)
        {
            std::advance(element,k);
            avgheight = avgheight; //+ element->second;
        }

        superblocks[active]->changeAttribute("type", avgheight);
    }
}
