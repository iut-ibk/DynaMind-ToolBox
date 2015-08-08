/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2015  Christian Urich

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

#ifndef DMSQLITECALCULATOR_H
#define DMSQLITECALCULATOR_H

#include <dmmodule.h>
#include <dm.h>

class DM_HELPER_DLL_EXPORT DM_SQliteCalculator: public DM::Module
{
	DM_DECLARE_NODE(DM_SQliteCalculator)
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

	bool oneToMany(DM::ViewContainer *lead, DM::ViewContainer *linked);


	std::multimap<DM::ViewContainer * , std::string> index_map; //index _id for faster serach
	DM::Attribute::AttributeType convertAttributeType(std::string type);

public:
	DM_SQliteCalculator();
	void init();
	void run();
	std::string getHelpUrl();
	void resetInit();
};

#endif // GDALATTRIBUTECALCULATOR_H
