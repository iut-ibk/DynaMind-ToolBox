#include "gdalattributecalculator.h"
#include <sstream>
#include <ogr_feature.h>

DM_DECLARE_NODE_NAME(GDALAttributeCalculator, GDALModules)

typedef std::map<std::string, std::string> varaible_map;
typedef std::map<std::string, DM::ViewContainer *> helper_map;
void GDALAttributeCalculator::initViews()
{
	this->GDALModule = true;
	if (leading_view)
		delete leading_view;
	leading_view = 0;

	for (helper_map::const_iterator it = helper_views_name.begin(); it != helper_views_name.end(); ++it) {
		DM::ViewContainer * v = it->second;
		delete v;
	}
	helper_views_name.clear();


	QString attr = QString::fromStdString(this->attribute);
	QStringList lattr = attr.split(".");

	this->leading_view = new DM::ViewContainer(lattr[0].toStdString(), DM::COMPONENT, DM::READ);
	leading_attribute = lattr[1].toStdString();
	this->leading_view->addAttribute(leading_attribute, DM::Attribute::DOUBLE, DM::WRITE);

	for (varaible_map::const_iterator it = variables.begin(); it != variables.end(); ++it) {
		std::string var_name = it->first;
		QString var = QString::fromStdString(it->second);
		//The latest is always the view that is needed
		QStringList l_var = var.split(".");

		DM::ViewContainer * v;
		if (helper_views_name.find(l_var[l_var.size()-2].toStdString()) != helper_views_name.end()) {
			v = new DM::ViewContainer(l_var[l_var.size()-2].toStdString(), DM::COMPONENT, DM::READ);
			helper_views_name[l_var[l_var.size()-2].toStdString()] = v;
		}
		else
			v = helper_views_name[l_var[l_var.size()-2].toStdString()];

		v->addAttribute(l_var[l_var.size()-1].toStdString(), DM::Attribute::DOUBLE, DM::READ);

		//Remove last element
		l_var.removeLast();

		//Create Links
		while (l_var.size() > 1) {
			if (helper_views_name.find(l_var[l_var.size()-2].toStdString()) != helper_views_name.end()) {
				v = new DM::ViewContainer(l_var[l_var.size()-2].toStdString(), DM::COMPONENT, DM::READ);
				helper_views_name[l_var[l_var.size()-2].toStdString()] = v;
			}
			else
				v = helper_views_name[l_var[l_var.size()-2].toStdString()];

			std::stringstream linkname;
			linkname << l_var[l_var.size()-1].toStdString() << "_id";
			v->addAttribute(linkname.str() , DM::Attribute::INT, DM::READ);

			l_var.removeLast();
		}
	}
}


double GDALAttributeCalculator::solve_variable(OGRFeature *feat, QStringList link_chain)
{
	double val = 0;
	//Check if last in chain
	//solve forward
	if (link_chain.size() > 2) {
		//Link Name
		std::stringstream linkname;
		linkname << link_chain[0].toStdString() << "_id";

		DM::ViewContainer * vc_next = helper_views_name[link_chain[0].toStdString()];


	}

	return val;
}

GDALAttributeCalculator::GDALAttributeCalculator()
{
	this->attribute = "";
	this->addParameter("attribute", DM::STRING, &this->attribute);
	this->equation = "";
	this->addParameter("equation", DM::STRING, &this->equation);
	this->variables = std::map<std::string, std::string>();
	this->addParameter("varaibles", DM::STRING_MAP, &this->variables);
	leading_view = 0;
}

void GDALAttributeCalculator::init()
{
	initViews();

	std::vector<DM::ViewContainer *> data_stream;
	data_stream.push_back(leading_view);

	for (helper_map::const_iterator it = helper_views_name.begin(); it != helper_views_name.end(); ++it) {
		DM::ViewContainer * v = it->second;
		data_stream.push_back(v);
	}
	this->registerViewContainers(data_stream);
}



void GDALAttributeCalculator::run()
{
	leading_view->resetReading();

	OGRFeature * l_feat = 0;
	while (l_feat = leading_view->getNextFeature()) {

		double result;

		l_feat->SetField(leading_attribute.c_str(), result);
	}
}
