#include "gdaladdcomponent.h"
#include "ogrsf_frmts.h"
#include "dmgdalsystem.h"
#include "dmgdalhelper.h"

DM_DECLARE_NODE_NAME(GDALAddComponent, Modules)

GDALAddComponent::GDALAddComponent()
{
	GDALModule = true;
	elements = 100000;
	this->addParameter("elements", DM::INT, &elements);
	append = false;
	this->addParameter("append", DM::BOOL, &append);

	//Add Attributes for test
	addAttributes = false;
	this->addParameter("add_attributes", DM::BOOL, &addAttributes);
}

void GDALAddComponent::init()
{
	if (!append) {
		components = DM::ViewContainer("component", DM::NODE, DM::WRITE);
	}
	else {
		components = DM::ViewContainer("component", DM::NODE, DM::MODIFY);
	}
	//components.addAttribute("persons",DM::Attribute::STRING, DM::WRITE);

	if (addAttributes) {
			components.addAttribute("string_attribute",DM::Attribute::STRING, DM::WRITE);
			components.addAttribute("int_attribute",DM::Attribute::INT, DM::WRITE);
			components.addAttribute("double_attribute",DM::Attribute::DOUBLE, DM::WRITE);
			components.addAttribute("double_vector_attribute",DM::Attribute::DOUBLEVECTOR, DM::WRITE);
			components.addAttribute("string_vector_attribute",DM::Attribute::STRINGVECTOR, DM::WRITE);
	}

	std::vector<DM::ViewContainer> datastream;
	datastream.push_back(components);
	this->addGDALData("city", datastream);
}


void GDALAddComponent::run()
{
	DM::GDALSystem * sys = this->getGDALData("city");
	sys->updateView(components);

	this->components.setCurrentGDALSystem(sys);

	int counter = 0;
	for (int i = 0; i < elements; i++) {
		OGRFeature * f = this->components.createFeature();

		if (addAttributes) {
			std::vector<double> double_vector;
			f->SetField("double_attribute", 10.5);
			double_vector.push_back(10.5);
			double_vector.push_back(20.5);
			double_vector.push_back(30.5);
			DM::DMFeature::SetDoubleList( f, "double_vector_attribute", double_vector);

		}
		//f->SetField("persons", "that is me");
		OGRPoint pt;
		pt.setX( i );
		pt.setY( i );

		f->SetGeometry(&pt);
		if (counter == 100000) {
			components.syncAlteredFeatures();
			counter = 0;
		}
		counter++;
	}
	this->components.syncAlteredFeatures();


}
