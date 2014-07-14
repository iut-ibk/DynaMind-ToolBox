#include "urbandevelPolygonLink.h"
#include <tbvectordata.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelPolygonLink, DynAlp)

urbandevelPolygonLink::urbandevelPolygonLink()
{
    blocks_name = "SUPERBLOCK";
    elements_name = "CITYBLOCK";

    this->addParameter("Blocks", DM::STRING, &this->blocks_name);
    this->addParameter("Elements", DM::STRING, &this->elements_name);
}

void urbandevelPolygonLink::init() {

    blocks_view = DM::View(blocks_name, DM::FACE, DM::READ);
    elements_view = DM::View(elements_name, DM::FACE, DM::READ);
    elements_centroids_view = DM::View(elements_name+"_CENTROIDS", DM::NODE, DM::READ);

    blocks_view.addAttribute(elements_name, elements_name, DM::WRITE);
    elements_view.addAttribute(blocks_name, blocks_name, DM::WRITE);

    blocks_view.addAttribute("status", DM::Attribute::STRING, DM::WRITE);

    std::vector<DM::View> views;
    views.push_back(blocks_view);
    views.push_back(elements_view);
    views.push_back(elements_centroids_view);
    this->addData("data", views);
}

void urbandevelPolygonLink::run()
{
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> blocks = sys->getAllComponentsInView(blocks_view);
    std::vector<DM::Component *> elements = sys->getAllComponentsInView(elements_view);
    std::vector<DM::Component *> centroids = sys->getAllComponentsInView(elements_centroids_view);

    for (int i = 0; i < blocks.size(); i++)
    {

        std::string status = "empty";

        DM::Face* block = dynamic_cast<DM::Face*>(blocks[i]);

        for (int j = 0; j < elements.size(); j++)
        {
            DM::Face* element = dynamic_cast<DM::Face*>(elements[j]);
            std::vector<DM::Component*> link = element->getAttribute(elements_name+"_CENTROIDS")->getLinkedComponents();

            if(link.size() < 1)
            {
                DM::Logger(DM::Error) << "no area - centroid link";
                return;
            }

            DM::Node* centroid = dynamic_cast<DM::Node*>(link[0]);

            if (TBVectorData::PointWithinFace((DM::Face*)block, (DM::Node*)centroid))
            {
                block->getAttribute(elements_view.getName())->addLink(element, elements_view.getName()); //Link SB->CB
                element->getAttribute(blocks_view.getName())->addLink(block, blocks_view.getName()); //Link CB->SB
                DM::Logger(DM::Debug) << "Link created";
                status = "populated";
            }
        }

        block->addAttribute("status", status);
    }
}
