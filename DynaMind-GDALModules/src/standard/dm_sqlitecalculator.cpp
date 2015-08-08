#include "dm_sqlitecalculator.h"
#include <sstream>
#include <ogr_feature.h>
#include "parser/mpParser.h"
#include "userdefinedfunctions.h"
#include "dmgroup.h"

DM_DECLARE_CUSTOM_NODE_NAME(DM_SQliteCalculator, SQlite Query, Data Handling)

typedef std::map<std::string, std::string> varaible_map;
typedef std::map<std::string, DM::ViewContainer *> helper_map;

DM_SQliteCalculator::DM_SQliteCalculator()
{
	this->GDALModule = true;
	this->init_failed = true;

	this->attribute = "";
	this->addParameter("attribute", DM::STRING, &this->attribute);
	this->equation = "";

	this->addParameter("attribute_type", DM::STRING, &this->attributeType);
	this->attributeType = "DOUBLE";

	this->addParameter("query", DM::STRING, &this->equation);
	this->variables = std::map<std::string, std::string>();
	this->addParameter("variables", DM::STRING_MAP, &this->variables);
	leading_view = 0;

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}
DM::Attribute::AttributeType DM_SQliteCalculator::convertAttributeType(std::string type) {
	if (type == "STRING") {
		return DM::Attribute::STRING;
	}
	if (type == "INT") {
		return DM::Attribute::INT;
	}
	return DM::Attribute::DOUBLE;
}



void DM_SQliteCalculator::resetInit()
{
	leading_view = 0;
	index_map.clear();
	variable_types.clear();

	for (helper_map::const_iterator it = helper_views_name.begin(); it != helper_views_name.end(); ++it) {
		DM::ViewContainer * v = it->second;
		delete v;
	}

	helper_views_name.clear();
}

bool DM_SQliteCalculator::initViews()
{
	resetInit();
	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];

	QString attr = QString::fromStdString(this->attribute);
	QStringList lattr = attr.split(".");
	if (lattr.size() != 2) {
		DM::Logger(DM::Error) << "Wrong attribute definition use VIEWNAME.AttributeName";
		return false;
	}
	this->leading_view = new DM::ViewContainer(lattr[0].toStdString(), DM::COMPONENT, DM::READ);
	leading_attribute = lattr[1].toStdString();
	this->leading_view->addAttribute(leading_attribute, convertAttributeType(this->attributeType), DM::WRITE);

	helper_views_name[lattr[0].toStdString()] = leading_view;
	//Register Variables
	for (varaible_map::const_iterator it = variables.begin(); it != variables.end(); ++it) {

		QString var = QString::fromStdString(it->second);
		//The latest is always the view that is needed
		QStringList l_var = var.split(".");

		//Check if View exists in stream
		if(inViews.find(l_var[l_var.size()-2].toStdString()) == inViews.end()) {
			DM::Logger(DM::Warning) << "View " << l_var[l_var.size()-2].toStdString() << " not found";
			return false;
		}
		DM::View viewInStream = inViews[l_var[l_var.size()-2].toStdString()];

		DM::ViewContainer * v;
		if (helper_views_name.find(l_var[l_var.size()-2].toStdString()) == helper_views_name.end()) {
			v = new DM::ViewContainer(l_var[l_var.size()-2].toStdString(), DM::COMPONENT, DM::READ);
			helper_views_name[l_var[l_var.size()-2].toStdString()] = v;
		}
		else
			v = helper_views_name[l_var[l_var.size()-2].toStdString()];

		//Check if View has attribute
		if (!viewInStream.hasAttribute(l_var[l_var.size()-1].toStdString())) {
			DM::Logger(DM::Warning) << "Attribute " << l_var[l_var.size()-1].toStdString() <<  " for view " << viewInStream.getName() << " not found";
			foreach (std::string a, viewInStream.getAllAttributes()) {
				DM::Logger(DM::Warning) << a;
			}

			return false;
		}

		DM::Attribute::AttributeType variable_type = viewInStream.getAttributeType(l_var[l_var.size()-1].toStdString());
		variable_types[it->first] = variable_type;
		DM::Logger(DM::Debug) << it->first << " " << (int) variable_type;
		if (!v->hasAttribute(l_var[l_var.size()-1].toStdString())) {
			v->addAttribute(l_var[l_var.size()-1].toStdString(), variable_type, DM::READ);
		}

		//Remove last element
		l_var.removeLast();

		//Create Links between var[end] and var[end-1]. Since direction is only in one direction link attribute is either on var[end] or var [end-1]
		while (l_var.size() > 1) {
			if (helper_views_name.find(l_var[l_var.size()-2].toStdString()) == helper_views_name.end()) {
				v = new DM::ViewContainer(l_var[l_var.size()-2].toStdString(), DM::COMPONENT, DM::READ);
				helper_views_name[l_var[l_var.size()-2].toStdString()] = v;
			}
			else
				v = helper_views_name[l_var[l_var.size()-2].toStdString()];

			oneToMany(v, helper_views_name[l_var[l_var.size()-1].toStdString()]);

			l_var.removeLast();
		}
	}
	return true;
}




bool DM_SQliteCalculator::oneToMany( DM::ViewContainer * lead,  DM::ViewContainer  *linked)
{

	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(lead->getName()) == inViews.end()){
		DM::Logger(DM::Debug) << "View " << lead->getName() << " not found in stream";
		return false;
	}
	if (inViews.find(linked->getName()) == inViews.end()){
		DM::Logger(DM::Debug) << "View " << linked->getName() << " not found in stream";
		return false;
	}
	std::stringstream link_id_onToMany;
	link_id_onToMany << lead->getName() << "_id"; //PARCEL

	if (inViews[linked->getName()].hasAttribute(link_id_onToMany.str())) {
		linked->addAttribute(link_id_onToMany.str(), lead->getName() , DM::READ);
		//Mark to create index for faster search
		this->index_map.insert(std::pair<DM::ViewContainer * , std::string>(linked, link_id_onToMany.str()));
		return true;
	}

	std::stringstream link_id_many_to_one; //PARCEL
	link_id_many_to_one << linked->getName() << "_id";

	if (inViews[lead->getName()].hasAttribute(link_id_many_to_one.str())) {
		lead->addAttribute(link_id_many_to_one.str(), lead->getName() , DM::READ);
		return true;
	}

	return false;

}

void DM_SQliteCalculator::init()
{
	init_failed = true;
	if (this->attribute.empty())
		return;
	if (!initViews()) {

		resetInit();
		return;
	}
	init_failed = false;
	std::vector<DM::ViewContainer * > data_stream;
	for (helper_map::const_iterator it = helper_views_name.begin(); it != helper_views_name.end(); ++it) {
		DM::ViewContainer * v = it->second;
		data_stream.push_back(v);
	}
	this->registerViewContainers(data_stream);
}

void DM_SQliteCalculator::run()
{
	if (this->init_failed) {
		DM::Logger(DM::Error) << "Attribute Calculator Init Failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		return;
	}
	//Create Index for faster lookup
	for (std::multimap<DM::ViewContainer *, std::string>::const_iterator it = this->index_map.begin(); it != this->index_map.end(); ++it) {
		DM::ViewContainer * v = it->first;
		v->createIndex(it->second);
	}

	this->leading_view->executeSQL(equation.c_str());

}

string DM_SQliteCalculator::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalattributecalculator.html";
}


