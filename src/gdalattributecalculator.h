#ifndef GDALATTRIBUTECALCULATOR_H
#define GDALATTRIBUTECALCULATOR_H

#include <dmmodule.h>
#include <dm.h>

struct DM_HELPER_DLL_EXPORT AttributeValue {
	AttributeValue() : s_val(""), d_val(0){}
	std::string s_val;
	double d_val;
};

class DM_HELPER_DLL_EXPORT GDALAttributeCalculator: public DM::Module
{
	DM_DECLARE_NODE(GDALAttributeCalculator)
private:
	std::string attribute;

	std::map<std::string, std::string> variables;
	std::map<std::string, DM::Attribute::AttributeType> variable_types;

	std::string equation;

	string attributeType;

	bool init_failed;

	DM::ViewContainer * leading_view;

	std::map<std::string, DM::ViewContainer *> helper_views_name;

	std::string leading_attribute;

	bool initViews();

	void solve_variable(OGRFeature *feat, QStringList link_chain, std::vector<AttributeValue> & ress_vector, DM::Attribute::AttributeType attr_type);

	bool oneToMany(DM::ViewContainer *lead, DM::ViewContainer *linked);

	std::vector<OGRFeature *> resolveLink(OGRFeature * f, QString first, QString second);

	std::multimap<DM::ViewContainer * , std::string> index_map; //index _id for faster serach
	DM::Attribute::AttributeType convertAttributeType(std::string type);
public:
	GDALAttributeCalculator();
	void init();
	void run();
	void resetInit();
};

#endif // GDALATTRIBUTECALCULATOR_H
