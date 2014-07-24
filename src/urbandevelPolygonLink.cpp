#include "urbandevelPolygonLink.h"
#include <tbvectordata.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelPolygonLink, DynAlp)

urbandevelPolygonLink::urbandevelPolygonLink()
{
    blockview_name = "SUPERBLOCK";
    elementview_name = "CITYBLOCK";

    this->addParameter("Blocks", DM::STRING, &this->blockview_name);
    this->addParameter("Elements", DM::STRING, &this->elementview_name);
}

urbandevelPolygonLink::~urbandevelPolygonLink()
{

}

void urbandevelPolygonLink::init() {

    blockview = DM::View(blockview_name, DM::FACE, DM::READ);
    elementview = DM::View(elementview_name, DM::FACE, DM::READ);
    elementcentroidview = DM::View(elementview_name+"_CENTROIDS", DM::NODE, DM::READ);

    blockview.addAttribute(elementview_name, elementview_name, DM::WRITE);
    elementview.addAttribute(blockview_name, blockview_name, DM::WRITE);
    elementview.addAttribute("type", DM::Attribute::STRING, DM::WRITE);

    blockview.addAttribute("status", DM::Attribute::STRING, DM::WRITE);
    blockview.addAttribute("type", DM::Attribute::STRING, DM::READ);

    std::vector<DM::View> data;
    data.push_back(blockview);
    data.push_back(elementview);
    data.push_back(elementcentroidview);
    this->addData("data", data);
}

void urbandevelPolygonLink::run()
{
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> blocks = sys->getAllComponentsInView(blockview);
    std::vector<DM::Component *> elements = sys->getAllComponentsInView(elementview);
    std::vector<DM::Component *> centroids = sys->getAllComponentsInView(elementcentroidview);

    for (int i = 0; i < blocks.size(); i++)
    {

        std::string status = "empty";

        DM::Face* block = dynamic_cast<DM::Face*>(blocks[i]);

        for (int j = 0; j < elements.size(); j++)
        {
            DM::Face* element = dynamic_cast<DM::Face*>(elements[j]);
            std::vector<DM::Component*> link = element->getAttribute(elementview_name+"_CENTROIDS")->getLinkedComponents();

            if(link.size() < 1)
            {
                DM::Logger(DM::Error) << "no area - centroid link";
                return;
            }

            DM::Node* centroid = dynamic_cast<DM::Node*>(link[0]);

            if (TBVectorData::PointWithinFace((DM::Face*)block, (DM::Node*)centroid))
            {
                std::string type = block->getAttribute("type")->getString();
                block->getAttribute(elementview.getName())->addLink(element, elementview.getName()); //Link SB->CB
                element->getAttribute(blockview.getName())->addLink(block, blockview.getName()); //Link CB->SB
                element->changeAttribute("type", type);
                DM::Logger(DM::Debug) << "Link created";
                status = "populated";
            }
        }

        block->addAttribute("status", status);
    }
}

string urbandevelPolygonLink::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelBuilding.md";
}
