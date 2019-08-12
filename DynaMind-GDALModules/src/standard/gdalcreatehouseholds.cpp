#include "gdalcreatehouseholds.h"
#include "ogrsf_frmts.h"
#include <sstream>
//#include <random>

//DM_DECLARE_NODE_NAME(GDALCreateHouseholds, GDALModules)

DM_DECLARE_CUSTOM_NODE_NAME(GDALCreateHouseholds, Sample Housholds From AUST Census Data, Urban Form)

GDALCreateHouseholds::GDALCreateHouseholds()
{
	//generator = std::default_random_engine();
	distribution = boost::random::uniform_real_distribution<double>(0.0,1.0);
	srand (time(NULL));

	GDALModule = true;

	link_id = "district_id";
	this->addParameter("link_id", DM::STRING, &this->link_id);


	census_year = 2011;
	this->addParameter("census_year", DM::INT, &this->census_year);

	education_names.push_back("cdf_secondary");
	education_names.push_back("cdf_other");
	education_names.push_back("cdf_technical");
	education_names.push_back("cdf_tertiary");

	hh_income_names.push_back("cdf_low");
	hh_income_names.push_back("cdf_medium");
	hh_income_names.push_back("cdf_high");


	education_names_p.push_back("secondary");
	education_names_p.push_back("other");
	education_names_p.push_back("technical");
	education_names_p.push_back("tertiary");

	hh_income_names_p.push_back("low");
	hh_income_names_p.push_back("medium");
	hh_income_names_p.push_back("high");





}

