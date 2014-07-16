#include "urbandevelWaterAttributes.h"
#include "dmgeometry.h"
#include "cgalgeometry.h"
#include "tbvectordata.h"
//#include <dmhelper.h>
//#include <tbvectordata.h>
//#include <littlegeometryhelpers.h>
#include <algorithm>

DM_DECLARE_NODE_NAME(urbandevelWaterAttributes, DynAlp)

urbandevelWaterAttributes::urbandevelWaterAttributes()
{
    width = 20;
    ratio = 1;
    buildingyear = 2010;
    stories = 3;
    dwf_per_person = 4.1;
    wsd_per_person = 200;
    space_required = 50;

    onSignal = TRUE;
    rotate90 = FALSE;
    paramfromCity = TRUE;
    createPopulation = TRUE;
    create3DGeometry = FALSE;

    this->addParameter("Parameters from City", DM::BOOL, &paramfromCity);
    this->addParameter("on Signal", DM::BOOL, &onSignal);
    this->addParameter("create Population", DM::BOOL, &createPopulation);

    this->addParameter("width", DM::DOUBLE, &width);
    this->addParameter("ratio", DM::DOUBLE, &ratio);
    this->addParameter("stories", DM::INT, &stories);
    this->addParameter("year", DM::INT, &buildingyear);

    this->addParameter("rotate90", DM::BOOL, &rotate90);
    this->addParameter("create3DGeometry", DM::BOOL, &create3DGeometry);

    this->addParameter("DWF per person", DM::DOUBLE, &dwf_per_person);
    this->addParameter("WSD per person", DM::DOUBLE, &wsd_per_person);
    this->addParameter("req. space per person", DM::DOUBLE, &space_required);
}

