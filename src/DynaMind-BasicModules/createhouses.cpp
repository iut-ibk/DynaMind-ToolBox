#include "createhouses.h"
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

DM_DECLARE_NODE_NAME(CreateHouses, BlockCity)

CreateHouses::CreateHouses()
{

	heatingT = 20;
	coolingT = 20;
	buildyear = 1985;
	stories = 2;
	l = 16;
	b = 10;
	alpha = 30;
	onSingal = false;
	l_on_parcel_b = false;
	yearFromCity = true;
	create3DGeometry = true;

	this->addParameter("l", DM::DOUBLE, &l);
	this->addParameter("b", DM::DOUBLE, &b);
	this->addParameter("stories", DM::INT, &stories);
	this->addParameter("alhpa", DM::DOUBLE, &alpha);
	this->addParameter("built_year", DM::INT, &buildyear);
	this->addParameter("year_from_city", DM::BOOL, &yearFromCity);

	this->addParameter("T_heating", DM::DOUBLE, &heatingT);
	this->addParameter("T_cooling", DM::DOUBLE, &coolingT);

	this->addParameter("onSignal", DM::BOOL, &onSingal);

	this->addParameter("l_on_parcel_b", DM::BOOL, &l_on_parcel_b);
	this->addParameter("create3DGeometry", DM::BOOL, &create3DGeometry);

}


void CreateHouses::run()
{
	DM::System * city = this->getData("City");
	DM::SpatialNodeHashMap spatialNodeMap(city, 100,false);

	std::vector<Component*> city_comps = city->getAllComponentsInView(cityView);
	if (city_comps.size() != 0)
		buildyear = city_comps[0]->getAttribute("year")->getDouble();

	int numberOfHouseBuild = 0;

	foreach(DM::Component* c, city->getAllComponentsInView(parcels))
	{
		DM::Face* parcel = (DM::Face*)c;

		if (parcel->getAttribute("selected")->getDouble() < 0.01 && onSingal == true)
			continue;
		if (parcel->getAttribute("is_built")->getDouble() > 0.01)
			continue;
		std::vector<DM::Node * > nodes  = TBVectorData::getNodeListFromFace(city, parcel);

		std::vector<DM::Node> bB;
		//Calcualte bounding minial bounding box
		std::vector<double> size;
		double angle = CGALGeometry::CalculateMinBoundingBox(nodes, bB,size);
		if (l_on_parcel_b)
			angle+=90;

		Node centroid = DM::CGALGeometry::CaclulateCentroid2D(parcel);

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

		DM::Component * building = city->addComponent(new Component(), houses);

		//Create Building and Footprints
		DM::Face * foot_print = city->addFace(houseNodes, footprint);
		foot_print->addAttribute("area", DM::CGALGeometry::CalculateArea2D(foot_print));
		foot_print->addAttribute("built_year", buildyear);
		foot_print->addAttribute("height", stories*3);
		Node  n = DM::CGALGeometry::CaclulateCentroid2D(foot_print);
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

		//Create Links
		building->getAttribute("Footprint")->addLink(foot_print, footprint.getName());
		foot_print->getAttribute("BUILDING")->addLink(building, houses.getName());
		building->getAttribute("PARCEL")->addLink(parcel, parcels.getName());
		parcel->getAttribute("BUILDING")->addLink(building, houses.getName());
		parcel->addAttribute("is_built",1);
		numberOfHouseBuild++;

		if (!create3DGeometry) {
			city->addComponentToView(foot_print, building_model);
			building->getAttribute("Geometry")->addLink(foot_print, building_model.getName());
			continue;
		}
		LittleGeometryHelpers::CreateStandardBuilding(city, houses, building_model, building, houseNodes, stories);
		if (alpha > 10) {
			LittleGeometryHelpers::CreateRoofRectangle(city, houses, building_model, building, houseNodes, stories*3, alpha);
		}



	}
	Logger(Standard) << "Created Houses " << numberOfHouseBuild;
}

void CreateHouses::init()
{
	cityView = DM::View("CITY", DM::COMPONENT, DM::READ);
	cityView.addAttribute("year", DM::Attribute::DOUBLE, DM::READ);

	parcels = DM::View("PARCEL", DM::FACE, DM::READ);
	parcels.addAttribute("is_built", DM::Attribute::DOUBLE, DM::WRITE);
	if (this->onSingal)
		parcels.addAttribute("selected", DM::Attribute::DOUBLE, DM::READ);

	houses = DM::View("BUILDING", DM::COMPONENT, DM::WRITE);

	houses.addAttribute("centroid_x", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("centroid_y", DM::Attribute::DOUBLE, DM::WRITE);

	houses.addAttribute("built_year", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("stories", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("stories_below", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("stories_height", DM::Attribute::DOUBLE, DM::WRITE);

	houses.addAttribute("floor_area", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("roof_area", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("gross_floor_area", DM::Attribute::DOUBLE, DM::WRITE);

	houses.addAttribute("l_bounding", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("b_bounding", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("h_bounding", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("alhpa_bounding", DM::Attribute::DOUBLE, DM::WRITE);

	houses.addAttribute("alpha_roof", DM::Attribute::DOUBLE, DM::WRITE);

	houses.addAttribute("cellar_used", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("roof_used", DM::Attribute::DOUBLE, DM::WRITE);

	houses.addAttribute("T_heating", DM::Attribute::DOUBLE, DM::WRITE);
	houses.addAttribute("T_cooling", DM::Attribute::DOUBLE, DM::WRITE);

	houses.addAttribute("Geometry", "Geometry", DM::WRITE);
	houses.addAttribute("V_living", DM::Attribute::DOUBLE, DM::WRITE);

	footprint = DM::View("Footprint", DM::FACE, DM::WRITE);

	footprint.addAttribute("h", DM::Attribute::DOUBLE, DM::WRITE);
	footprint.addAttribute("built_year", DM::Attribute::DOUBLE, DM::WRITE);
	footprint.addAttribute("area", DM::Attribute::DOUBLE, DM::WRITE);

	building_model = DM::View("Geometry", DM::FACE, DM::WRITE);
	building_model.addAttribute("type", DM::Attribute::DOUBLE, DM::WRITE);
	building_model.addAttribute("color", DM::Attribute::DOUBLEVECTOR, DM::WRITE);
	building_model.addAttribute("parent", DM::Attribute::DOUBLE, DM::WRITE);

	parcels.addAttribute("BUILDING", houses.getName(), DM::WRITE);
	houses.addAttribute("PARCEL", parcels.getName(), DM::WRITE);
	houses.addAttribute("Geometry", building_model.getName(), DM::WRITE);
	houses.addAttribute("Footprint", footprint.getName(), DM::WRITE);

	footprint.addAttribute("BUILDING",  houses.getName(), DM::WRITE);

	std::vector<DM::View> data;
	data.push_back(houses);
	data.push_back(parcels);
	data.push_back(footprint);
	data.push_back(building_model);
	if (this->yearFromCity)
		data.push_back(cityView);

	this->addData("City", data);

}

string CreateHouses::getHelpUrl()
{
	return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/CreateHouses.md";
}
