/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2014  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef GDALATTRIBUTECALCULATOR_H
#define GDALATTRIBUTECALCULATOR_H

#include <dmmodule.h>
#include <dm.h>

struct DM_HELPER_DLL_EXPORT AttributeValue {
	AttributeValue() : s_val(""), d_val(0){}
	std::string s_val;
	double d_val;
	std::vector<double> v_val;
};

namespace mup {
	class Value;
}

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

	std::vector<OGRFeature *> gc_global_features;

	std::map<std::string, DM::ViewContainer *> helper_views_name;

	std::string leading_attribute;

	bool initViews();

	void solve_variable(OGRFeature *feat, QStringList link_chain, std::vector<AttributeValue> & ress_vector, DM::Attribute::AttributeType attr_type, bool is_first = true);

	bool oneToMany(DM::ViewContainer *lead, DM::ViewContainer *linked);

	std::vector<OGRFeature *> resolveLink(OGRFeature * f, QString first, QString second);

	std::multimap<DM::ViewContainer * , std::string> index_map; //index _id for faster serach
	DM::Attribute::AttributeType convertAttributeType(std::string type);

	std::map<std::string, OGRFeature *> global_features;
public:
	GDALAttributeCalculator();
	void init();
	void run();
	std::string getHelpUrl();
	void resetInit();
	void updateDoubleVector(std::string variable_name, std::vector<AttributeValue> & ressult_vec, std::map<std::string, mup::Value * > & muVariables);
};

#endif // GDALATTRIBUTECALCULATOR_H