void urbandevelWaterAttributes::init()
{
    city = DM::View("CITY", DM::COMPONENT, DM::READ);

    parcels = DM::View("PARCEL", DM::FACE, DM::READ);
    parcels.addAttribute("status", DM::Attribute::DOUBLE, DM::WRITE);

    houses = DM::View("BUILDING", DM::FACE, DM::MODIFY);

    houses.addAttribute("centroid_x", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("centroid_y", DM::Attribute::DOUBLE, DM::WRITE);

    houses.addAttribute("year", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("stories", DM::Attribute::DOUBLE, DM::WRITE);

    houses.addAttribute("roofarea", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("roofarea_effective", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("trafficarea", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("trafficarea_effective", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("imperviousarea", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("imperviousarea_effective", DM::Attribute::DOUBLE, DM::WRITE);
    houses.addAttribute("Geometry", "Geometry", DM::WRITE);

    parcels.addAttribute("BUILDING", houses.getName(), DM::WRITE);
    houses.addAttribute("PARCEL", parcels.getName(), DM::WRITE);
    houses.addAttribute("Geometry", building_model.getName(), DM::WRITE);

    std::vector<DM::View> data;
    data.push_back(houses);
    data.push_back(parcels);
    if (create3DGeometry)
    {
        building_model = DM::View("Geometry", DM::FACE, DM::WRITE);
        building_model.addAttribute("type", DM::Attribute::DOUBLE, DM::WRITE);
        building_model.addAttribute("color", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
        building_model.addAttribute("parent", DM::Attribute::DOUBLE, DM::WRITE);
        data.push_back(building_model);
    }

    if (this->createPopulation)
    {
        city.addAttribute("required_space", DM::Attribute::DOUBLE, DM::READ);
        city.addAttribute("cyclepopdiff", DM::Attribute::DOUBLE, DM::READ);
    }
    if (this->paramfromCity)
    {
        city.addAttribute("year", DM::Attribute::DOUBLE, DM::READ);
        city.addAttribute("DWF_per_person", DM::Attribute::DOUBLE, DM::READ);
        city.addAttribute("WSD_per_person", DM::Attribute::DOUBLE, DM::READ);
    }
    data.push_back(city);
    this->addData("data", data);

}

void urbandevelWaterAttributes::run()
{
    DM::System * sys = this->getData("data");
    DM::SpatialNodeHashMap spatialNodeMap(sys, 100,false);

    std::vector<DM::Component *> cities = sys->getAllComponentsInView(city);
    if (cities.size() != 1 && paramfromCity)
    {
        DM::Logger(DM::Warning) << "One component expected. There are " << cities.size();
        return;
    }

    DM::Component * currentcity = cities[0];
    if (paramfromCity) buildingyear = currentcity->getAttribute("year")->getDouble();
    int cyclepopdiff = static_cast<int>(currentcity->getAttribute("cyclepopdiff")->getDouble());

    int numberOfHouseBuild = 0;

    foreach(DM::Component* c, sys->getAllComponentsInView(parcels))
    {
        if (cyclepopdiff == 0) continue;
        DM::Face* parcel = (DM::Face*)c;

        if (parcel->getAttribute("status")->getString() != "develop" && onSignal == true)
            continue;

        std::vector<DM::Node * > nodes  = TBVectorData::getNodeListFromFace(sys, parcel);

        std::vector<DM::Node> bB;
        //Calcualte bounding minial bounding box
        std::vector<double> size;
        double angle = CGALGeometry::CalculateMinBoundingBox(nodes, bB,size);
        if (rotate90) angle+=90;

        double length = width*ratio;

        Node centroid = DM::CGALGeometry::CalculateCentroid2D(parcel);

        double house_centx = centroid.getX();
        double house_centy = centroid.getY();

/*
       QPointF f1 (- length/2,  - width/2);
        QPointF f2 (+ length/2,- width/2);
        QPointF f3 ( + length/2,  + width/2);
        QPointF f4 (- length/2,  + width/2);

        QPolygonF original = QPolygonF() << f1 << f2 << f3 << f4;
        QTransform transform = QTransform().rotate(angle);
        QPolygonF rotated = transform.map(original);

        std::vector<DM::Node * > houseNodes;

        foreach (QPointF p, rotated) {
            houseNodes.push_back(spatialNodeMap.addNode(p.x()+centroid.getX(), p.y()+centroid.getY(), 0, 0.01, DM::View()));

        }
        if (houseNodes.size() < 2) {
            Logger(Error) << "Can't create House";
            continue;
        }
*/
        double roof_area = length*width;
        double traffic_area = roof_area/3;
        double impervious_area = TBVectorData::CalculateArea((DM::System*)sys, (DM::Face*)parcel) - roof_area - traffic_area;

        //DM::Face * building = sys->addFace(new Face(), houses);

        //Building 2D

        DM::Node * n1 = sys->addNode(house_centx-width/2,house_centy-length/2,0);
        DM::Node * n2 = sys->addNode(house_centx+width/2,house_centy-length/2,0);
        DM::Node * n3 = sys->addNode(house_centx+width/2,house_centy+length/2,0);
        DM::Node * n4 = sys->addNode(house_centx-width/2,house_centy+length/2,0);

        std::vector<DM::Node*> ve;
        ve.push_back(n1);
        ve.push_back(n2);
        ve.push_back(n3);
        ve.push_back(n4);
        ve.push_back(n1);

        DM::Face * building = sys->addFace(ve, houses);
        building->addAttribute("length", width);
        building->addAttribute("width", width);


        //Create Building and Footprints

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
        building->getAttribute("PARCEL")->addLink(parcel, parcels.getName());
        parcel->getAttribute("BUILDING")->addLink(building, houses.getName());

        numberOfHouseBuild++;

        //if (create3DGeometry) LittleGeometryHelpers::CreateStandardBuilding(sys, houses, building_model, building, houseNodes, stories);
/*        if (alpha > 10) {
            LittleGeometryHelpers::CreateRoofRectangle(city, houses, building_model, building, houseNodes, stories*3, alpha);
        }
*/


        if (createPopulation)
        {
            if (paramfromCity)
            {
                dwf_per_person = currentcity->getAttribute("DWF_per_person")->getDouble();
                wsd_per_person = currentcity->getAttribute("WSD_per_person")->getDouble();
                space_required = currentcity->getAttribute("space_required")->getDouble();

            }
            int peopleinbuilding = static_cast<int>(roof_area * stories / space_required);

            cyclepopdiff = std::max(cyclepopdiff - peopleinbuilding,0);
            double dwf = peopleinbuilding * dwf_per_person;
            double wsd = peopleinbuilding * wsd_per_person;

            building->addAttribute("DWF", dwf);
            building->addAttribute("WSD", wsd);
            building->addAttribute("POP", peopleinbuilding);
            building->addAttribute("height", stories*3.5);

            parcel->addAttribute("status", "populated");
            currentcity->addAttribute("cyclepopdiff", cyclepopdiff);
        }
    }

    DM::Logger(DM::Debug) << "Created Houses " << numberOfHouseBuild;
}

string urbandevelWaterAttributes::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-DynAlp/blob/master/doc/urbandevelWaterAttributes.md";
}
