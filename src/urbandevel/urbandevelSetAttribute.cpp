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
    this->addParameter("Check Value as Number", DM::BOOL, &this->cisnumber);
    this->addParameter("Set Value as Number", DM::BOOL, &this->sisnumber);
}

urbandevelSetAttribute::~urbandevelSetAttribute()
{

}

void urbandevelSetAttribute::init() {

    view = DM::View(view_name, DM::COMPONENT, DM::READ);
    view.addAttribute(checkattribute, DM::Attribute::STRING, DM::READ);

    if (sisnumber) {
        view.addAttribute(setattribute, DM::Attribute::DOUBLE, DM::WRITE);
    }
    else {
        view.addAttribute(setattribute, DM::Attribute::STRING, DM::WRITE);
    }

    std::vector<DM::View> data;
    data.push_back(view);
    this->addData("data", data);
}

void urbandevelSetAttribute::run()
{
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> elements = sys->getAllComponentsInView(view);

    QString qsetval = QString::fromStdString(setvalue);
    QString qcheckval = QString::fromStdString(checkvalue);


    for (int i = 0; i < elements.size(); i++)
    {
        DM::Component* element = dynamic_cast<DM::Component*>(elements[i]);

        QString curvalue = QString::fromStdString(element->getAttribute(checkattribute)->getString());

        //DM::Logger(DM::Warning) << "compare ... as" << sisnumber << " " << checkvalue << " == " << curvalue;

        if ( ( !cisnumber && curvalue == qcheckval ) || (cisnumber && curvalue.toDouble() == qcheckval.toDouble() ))
        {
            DM::Logger(DM::Debug) << "Match. Setting: " << setvalue;
            if (sisnumber) {
                element->changeAttribute(setattribute, qsetval.toDouble());
            }
            else {
                element->changeAttribute(setattribute, setvalue);
            }
            DM::Logger(DM::Debug) << "Attribute Set";
        }
    }
}

string urbandevelSetAttribute::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelSetAttribute.md";
}
