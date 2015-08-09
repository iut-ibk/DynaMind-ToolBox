#include "gdalattributecalculator.h"
#include <sstream>
#include <ogr_feature.h>
#include "parser/mpParser.h"
#include "userdefinedfunctions.h"
#include "dmgroup.h"

DM_DECLARE_CUSTOM_NODE_NAME(GDALAttributeCalculator, Calculate Attributes, Data Handling)

typedef std::map<std::string, std::string> varaible_map;
typedef std::map<std::string, DM::ViewContainer *> helper_map;

GDALAttributeCalculator::GDALAttributeCalculator()
{
	this->GDALModule = true;
	this->init_failed = true;

	this->attribute = "";
	this->addParameter("attribute", DM::STRING, &this->attribute);
	this->equation = "";

	this->addParameter("attribute_type", DM::STRING, &this->attributeType);
	this->attributeType = "DOUBLE";

	this->addParameter("equation", DM::STRING, &this->equation);
	this->variables = std::map<std::string, std::string>();
	this->addParameter("variables", DM::STRING_MAP, &this->variables);
	leading_view = 0;

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}
DM::Attribute::AttributeType GDALAttributeCalculator::convertAttributeType(std::string type) {
	if (type == "STRING") {
		return DM::Attribute::STRING;
	}
	if (type == "INT") {
		return DM::Attribute::INT;
	}
	return DM::Attribute::DOUBLE;
}



void GDALAttributeCalculator::resetInit()
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

bool GDALAttributeCalculator::initViews()
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




bool GDALAttributeCalculator::oneToMany( DM::ViewContainer * lead,  DM::ViewContainer  *linked)
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
		while(next_f = v->getNextFeature()) {
			next_features.push_back(next_f);
		}
		return next_features;
	}

	std::stringstream link_id_many_to_one; //PARCEL
	link_id_many_to_one << second.toStdString() << "_id";

	DM::ViewContainer * v = helper_views_name[second.toStdString()];
	OGRFeature * next_f = v->getFeature(f->GetFieldAsInteger(link_id_many_to_one.str().c_str()));
	if (next_f)
		next_features.push_back(next_f);
	return next_features;
}

void GDALAttributeCalculator::init()
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

