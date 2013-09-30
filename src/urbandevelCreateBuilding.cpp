#include <urbandevelCreateBuilding.h>
#include <dm.h>

DM_DECLARE_NODE_NAME(urbandevelCreateBuilding, DynAlp)

urbandevelCreateBuilding::urbandevelCreateBuilding()
{
    // declare parameters
    this->addParameter("Length", DM::INT, &this->length);
    this->addParameter("Ratio", DM::DOUBLE, &this->ratio);
    this->addParameter("Stories", DM::INT, &this->stories);
    this->addParameter("Year", DM::INT, &this->year);
}

urbandevelCreateBuilding::~urbandevelCreateBuilding()
{
}

void urbandevelCreateBuilding::init()
{
    // create a view
    buildings = DM::View("BUILDINGS", DM::COMPONENT, DM::WRITE);
    parcels = DM::View("PARCELS", DM::COMPONENT, DM::MODIFY);

    // attach new attributes to view
    buildings.addAttribute("width");
    buildings.addAttribute("ratio");
    buildings.addAttribute("stories");
    buildings.addAttribute("year");

    // push the view-access settings into the module via 'addData'
    std::vector<DM::View> views;
    views.push_back(buildings);
    views.push_back(parcels);
    this->addData("city", views);
}

void urbandevelCreateBuilding::run()
{
    // get data from stream/port
    srand ( NULL );

    DM::System * data = this->getData("city");
    mforeach(DM::Component * parcel, data->getAllComponentsInView(parcels))
    {
        DM::Attribute* parcel_empty = parcel->getAttribute("empty");
        if (parcel_empty) {
            if (parcel->getAttribute("maxheight")->getDouble() > 0) { stories = parcel->getAttribute("maxheight")->getDouble(); }

            stories = (int)(parcel->getAttribute("maxheight")->getDouble());
            int length_rand = static_cast<int>(length/4*3 + (rand() % (length/3)));
            int width_rand = static_cast<int>(length_rand*ratio);




            parcel->getAttribute("centroid_x"); //should be calculated here?
            parcel->getAttribute("centroid_y");
            parcel->changeAttribute("empty", 0);

        }
        //g->setDouble(g->getDouble() * 2.0);
    }

    // be sure to destruct any objects allocated with malloc or new!
}

/*
    for (int i = 0; i < nparcels; i++) {
        DM::Face * parcel = city->getFace(parcelUUIDs[i]);

        if (parcel->getAttribute("released")->getDouble() < 0.01)
            continue;
        if (parcel->getAttribute("is_built")->getDouble() > 0.01)
            continue;
        if (DatafromView) {
            stories = (int)(parcel->getAttribute("maxheight")->getDouble());
        }

        std::vector<DM::Node * > nodes  = TBVectorData::getNodeListFromFace(city, parcel);

        int length_rand = static_cast<int>(length/4*3 + (rand() % (length/3)));
        int width_rand = static_cast<int>(length_rand*ratio);

        DM::Logger(DM::Warning) << "Calc stories rand";

        //int stories_rand = 0;

        //stories_rand = (stories*2)/3 + ( rand() % (stories/2));
        int stories_rand = (stories*2/3) + (rand() % ((stories/2)+2));

        DM::Logger(DM::Warning) << "Calc stories rand: " << stories_rand;


        std::vector<DM::Node> bB;
        //Calcualte bounding minial bounding box
        std::vector<double> size;
        double angle = CGALGeometry::CalculateMinBoundingBox(nodes, bB,size);
        Node centroid = DM::Node(parcel->getAttribute("centroid_x")->getDouble(),  parcel->getAttribute("centroid_y")->getDouble(), 0);

        QPointF f1 (- length_rand/2,  - width_rand/2);
        QPointF f2 (+ length_rand/2,- width_rand/2);
        QPointF f3 ( + length_rand/2,  + width_rand/2);
        QPointF f4 (- length_rand/2,  + width_rand/2);

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
        foot_print->addAttribute("height", stories_rand*3);
        Node  n = TBVectorData::PointWithinFace(city, foot_print);
        building->addAttribute("type", "single_family_house");
        building->addAttribute("built_year", buildyear);
        building->addAttribute("stories", stories_rand);
        building->addAttribute("stories_below", 0); //cellar counts as story
        building->addAttribute("stories_height",3 );

        building->addAttribute("floor_area", length_rand*width_rand);
        building->addAttribute("roof_area", length_rand*width_rand);
        building->addAttribute("total_floor_area", length_rand * width_rand * stories_rand);
        building->addAttribute("volume", length_rand*width_rand*stories_rand*3);

        building->addAttribute("centroid_x", n.getX());
        building->addAttribute("centroid_y", n.getY());

        building->addAttribute("l_bounding", length_rand);
        building->addAttribute("b_bounding", width_rand);
        building->addAttribute("h_bounding", stories_rand * 3);

        building->addAttribute("DWF", length_rand * width_rand * stories_rand / 10000 );
        building->addAttribute("EIA", length_rand * width_rand * 1.3);

        building->addAttribute("cellar_used", 1);
        building->addAttribute("roof_used", 0);
        building->addAttribute("color", "0.3;0.4;0.8");

        totalareaofhouses = totalareaofhouses + (length_rand * width_rand);

        LittleGeometryHelpers::CreateStandardBuilding(city, houses, building_model, building, houseNodes, stories_rand);

        //Create Links
        building->getAttribute("PARCEL")->setLink(parcels.getName(), parcel->getUUID());
        parcel->getAttribute("BUILDING")->setLink(houses.getName(), building->getUUID());
        parcel->addAttribute("is_built",1);
        parcel->addAttribute("DWF", length_rand * width_rand * stories_rand / 10000);
        parcel->addAttribute("EIA", length_rand * width_rand * 1.3);
        numberOfHouseBuild++;

    }
    Logger(Debug) << "Created Houses " << numberOfHouseBuild;
}
*/



    /*
    #include "urbandevelCreateBuilding.h"
    #include "cgalskeletonisation.h"
    #include <tbvectordata.h>
    #include <dmgeometry.h>

    DM_DECLARE_NODE_NAME(urbandevelCreateBuilding, DynAlp)

    urbandevelCreateBuilding::urbandevelCreateBuilding()
    {

        //init values

        length = 30;
        ratio = 0.8;
        stories = 3;
        year = 2000;

        this->addParameter("length", DM::INT, &length);
        this->addParameter("aspect ratio", DM::DOUBLE, &ratio);
        this->addParameter("stories", DM::INT, &stories);
        this->addParameter("build year", DM::INT, &year);

        DM::View city = getViewInStream("DATA", "CITY");

        if city.getAttribute("year");

    }

    void urbandevelCreateBuilding::run()
    {



        this->addParameter("length", DM::INT, &length);
        this->addParameter("aspect ratio", DM::DOUBLE, &ratio);
        this->addParameter("stories", DM::INT, &stories);

        this->addParameter("Data from View", DM::BOOL, &DatafromView);

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


    void urbandevelCreateBuilding::run()
    {

        srand ( time(NULL) );

        DM::System * city = this->getData("City");
        DM::SpatialNodeHashMap spatialNodeMap(city, 100);

        std::vector<std::string> city_uuid = city->getUUIDs(cityView);
        if (city_uuid.size() != 0) {
            buildyear = city->getComponent(city_uuid[0])->getAttribute("year")->getDouble();
        }

        std::vector<std::string> parcelUUIDs = city->getUUIDs(parcels);

        int nparcels = parcelUUIDs.size();
        int numberOfHouseBuild = 0;
        int totalareaofhouses = 0;
*/

