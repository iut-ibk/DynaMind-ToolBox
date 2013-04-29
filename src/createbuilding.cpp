#include "createbuilding.h"
#include "dmgeometry.h"
#include "cgalgeometry.h"
#include "tbvectordata.h"
#include <QPolygonF>
#include <QTransform>
#include <dmhelper.h>
#include <tbvectordata.h>
#include <littlegeometryhelpers.h>
#ifdef _OPENMP
#include <omp.h>
#endif

DM_DECLARE_NODE_NAME(CreateBuilding, DynAlp)

CreateBuilding::CreateBuilding()
{

    heatingT = 20;
    coolingT = 20;
    buildyear = 1985;
    stories = 2;
    l = 16;
    b = 10;
    alpha = 30;
    onSingal = false;

    this->addParameter("length", DM::DOUBLE, &l);
    this->addParameter("width", DM::DOUBLE, &b);
    this->addParameter("stories", DM::INT, &stories);
    this->addParameter("alpha", DM::DOUBLE, &alpha);
    this->addParameter("built_year", DM::INT, &buildyear);

    this->addParameter("T_heating", DM::DOUBLE, &heatingT);
    this->addParameter("T_cooling", DM::DOUBLE, &coolingT);

    this->addParameter("onSignal", DM::BOOL, &onSingal);

    cityView = DM::View("CITY", DM::FACE, DM::READ);
    cityView.getAttribute("year");
    parcels = DM::View("PARCEL", DM::FACE, DM::READ);
    parcels.addAttribute("is_built");

    parcels.getAttribute("released");
    parcels.getAttribute("centroid_x");
    parcels.getAttribute("centroid_y");

    houses = DM::View("BUILDING", DM::COMPONENT, DM::WRITE);

    houses.addAttribute("centroid_x");
    houses.addAttribute("centroid_y");

    houses.addAttribute("built_year");
    houses.addAttribute("stories");
    houses.addAttribute("stories_below");
    houses.addAttribute("stories_height");

    houses.addAttribute("floor_area");
    houses.addAttribute("roof_area");
    houses.addAttribute("gross_floor_area");

    houses.addAttribute("centroid_x");
    houses.addAttribute("centroid_y");

    houses.addAttribute("l_bounding");
    houses.addAttribute("b_bounding");
    houses.addAttribute("h_bounding");
    houses.addAttribute("alhpa_bounding");

    houses.addAttribute("alpha_roof");

    houses.addAttribute("cellar_used");
    houses.addAttribute("roof_used");

    houses.addAttribute("T_heating");
    houses.addAttribute("T_cooling");

    houses.addAttribute("Geometry");
    houses.addAttribute("V_living");

    footprint = DM::View("Footprint", DM::FACE, DM::WRITE);
    footprint.addAttribute("year");
    footprint.addAttribute("h");
    footprint.addAttribute("built_year");
    building_model = DM::View("Geometry", DM::FACE, DM::WRITE);
    building_model.addAttribute("type");

    parcels.addLinks("BUILDING", houses);
    houses.addLinks("PARCEL", parcels);

    std::vector<DM::View> data;
    data.push_back(houses);
    data.push_back(parcels);
    data.push_back(footprint);
    data.push_back(building_model);
    data.push_back(cityView);
    this->addData("City", data);
}



void CreateBuilding::run()
{
    DM::System * city = this->getData("City");
    DM::SpatialNodeHashMap spatialNodeMap(city, 100);

    std::vector<std::string> city_uuid = city->getUUIDs(cityView);
    if (city_uuid.size() != 0) {
        buildyear = city->getComponent(city_uuid[0])->getAttribute("year")->getDouble();
    }

    std::vector<std::string> parcelUUIDs = city->getUUIDs(parcels);

    int nparcels = parcelUUIDs.size();
    int numberOfHouseBuild = 0;

    for (int i = 0; i < nparcels; i++) {
        DM::Face * parcel = city->getFace(parcelUUIDs[i]);

        if (parcel->getAttribute("released")->getDouble() < 0.01 && onSingal == true)
            continue;
        if (parcel->getAttribute("is_built")->getDouble() > 0.01)
            continue;
        std::vector<DM::Node * > nodes  = TBVectorData::getNodeListFromFace(city, parcel);
        
        std::vector<DM::Node> bB;
        //Calcualte bounding minial bounding box
        std::vector<double> size;
        double angle = CGALGeometry::CalculateMinBoundingBox(nodes, bB,size);
        Node centroid = DM::Node(parcel->getAttribute("centroid_x")->getDouble(),  parcel->getAttribute("centroid_y")->getDouble(), 0);

        QPointF f1 (- l/2,  - b/2);
        QPointF f2 (+ l/2,- b/2);
        QPointF f3 ( + l/2,  + b/2);
        QPointF f4 (- l/2,  + b/2);

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
        houseNodes.push_back(houseNodes[0]);

        DM::Component * building = city->addComponent(new Component(), houses);


        //Create Building and Footprints
        DM::Face * foot_print = city->addFace(houseNodes, footprint);
        foot_print->addAttribute("year", buildyear);
        foot_print->addAttribute("built_year", buildyear);
        foot_print->addAttribute("height", stories*3);
        Node  n = TBVectorData::CaclulateCentroid(city, foot_print);
        building->addAttribute("type", "single_family_house");
        building->addAttribute("built_year", buildyear);
        building->addAttribute("stories", stories);
        building->addAttribute("stories_below", 0); //cellar counts as story
        building->addAttribute("stories_height",3 );

        building->addAttribute("floor_area", l*b);
        building->addAttribute("roof_area", l*b);
        building->addAttribute("gross_floor_area", l * b * stories * 3);

        building->addAttribute("centroid_x", n.getX());
        building->addAttribute("centroid_y", n.getY());

        building->addAttribute("l_bounding", l);
        building->addAttribute("b_bounding", b);
        building->addAttribute("h_bounding", stories * 3);

        building->addAttribute("alpha_bounding", angle);

        building->addAttribute("alpha_roof", alpha);

        building->addAttribute("cellar_used", 1);
        building->addAttribute("roof_used", 0);

        building->addAttribute("T_heating", heatingT);
        building->addAttribute("T_cooling", coolingT);


        building->addAttribute("V_living", l*b*stories * 3);

        LittleGeometryHelpers::CreateStandardBuilding(city, houses, building_model, building, houseNodes, stories);
        if (alpha > 10) {
            LittleGeometryHelpers::CreateRoofRectangle(city, houses, building_model, building, houseNodes, stories*3, alpha);
        }

        //Create Links
        building->getAttribute("PARCEL")->setLink(parcels.getName(), parcel->getUUID());
        parcel->getAttribute("BUILDING")->setLink(houses.getName(), building->getUUID());
        parcel->addAttribute("is_built",1);
        numberOfHouseBuild++;
        
    }
    Logger(Debug) << "Created Houses " << numberOfHouseBuild;
}
