#include <urbandevelSetAttribute.h>
#include <dm.h>

DM_DECLARE_NODE_NAME(urbandevelSetAttribute, DynAlp)

urbandevelSetAttribute::urbandevelSetAttribute()
{
    view_name = "SUPERBLOCK";


    this->addParameter("View", DM::STRING, &this->view_name);
    this->addParameter("Check Attribute", DM::STRING, &this->checkattribute);
    this->addParameter("Check Value", DM::STRING, &this->checkvalue);
    this->addParameter("Set Attribute", DM::STRING, &this->setattribute);
    this->addParameter("Set Value", DM::STRING, &this->setvalue);
}

urbandevelSetAttribute::~urbandevelSetAttribute()
{

}

void urbandevelSetAttribute::init() {

    view = DM::View(view_name, DM::COMPONENT, DM::READ);

    view.addAttribute(checkattribute, DM::Attribute::STRING, DM::READ);
    view.addAttribute(setattribute, DM::Attribute::STRING, DM::WRITE);


    std::vector<DM::View> data;
    data.push_back(view);
    this->addData("data", data);
}

void urbandevelSetAttribute::run()
{
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> elements = sys->getAllComponentsInView(view);

    for (int i = 0; i < elements.size(); i++)
    {
        DM::Component* element = dynamic_cast<DM::Component*>(elements[i]);

        std::string curvalue= element->getAttribute(checkattribute)->getString();
        if (curvalue == checkvalue)
        {
            element->changeAttribute(setattribute, setvalue);
            DM::Logger(DM::Debug) << "Attribute Set";
        }
    }
}

string urbandevelSetAttribute::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelSetAttribute.md";
}
