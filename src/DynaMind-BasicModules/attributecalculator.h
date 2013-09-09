/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich

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

#ifndef ATTRIBUTECALCULATOR_H
#define ATTRIBUTECALCULATOR_H

#include <dmmodule.h>
#include <dm.h>
#include "parser/mpParser.h"

using namespace DM;
class DM_HELPER_DLL_EXPORT AttributeCalculator : public Module
{

	DM_DECLARE_NODE(AttributeCalculator)
public:
	std::map<std::string, std::string> variablesMap;
	bool asVector;
private:
	DM::System * sys_in;
	std::string nameOfBaseView;
	std::string nameOfNewAttribute;
	std::string equation;
	std::map<std::string, DM::View> viewsmap;
	std::vector<std::string> varaibleNames;
	void getLinkedAttribute(std::vector< mup::Value> * varaible_container, DM::Component *currentcmp,std::string name);

	mup::Value mp_counter;

public:
	AttributeCalculator();
	void run();
	void init();
	bool createInputDialog();
	QString IfElseConverter(QString exp);
	DM::System * getSystemIn() {return this->sys_in;}
	std::string getHelpUrl();

};

#endif // ATTRIBUTECALCULATOR_H