void GDALAttributeCalculator::run()
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

	// Reset global features
	global_features.clear();


	DM::Attribute::AttributeType result_type = leading_view->getAttributeType(leading_attribute);

	leading_view->resetReading();

	OGRFeature * l_feat = 0;


	std::map<std::string, mup::Value * > muVariables;
	mup::ParserX * p  = new mup::ParserX();
	mup::Value mp_c;

	p->DefineFun(new dm::Random);
	p->DefineFun(new dm::Round);
	p->DefineFun(new dm::Num2Str);
	p->DefineFun(new dm::Str2Num);
	p->DefineVar("counter", &mp_c);

	for (varaible_map::const_iterator it = variables.begin(); it != variables.end(); ++it) {
		DM::Attribute::AttributeType attr_type = variable_types[it->first];
		mup::Value * v = 0;
		switch (attr_type) {
		case DM::Attribute::STRING:
			v = new mup::Value("");
			DM::Logger(DM::Debug) <<  it->first << " " << attr_type << " Init " << it->first << " as string";
			break;
		case DM::Attribute::LINK:
		case DM::Attribute::INT:
			v = new mup::Value(0);
			DM::Logger(DM::Debug) << "Init " << it->first << " as int";
			break;
		default:
			v = new mup::Value(0.0);
			DM::Logger(DM::Debug) << "Init " << it->first << " as double";
			break;
		}
		muVariables[it->first] = v;
		p->DefineVar(it->first, v);
	}
	p->SetExpr(this->equation);

	int counter = 0;
	while (l_feat = leading_view->getNextFeature()) {
		counter++;

		//set mpc
		DM::Group* lg = dynamic_cast<DM::Group*>(getOwner());
		if(lg) {
			mp_c = lg->getGroupCounter();
		}
		else {
			//DM::Logger(DM::Debug) << "attribute calc: counter not found";
			mp_c = 0;
		}

		//Update Varaibles
		for (varaible_map::const_iterator it = variables.begin(); it != variables.end(); ++it) {
			QString var = QString::fromStdString(it->second);
			//The latest is always the view that is needed
			QStringList l_var = var.split(".");

			std::vector<AttributeValue> ressult_vec;
			DM::Attribute::AttributeType attr_type = this->variable_types[it->first];
			solve_variable(l_feat, l_var,ressult_vec, attr_type );
			double d_val = 0;
			std::string s_val = "";
			switch (attr_type) {
			case DM::Attribute::DOUBLE:
				d_val = 0;
				foreach(AttributeValue v, ressult_vec) {
					d_val+=v.d_val;
				}
				*muVariables[it->first] = mup::Value(d_val);
				break;
			case DM::Attribute::LINK:
			case DM::Attribute::INT:
				d_val = 0;
				foreach(AttributeValue v, ressult_vec) {
					d_val+=v.d_val;
				}
				*muVariables[it->first] = (int) mup::Value(d_val);
				break;
			case DM::Attribute::STRING:
				s_val = "";
				if (ressult_vec.size() > 0) {
					s_val = ressult_vec[0].s_val;
				}
				*muVariables[it->first] = mup::Value(s_val);
				break;
			default:
				*muVariables[it->first] = mup::Value(d_val);
				break;
			}
		}
		try
		{
			mup::Value val = p->Eval();
			if (result_type == DM::Attribute::INT) {
				l_feat->SetField(leading_attribute.c_str(), (int) val.GetFloat());
			}
			else if (result_type == DM::Attribute::STRING) {
				l_feat->SetField(leading_attribute.c_str(), val.GetString().c_str());
			} else {
				DM::Logger(DM::Debug) << val.GetFloat();
				l_feat->SetField(leading_attribute.c_str(), val.GetFloat());
			}

			/*if (counter%100000 == 0){
				//DM::Logger(DM::Error) << "Sync Altered";
				leading_view->syncAlteredFeatures();
				for (std::map<std::string, DM::ViewContainer *>::const_iterator it = helper_views_name.begin(); it != helper_views_name.end(); ++it ) {
					it->second->syncAlteredFeatures();
					it->second->syncReadFeatures();
				}
			}*/
		}


		catch (mup::ParserError &e)
		{
			DM::Logger(DM::Error) << "Error in equation "<< e.GetMsg();
			this->setStatus(DM::MOD_CHECK_ERROR);
			return;
		}
	}
}

string GDALAttributeCalculator::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalattributecalculator.html";
}


void GDALAttributeCalculator::solve_variable(OGRFeature *feat, QStringList link_chain, std::vector<AttributeValue> & ress_vector, DM::Attribute::AttributeType attr_type, bool is_first)
{
	//Check if last in chain
	//solve forward
	if (link_chain.size() > 2) {
		//Link Name
		std::vector<OGRFeature*> features = resolveLink(feat, link_chain[0], link_chain[1]);

		QStringList link_chain_next = link_chain;
		link_chain_next.removeFirst();

		foreach (OGRFeature * f, features) {
			solve_variable(f, link_chain_next, ress_vector, attr_type, false);
		}
	} else if (link_chain.size() == 2) { // Get Values
		DM::Logger(DM::Debug) << link_chain[0].toStdString();
		DM::ViewContainer * v = helper_views_name[link_chain[0].toStdString()];
		DM::Attribute::AttributeType attr_type = v->getAttributeType(link_chain[1].toStdString());
		AttributeValue val;

		if (is_first && link_chain[0].toStdString() != this->leading_view->getName()) {
			if (global_features.find(link_chain[0].toStdString()) == global_features.end()) { // Set global Feature
				DM::Logger(DM::Debug) << "global variable " << link_chain[0].toStdString() << " " << this->leading_view->getName();
				v->resetReading();
				while (feat = v->getNextFeature()) { // Assume first feature is the right one
					DM::Logger(DM::Debug) << "set variable";
					global_features[link_chain[0].toStdString()] = feat;
					break;
				}
			}
			feat = global_features[link_chain[0].toStdString()];
		}

		if (attr_type == DM::Attribute::STRING) {
			val.s_val = feat->GetFieldAsString(link_chain[1].toStdString().c_str());
			ress_vector.push_back(val);
			return;
		}
		val.d_val = feat->GetFieldAsDouble(link_chain[1].toStdString().c_str());
		ress_vector.push_back(val);
		return;
	} else {
		DM::Logger(DM::Error) << "something is wrong with the varaible definitaion";
		return;
	}

}
