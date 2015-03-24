#include <urbandevelBuilding.h>
#include <cgalgeometry.h>
#include <tbvectordata.h>
#include <dm.h>

DM_DECLARE_NODE_NAME(urbandevelBuilding, DynAlp)

urbandevelBuilding::urbandevelBuilding()
{
    offset = 2;
    buildingyear = 1900;
    stories = 3;

    onSignal = TRUE;
    paramfromCity = TRUE;
    genPopulation = TRUE;
    spacepp = 45;
    buildingtype = "res";

    this->addParameter("on Signal", DM::BOOL, &onSignal);
    this->addParameter("Parameters from City", DM::BOOL, &paramfromCity);
    this->addParameter("create Population", DM::BOOL, &genPopulation);

    this->addParameter("offset", DM::DOUBLE, &offset);
    this->addParameter("stories", DM::INT, &stories);
    this->addParameter("year", DM::INT, &buildingyear);
    this->addParameter("space per person", DM::INT, &spacepp);
    this->addParameter("check type (ignore if empty)", DM::STRING, &buildingtype);
}

urbandevelBuilding::~urbandevelBuilding()
{

}

void urbandevelBuilding::init()
{
    cityview = DM::View("CITY", DM::COMPONENT, DM::READ);
    if (this->genPopulation)
    {
        cityview.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);
    }
    if (this->paramfromCity)
    {
        cityview.addAttribute("required_space", DM::Attribute::DOUBLE, DM::READ);
        cityview.addAttribute("year", DM::Attribute::DOUBLE, DM::READ);
        cityview.addAttribute("offset", DM::Attribute::DOUBLE, DM::READ);
    }
    parcelview = DM::View("PARCEL", DM::FACE, DM::MODIFY);
    parcelview.addAttribute("status", DM::Attribute::DOUBLE, DM::MODIFY);
    parcelview.addAttribute("BUILDING", buildingview.getName(), DM::WRITE);

    buildingview = DM::View("BUILDING", DM::FACE, DM::MODIFY);
    buildingview.addAttribute("centroid_x", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("centroid_y", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("year", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("stories", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("A_roof", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("Aeff_roof", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("A_road", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("Aeff_road", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("A_imp", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("Aeff_imp", DM::Attribute::DOUBLE, DM::WRITE);
    buildingview.addAttribute("PARCEL", parcelview.getName(), DM::WRITE);

    std::vector<DM::View> data;
    data.push_back(buildingview);
    data.push_back(parcelview);
    data.push_back(cityview);
    this->addData("data", data);

}

void urbandevelBuilding::run()
{
    DM::System * sys = this->getData("data");
    std::vector<DM::Component *> cities = sys->getAllComponentsInView(cityview);
    std::vector<DM::Component *> parcels = sys->getAllComponentsInView(parcelview);

    if (cities.size() != 1 && paramfromCity)
    {
        DM::Logger(DM::Warning) << "One component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcity = cities[0];

    if (paramfromCity)
    {
        buildingyear = static_cast<int>(currentcity->getAttribute("year")->getDouble());
        offset = static_cast<int>(currentcity->getAttribute("ratio")->getDouble());
    }

    int cyclepopdiff = static_cast<int>(currentcity->getAttribute("cyclepopdiff")->getDouble());
    int currentyear = static_cast<int>(currentcity->getAttribute("currentyear")->getDouble());

    int numberOfHouseBuild = 0;

    for (int i = 0; i < parcels.size(); i++)
    {
        DM::Face * currentparcel = dynamic_cast<DM::Face *>(parcels[i]);
        // do not generate houses if no population (if population should be generated) is available
        // OR no parcel status equals develop (if development should happen on signal

        if ((cyclepopdiff == 0 && genPopulation) || (parcels[i]->getAttribute("status")->getString() != "develop" && parcels[i]->getAttribute("type")->getString() ==  buildingtype &&!onSignal))
            continue;

        //calculate house from parcel with offset

        std::vector<std::vector<DM::Node> > result_nodes = DM::CGALGeometry::OffsetPolygon(currentparcel->getNodePointers(), offset);

        //taking only first polygon result

        std::vector<DM::Node*> buildingnodes;
        foreach (DM::Node n, result_nodes[0])
        {
            DM::Node * np = sys->addNode(n);
                if (np) buildingnodes.push_back(np);
        }
        if (buildingnodes.size() < 3) {
                DM::Logger(DM::Warning) << "offest failed";
        }

        DM::Face * building = sys->addFace(buildingnodes, buildingview);

        double roof_area = TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)building);
        double parcel_area = TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)currentparcel);
        double traffic_area = (parcel_area - roof_area)/3;
        double impervious_area = parcel_area - traffic_area - roof_area;

        //Attributes

        building->addAttribute("type", "residential");
        building->addAttribute("year", buildingyear);
        building->addAttribute("stories", stories);

        building->addAttribute("roofarea", roof_area);
        building->addAttribute("roofarea_effective", 0.8);
        building->addAttribute("trafficarea", traffic_area);
        building->addAttribute("trafficarea_effective", 0.9);
        building->addAttribute("imperviousarea", impervious_area);
        building->addAttribute("imperviousarea_effective", 0.1);

        //Create Links
        building->getAttribute("PARCEL")->addLink(currentparcel, parcelview.getName());
        currentparcel->getAttribute("BUILDING")->addLink(building, buildingview.getName());

        numberOfHouseBuild++;

        if (genPopulation)
        {
            int peopleinbuilding = static_cast<int>(roof_area * stories / spacepp);
            cyclepopdiff = std::max(cyclepopdiff - peopleinbuilding,0);
            building->addAttribute("POP", peopleinbuilding);
            currentcity->addAttribute("cyclepopdiff", cyclepopdiff);
        }
        currentparcel->addAttribute("status", "populated");
        building->addAttribute("height", stories*4);
        building->addAttribute("type", buildingtype);
    }

    DM::Logger(DM::Warning) << "Created Houses " << numberOfHouseBuild << " of type " << buildingtype;
}

string urbandevelBuilding::getHelpUrl()
{
    return "http://dynalp.com/documentation/DynaMind/html/urbandevelBuilding.html";
}
