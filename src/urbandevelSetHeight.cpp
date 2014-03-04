#include<urbandevelSetHeight.h>
#include<dm.h>

DM_DECLARE_NODE_NAME(urbandevelSetHeight, DynAlp)

urbandevelSetHeight::urbandevelSetHeight()
{
}

urbandevelSetHeight::~urbandevelSetHeight()
{
}

void urbandevelSetHeight::init()
{
    city = DM::View("CITY", DM::NODE, DM::READ);
    sb = DM::View("SUPERBLOCK", DM::FACE, DM::MODIFY);
    cb = DM::View("CITYBLOCK", DM::FACE, DM::MODIFY);

    sb.addAttribute("type", DM::Attribute::DOUBLE, DM::WRITE);
    cb.addAttribute("type", DM::Attribute::DOUBLE, DM::WRITE);

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(city);
    views.push_back(sb);
    views.push_back(cb);
    this->addData("data", views);
}

void urbandevelSetHeight::run()
{
    // get data from stream/port
    DM::System * sys = this->getData("data");

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);
    if (cities.size() != 1)
    {
        DM::Logger(DM::Warning) << "Only one component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcity = cities[0];
}
