#include "dm_sqlitecalculator.h"
#include <sstream>
#include <ogr_feature.h>
#include "parser/mpParser.h"
#include "userdefinedfunctions.h"
#include "dmgroup.h"
#include <sqlite3.h>

DM_DECLARE_CUSTOM_NODE_NAME(DM_SQliteCalculator, SQlite Query, Data Handling)

typedef std::map<std::string, std::string> varaible_map;
typedef std::map<std::string, DM::ViewContainer *> helper_map;

DM_SQliteCalculator::DM_SQliteCalculator()
{
	this->GDALModule = true;
	this->SQLExclusive = true;
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
	if (type == "DOUBLEVECTOR") {
		return DM::Attribute::DOUBLEVECTOR;
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

void execute_query(sqlite3 *db, const char *sql ) {
	char *zErrMsg = 0;

	int rc;
		rc = sqlite3_exec(db, sql , 0, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		DM::Logger(DM::Error) <<  "SQL error: " << zErrMsg;
		std::cout <<  "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
}


//void DM_SQliteCalculator::sqlite_backend()
//{




//}

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

std::string DM_SQliteCalculator::getSQLExtension()
{
	std::stringstream query_ss;
	#if defined(_WIN32) || defined(__CYGWIN__)
	query_ss << "SELECT load_extension('" << this->getSimulationConfig().getDefaultModulePath() + "/SqliteExtension/dm_sqlite_plugin" << "')";
	std::cout << query_ss.str() << std::endl;
	#else
	query_ss << "SELECT load_extension('" << this->getSimulationConfig().getDefaultModulePath() + "/SqliteExtension/libdm_sqlite_plugin" << "')";
	std::cout << query_ss.str() << std::endl;
	#endif
	return query_ss.str();
}

void DM_SQliteCalculator::run()
{
	if (this->init_failed) {
		DM::Logger(DM::Error) << "Attribute Calculator Init Failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		return;
	}

	sqlite3 *db;
	int rc = sqlite3_open(this->leading_view->getDBID().c_str(), &db);
	if( rc ){
		DM::Logger(DM::Error) << "Can't open database: " << sqlite3_errmsg(db);
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;
	}
	sqlite3_enable_load_extension(db,1);
	execute_query(db,"SELECT load_extension('mod_spatialite')");

	execute_query(db,getSQLExtension().c_str());

	std::stringstream query;


	//Create Index for faster lookup
	for (std::multimap<DM::ViewContainer *, std::string>::const_iterator it = this->index_map.begin(); it != this->index_map.end(); ++it) {
		DM::ViewContainer * v = it->first;
		std::stringstream index_drop;
		index_drop << "DROP INDEX IF EXISTS "<< it->second <<"_index";
		execute_query(db,index_drop.str().c_str());

		std::stringstream index;
		index << "CREATE INDEX "<< it->second <<"_index ON " << v->getName() << " (" << it->second << ")";
		execute_query(db,index.str().c_str());
	}

	query << this->equation;

	std::string filter = leading_view->get_attribute_filter_sql_string();

	if (!filter.empty())
		query << " WHERE " << filter;

	DM::Logger(DM::Standard) << query.str();

	execute_query(db,query.str().c_str());
	DM::Logger(DM::Standard) << "End Syncronise DB";
	sqlite3_close(db);


}

string DM_SQliteCalculator::getHelpUrl()
{
	return "/DynaMind-GDALModules/dm_sqlcalculator.html";
}


