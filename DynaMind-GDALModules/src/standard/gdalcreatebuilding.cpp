#include "gdalcreatebuilding.h"

#include <ogr_api.h>
#include <ogrsf_frmts.h>


typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>              Pwh_list_2;

DM_DECLARE_CUSTOM_NODE_NAME(GDALCreateBuilding,Generate Simple Buildings, Urban Form)

OGRGeometry* GDALCreateBuilding::createBuilding(OGRPolygon *ogr_poly)
{


	char* geo;
	ogr_poly->exportToWkt(&geo);

	double width = this->width;
	double height = this->height;

	std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(geo));
	OGRFree(geo); //Not needed after here

	SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();

	if (!poly.toPolygon_2(false).is_simple()) {
		DM::Logger(DM::Warning) << "Polygon is not simple";
		return NULL;
	}
	if (poly.toPolygon_2(false).area() < 0.001 && poly.toPolygon_2(false).area() > -0.001){
		DM::Logger(DM::Warning) << "Not a polygon " << CGAL::to_double(poly.toPolygon_2(false).area());
		return NULL;
	}

	//Transfer to GDAL polygon
	Polygon_with_holes_2 p = poly.toPolygon_with_holes_2(true);

	Polygon_2 p_c;
	CGAL::convex_hull_2(p.outer_boundary().vertices_begin(), p.outer_boundary().vertices_end(), std::back_inserter(p_c));

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));


	Point_2 p1 = p_m.vertex(0);
	Point_2 p2 = p_m.vertex(1);

	Point_2 p3 = p_m.vertex(2);
	Point_2 p4 = p_m.vertex(3);

	Vector_2 v1 = (p2-p1);
	Vector_2 v2 = (p3-p2);

	bool v1_bigger = true;


	if (v1.squared_length() < v2.squared_length()) {
		v1_bigger = false;
	}



	Vector_2 e1 = v1 / sqrt(CGAL::to_double(v1.squared_length()));
	Vector_2 e2 = v2 / sqrt(CGAL::to_double(v2.squared_length()));



	OGRPoint ct;
	ogr_poly->Centroid(&ct);

	Point_2 centre(ct.getX(), ct.getY());

	double w = (!v1_bigger) ? this->width/2. : this->height/2.;
	double h = (!v1_bigger) ? this->height/2. : this->width/2.;


	if (this->site_coverage > 0.01 ) {
		double area = CGAL::to_double(poly.toPolygon_2(false).area());
		width =  sqrt(CGAL::to_double(v2.squared_length()));
		height = sqrt(CGAL::to_double(v1.squared_length()));
		if (v1_bigger) {
			h = (width > 13) ? 6  : width/2 - 1;
			w = area / (h*2)*this->site_coverage / 2;
			//Correct if height is to big
			if(w > height/2){
				w = height/2 - 1;
				h = area / (w*2)*this->site_coverage / 2;
			}

		}else {
			w = (height > 13) ? 6  : height/2 - 1;
			h = area / (w*2)*this->site_coverage / 2;
			if (h > width/2) {
				h =  width/2 -1;
				w = area / (h*2)*this->site_coverage / 2;
			}


		}
	}
	//DM::Logger(DM::Standard) << (int)v1_bigger  << sqrt(CGAL::to_double(v1.squared_length())) << "/"<<sqrt(CGAL::to_double(v2.squared_length()))  << "/"<< w << "/"<< h << "/" << sqrt(CGAL::to_double(e1.squared_length()));
	Polygon_with_holes_2 footprint;

	//DM::Logger(DM::Standard) << (int)v1_bigger  << sqrt(CGAL::to_double(c1.squared_length()));

	footprint.outer_boundary().push_back( centre - e1*w - e2*h );
	footprint.outer_boundary().push_back( centre + e1*w - e2*h );
	footprint.outer_boundary().push_back( centre + e1*w + e2*h );
	footprint.outer_boundary().push_back( centre - e1*w + e2*h );
	//footprint.outer_boundary().push_back( centre - e1*w - e2*h );

	SFCGAL::Polygon f(footprint);

	return addToSystem(f);


}

