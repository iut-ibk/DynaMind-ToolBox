#include "gdalattributecalculator.h"
#include <sstream>
#include <ogr_feature.h>
#include "parser/mpParser.h"

DM_DECLARE_NODE_NAME(GDALAttributeCalculator, GDALModules)

typedef std::map<std::string, std::string> varaible_map;
typedef std::map<std::string, DM::ViewContainer *> helper_map;

GDALAttributeCalculator::GDALAttributeCalculator()
{
	this->GDALModule = true;

	this->attribute = "";
	this->addParameter("attribute", DM::STRING, &this->attribute);
	this->equation = "";
	this->addParameter("equation", DM::STRING, &this->equation);
	this->variables = std::map<std::string, std::string>();
	this->addParameter("variables", DM::STRING_MAP, &this->variables);
	leading_view = 0;

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALAttributeCalculator::initViews()
{
	leading_view = 0;
	index_map.clear();

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

	helper_views_name[lattr[0].toStdString()] = leading_view;
	//Register Variables
	for (varaible_map::const_iterator it = variables.begin(); it != variables.end(); ++it) {

		QString var = QString::fromStdString(it->second);
		//The latest is always the view that is needed
		QStringList l_var = var.split(".");

		DM::ViewContainer * v;
		if (helper_views_name.find(l_var[l_var.size()-2].toStdString()) == helper_views_name.end()) {
			v = new DM::ViewContainer(l_var[l_var.size()-2].toStdString(), DM::COMPONENT, DM::READ);
			helper_views_name[l_var[l_var.size()-2].toStdString()] = v;
		}
		else
			v = helper_views_name[l_var[l_var.size()-2].toStdString()];
		if (!v->hasAttribute(l_var[l_var.size()-1].toStdString()))
			v->addAttribute(l_var[l_var.size()-1].toStdString(), DM::Attribute::DOUBLE, DM::READ);

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
}




bool GDALAttributeCalculator::oneToMany( DM::ViewContainer * lead,  DM::ViewContainer  *linked)
{

	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(lead->getName()) == inViews.end()){
		DM::Logger(DM::Error) << "View " << lead->getName() << " not found in stream";
		return false;
	}
	if (inViews.find(linked->getName()) == inViews.end()){
		DM::Logger(DM::Error) << "View " << linked->getName() << " not found in stream";
		return false;
	}
	std::stringstream link_id_onToMany;
	link_id_onToMany << lead->getName() << "_id"; //PARCEL

	if (inViews[linked->getName()].hasAttribute(link_id_onToMany.str())) {
		linked->addAttribute(link_id_onToMany.str(), DM::Attribute::INT, DM::READ);
		//Mark to create index for faster search
		this->index_map.insert(std::pair<DM::ViewContainer * , std::string>(linked, link_id_onToMany.str()));
		return true;
	}

	std::stringstream link_id_many_to_one; //PARCEL
	link_id_many_to_one << linked->getName() << "_id";

	if (inViews[lead->getName()].hasAttribute(link_id_many_to_one.str())) {
		lead->addAttribute(link_id_many_to_one.str(), DM::Attribute::INT, DM::READ);
		return true;
	}

	return false;

}

//CITYBLOCK.PARCEL
std::vector<OGRFeature *> GDALAttributeCalculator::resolveLink(OGRFeature * f, QString first, QString second)
{
	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	std::stringstream link_id_onToMany;
	link_id_onToMany << first.toStdString() << "_id"; //PARCEL
	std::vector<OGRFeature *> next_features;

	if (inViews[second.toStdString()].hasAttribute(link_id_onToMany.str())) {
		OGRFeature * next_f = 0;
		long search_id = f->GetFID();
		DM::ViewContainer * v = helper_views_name[second.toStdString()];
		std::stringstream filter;
		filter << link_id_onToMany.str() << " = " << search_id;
		v->resetReading();
		v->setAttributeFilter(filter.str());
		DM::Logger(DM::Debug) << filter.str();
		while(next_f = v->getNextFeature()) {
			next_features.push_back(next_f);
		}
		return next_features;
	}

	std::stringstream link_id_many_to_one; //PARCEL
	link_id_many_to_one << second.toStdString() << "_id";

	DM::ViewContainer * v = helper_views_name[second.toStdString()];
	OGRFeature * next_f = v->getFeature(f->GetFID());
	if (next_f)
		next_features.push_back(next_f);
	return next_features;
}


void GDALAttributeCalculator::init()
{
	if (this->attribute.empty())
		return;
	initViews();

	std::vector<DM::ViewContainer * > data_stream;
	for (helper_map::const_iterator it = helper_views_name.begin(); it != helper_views_name.end(); ++it) {
		DM::ViewContainer * v = it->second;
		data_stream.push_back(v);
	}
	this->registerViewContainers(data_stream);
}

void GDALAttributeCalculator::run()
{
	for (std::multimap<DM::ViewContainer *, std::string>::const_iterator it = this->index_map.begin(); it != this->index_map.end(); ++it) {
		DM::ViewContainer * v = it->first;
//		if (v == leading_view)
//			continue;
		v->createIndex(it->second);
	}

	leading_view->resetReading();

	OGRFeature * l_feat = 0;


	std::map<std::string, mup::Value * > muVariables;
	mup::ParserX * p  = new mup::ParserX();

	for (varaible_map::const_iterator it = variables.begin(); it != variables.end(); ++it) {
		mup::Value * v = new mup::Value(0.0);
		muVariables[it->first] = v;
		p->DefineVar(it->first, v);
	}
	p->SetExpr(this->equation);

	while (l_feat = leading_view->getNextFeature()) {
		//Update Varaibles
		for (varaible_map::const_iterator it = variables.begin(); it != variables.end(); ++it) {

			QString var = QString::fromStdString(it->second);
			//The latest is always the view that is needed
			QStringList l_var = var.split(".");

			double value = solve_variable(l_feat, l_var);
			*muVariables[it->first] = mup::Value(value);
		}
		try
		{
			mup::Value val = p->Eval();
			l_feat->SetField(leading_attribute.c_str(), val.GetFloat());
		}
		catch (mup::ParserError &e)
		{
			DM::Logger(DM::Error) << "Error in qeuation "<< e.GetMsg();
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

		std::vector<OGRFeature*> features = resolveLink(feat, link_chain[0], link_chain[1]);

		QStringList link_chain_next = link_chain;
		link_chain_next.removeFirst();

		foreach (OGRFeature * f, features) {
			val+=solve_variable(f, link_chain_next);
		}
	} else {
		val = feat->GetFieldAsDouble(link_chain[1].toStdString().c_str());
	}

	return val;
}