void GDALCreateHouseholds::init()
{

	hh_age_names.clear();

	if (census_year == 2011) {
		hh_age_names.push_back("cdf_age_20_24");
		hh_age_names.push_back("cdf_age_25_29");
		hh_age_names.push_back("cdf_age_30_34");
		hh_age_names.push_back("cdf_age_35_39");
		hh_age_names.push_back("cdf_age_40_44");
		hh_age_names.push_back("cdf_age_45_49");
		hh_age_names.push_back("cdf_age_50_54");
		hh_age_names.push_back("cdf_age_55_59");
		hh_age_names.push_back("cdf_age_60_64");
		hh_age_names.push_back("cdf_age_65_69");
		hh_age_names.push_back("cdf_age_70_74");
		hh_age_names.push_back("cdf_age_75_79");
		hh_age_names.push_back("cdf_age_80_84");
	}
	if (census_year == 2016) {
		hh_age_names.push_back("cdf_age_20_24");
		hh_age_names.push_back("cdf_age_25_34");
		hh_age_names.push_back("cdf_age_35_44");
		hh_age_names.push_back("cdf_age_45_54");
		hh_age_names.push_back("cdf_age_55_64");
		hh_age_names.push_back("cdf_age_65_74");
		hh_age_names.push_back("cdf_age_75_84");
	}



	foreach (std::string name, hh_age_names) {
		hh_age_names_p.push_back(name.replace(0, 8, "").replace(2,5,""));

	}

	std::string income_str = "hh_income";
	std::string	education_str = "education";
	std::string age_str = "hh_age";

	if (census_year == 2016) {
		income_str = "hh_income_2016";
		education_str = "education_2016";
		age_str = "hh_age_2016";
	}


	hh_income = DM::ViewContainer(income_str, DM::COMPONENT, DM::READ);
	hh_income.addAttribute(link_id, DM::Attribute::INT, DM::READ);
	hh_income.addAttribute("cdf_low", DM::Attribute::DOUBLE, DM::READ);
	hh_income.addAttribute("cdf_medium", DM::Attribute::DOUBLE, DM::READ);
	hh_income.addAttribute("cdf_high", DM::Attribute::DOUBLE, DM::READ);

	education = DM::ViewContainer(education_str, DM::COMPONENT, DM::READ);
	education.addAttribute(link_id, DM::Attribute::INT, DM::READ);
	education.addAttribute("cdf_tertiary", DM::Attribute::DOUBLE, DM::READ);
	education.addAttribute("cdf_technical", DM::Attribute::DOUBLE, DM::READ);
	education.addAttribute("cdf_secondary", DM::Attribute::DOUBLE, DM::READ);
	education.addAttribute("cdf_other", DM::Attribute::DOUBLE, DM::READ);

	hh_age = DM::ViewContainer(age_str, DM::COMPONENT, DM::READ);
	hh_age.addAttribute(link_id, DM::Attribute::INT, DM::READ);

	foreach (std::string name, hh_age_names) {
		hh_age.addAttribute(name, DM::Attribute::DOUBLE, DM::READ);

	}




	building = DM::ViewContainer("building", DM::FACE, DM::READ);
	building.addAttribute("residential_units", DM::Attribute::INT, DM::READ);
	building.addAttribute(link_id, DM::Attribute::INT, DM::READ);

	household = DM::ViewContainer("household", DM::NODE, DM::WRITE);
	household.addAttribute("building_id", DM::Attribute::INT, DM::WRITE);
	household.addAttribute("persons", DM::Attribute::INT, DM::WRITE);
	household.addAttribute("education", DM::Attribute::STRING, DM::WRITE);
	household.addAttribute("income", DM::Attribute::STRING, DM::WRITE);
	household.addAttribute("age", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> datastream;
	//datastream.push_back(&district);
	datastream.push_back(&building);
	datastream.push_back(&hh_income);
	datastream.push_back(&education);
	datastream.push_back(&household);
	datastream.push_back(&hh_age);

	this->registerViewContainers(datastream);

}

void GDALCreateHouseholds::run()
{
	foreach (std::string e, education_names) {
		education_v.push_back(0);
	}

	foreach (std::string e, hh_income_names) {
		hh_income_v.push_back(0);
	}

	foreach (std::string e, hh_age_names) {
		hh_age_name_v.push_back(0);
	}


	building.createIndex(link_id);
	hh_income.createIndex(link_id);
	hh_age.createIndex(link_id);
	education.createIndex(link_id);


	OGRFeature * b = 0;

	//Assign residential units to buildings

	//while (d = district.getNextFeature()) {
	int counter = 0;
	building.resetReading();
	while (b = building.getNextFeature()) {

		int b_id = b->GetFID();
		int district_id = b->GetFieldAsInteger(link_id.c_str());
		int households = b->GetFieldAsInteger("residential_units"); //d->GetFieldAsInteger("hh");
		//DM::Logger(DM::Standard) << households;

		std::stringstream filter;
		filter << "ogc_fid = " << district_id;
		//building.setAttributeFilter(filter.str());

		fill_cdf(district_id, link_id ,education, this->education_names, this->education_v);
		fill_cdf(district_id, link_id ,hh_income, this->hh_income_names, this->hh_income_v);
		fill_cdf(district_id, link_id ,hh_age, this->hh_age_names, this->hh_age_name_v);

		OGRGeometry * geo = b->GetGeometryRef();
		if (!geo)
			continue;
		OGRPoint pt;
		geo->Centroid(&pt);


		//int max_size = building_ids.size();
		for (int i = 0; i < households ; i++) {
			//DM::Logger(DM::Standard) << "create";

			//if (counter == max_size)
			//	counter = 0;
			OGRFeature * h = household.createFeature();
			h->SetField("building_id", b_id);
			h->SetField("persons",rand() % 4 + 1 );
			h->SetField("education", this->sampler(this->education_names_p, this->education_v).c_str());
			h->SetField("income", this->sampler(this->hh_income_names_p, this->hh_income_v).c_str());
			h->SetField("age", QString::fromStdString(this->sampler(this->hh_age_names_p, this->hh_age_name_v)).toInt() + rand() % 5);

			h->SetGeometry(&pt);
			counter++;
			//households--;
		}
	}
	DM::Logger(DM::Standard) << "Created " << counter << " households";
}



std::string GDALCreateHouseholds::sampler(std::vector<std::string> & names, std::vector<double> & devec)
{
	//Sampling between 0 and 1
	double number = distribution(generator);
	//DM::Logger(DM::Error) << number;
	int l = names.size()-1;
	//[0.1, 0.5, 0.8, 1.0]
	for (int i = 0; i < l; i++) {
		if (number >= devec[i] && number < devec[i+1]) {
			return names[i];
		}
	}
	return names[l];
}

void GDALCreateHouseholds::fill_cdf(int id, std::string filtername, DM::ViewContainer & container,
									std::vector<std::string> & names, std::vector<double> &return_vec)
{
	container.resetReading();
	std::stringstream ss_filter;
	ss_filter << filtername << " = " << id;
	container.setAttributeFilter(ss_filter.str());

	OGRFeature * f = 0;

	while (f = container.getNextFeature()) {
		int l = names.size();
		for (int i = 0; i < l; i++)
			return_vec[i] = f->GetFieldAsDouble(names[i].c_str());
	}
}