OGRGeometry *  GDALCreateBuilding::addToSystem(SFCGAL::Polygon & poly)
{

	std::string wkt = poly.asText(9).c_str();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memeory hole?
	std::copy(wkt.begin(), wkt.end(), writable_wr);
	writable_wr[wkt.size()] = '\0';

	OGRGeometry * ogr_poly;

	OGRErr err = OGRGeometryFactory::createFromWkt(&writable_wr, 0, &ogr_poly);

	if (!ogr_poly->IsValid()) {
		DM::Logger(DM::Warning) << "Geometry is not valid!";
		return 0;
	}
	if (ogr_poly->IsEmpty()) {
		DM::Logger(DM::Warning) << "Geometry is empty ";
		DM::Logger(DM::Warning) << "OGR Error " << err;
		DM::Logger(DM::Warning) << poly.asText(9);
		return 0;
	}

	return ogr_poly;
}

GDALCreateBuilding::GDALCreateBuilding()
{
	GDALModule = true;

	this->width = 10;
	this->addParameter("width", DM::DOUBLE, &this->width);

	this->height = 10;
	this->addParameter("length", DM::DOUBLE, &this->height);

	this->building_height = 6;
	this->addParameter("height", DM::DOUBLE, &this->building_height);

	this->site_coverage = 0.0;
	this->addParameter("site_coverage", DM::DOUBLE, &this->site_coverage);

	this->residential_units = 1;
	this->addParameter("residential_units", DM::INT, &this->residential_units);

    this->paramter_from_linked_view = "";
    this->addParameter("paramter_from_linked_view", DM::STRING, &this->paramter_from_linked_view);

	parcel = DM::ViewContainer("parcel", DM::FACE, DM::READ);
	building = DM::ViewContainer("building", DM::FACE, DM::WRITE);
	building.addAttribute("residential_units", DM::Attribute::INT, DM::WRITE);
	building.addAttribute("height", DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&parcel);
	data_stream.push_back(&building);

	this->registerViewContainers(data_stream);
}

void GDALCreateBuilding::init()
{


    std::vector<DM::ViewContainer*> views;
    views.push_back(&parcel);
    views.push_back(&building);

    if (!this->paramter_from_linked_view.empty()) {
        this->linked_view = DM::ViewContainer(this->paramter_from_linked_view, DM::COMPONENT, DM::READ);
        this->linked_view.addAttribute("residential_units", DM::Attribute::DOUBLE, DM::READ);
        this->linked_view.addAttribute("building_height", DM::Attribute::DOUBLE, DM::READ);

        std::stringstream ss_link_view_id;
        ss_link_view_id << this->paramter_from_linked_view << "_id";
        this->link_view_id = ss_link_view_id.str();

        views.push_back(&linked_view);
        parcel.addAttribute(link_view_id, DM::Attribute::LINK, DM::READ);


    }

    this->registerViewContainers(views);
}

void GDALCreateBuilding::run()
{

    std::map<int, std::map<std::string, double> > templates;
    OGRFeature *poFeature;
    if (!this->paramter_from_linked_view.empty()) {
        this->linked_view.resetReading();
        while ( (poFeature = linked_view.getNextFeature()) != NULL ) {
            std::map<std::string, double> params;
            params["residential_units"] = poFeature->GetFieldAsDouble("residential_units");
            params["building_height"] = poFeature->GetFieldAsDouble("building_height");
            templates[poFeature->GetFID()] = params;
        }
    }

	parcel.resetReading();

	OGRFeature * f;

	while ((f = parcel.getNextFeature())) {

        double ru = this->residential_units;
        double bh= this->building_height;

        int link_id = 0;

        if (!this->paramter_from_linked_view.empty()) {
            link_id = f->GetFieldAsInteger(this->link_view_id.c_str());

            if (link_id == 0) // If field returns zero the no template has been set
                continue;

            //Check if link ID exists
            std::map<int, std::map<std::string, double> >::const_iterator it = templates.find(link_id);
            if (it == templates.end()) {
                DM::Logger(DM::Warning) << "Template " << link_id << "not found" << "for " << (int) f->GetFID();
                continue;
            }

            ru = templates[link_id]["residential_units"];
            bh = templates[link_id]["building_height"];
        }

		OGRPolygon * geo = (OGRPolygon *)f->GetGeometryRef();
		if (!geo)
			continue;
        OGRPolygon * building_geo  = (OGRPolygon *) this->createBuilding(geo);

		if (!building_geo)
			continue;
		//Create Feature
		OGRFeature * b = building.createFeature();
		b->SetGeometry(building_geo);

		//Cacluate RU
		if (ru < 0) {
            double area =  building_geo->get_Area();
            ru = (int) (area * (int) (building_height) / 3) / 125;

		}

		b->SetField("residential_units", ru);
		b->SetField("height", bh);

		OGRGeometryFactory::destroyGeometry(building_geo);
	}
}

string GDALCreateBuilding::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalcreatebuilding.html";
}
