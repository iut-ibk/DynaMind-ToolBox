/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012-2013  Christian Urich
 
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

#include "attributecalculator.h"
#include "guiattributecalculator.h"
#include "userdefinedfunctions.h"
#include "loopgroup.h"

DM_DECLARE_NODE_NAME(AttributeCalculator, Modules)

AttributeCalculator::AttributeCalculator()
{

    this->nameOfBaseView = "";
    this->addParameter("NameOfBaseView", DM::STRING, & this->nameOfBaseView);
    this->variablesMap = std::map<std::string, std::string>();
    this->addParameter("variablesMap", DM::STRING_MAP, & this->variablesMap);
    this->nameOfNewAttribute = "";
    this->addParameter("nameOfNewAttribute", DM::STRING, & this->nameOfNewAttribute);
    this->addParameter("equation", DM::STRING, & this->equation);
    this->asVector = false;
    this->addParameter("asVector", DM::BOOL, & this->asVector);

    sys_in = 0;
    mp_counter = 0;
    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addData("Data", data);
}

void AttributeCalculator::init() {
    if (nameOfBaseView.empty())
        return;
    if (nameOfNewAttribute.empty())
        return;
    if (equation.empty())
        return;

	DM::View baseView  = getViewInStream("Data", nameOfBaseView);
	if (baseView.getName().length() == 0)
        return;

    viewsmap.clear();
    varaibleNames.clear();
    DM::View writeView = DM::View(baseView.getName(), baseView.getType(), DM::READ);
    writeView.addAttribute(nameOfNewAttribute);
    viewsmap[nameOfBaseView] = writeView;
    for (std::map<std::string, std::string>::const_iterator it = variablesMap.begin();
         it != variablesMap.end();
         ++it) {
        QString viewNametotal = QString::fromStdString(it->first);
        QStringList viewNameList = viewNametotal.split(".");
        std::string viewname = viewNameList[viewNameList.size()-2].toStdString();
        std::string attributename = viewNameList[viewNameList.size()-1].toStdString();
        varaibleNames.push_back(it->second);

        if (viewsmap.find(viewname) == viewsmap.end()) {
			baseView  = getViewInStream("Data", viewname);
			if (baseView.getName().length() == 0)
				return;
            viewsmap[viewname] = DM::View(baseView.getName(), baseView.getType(), DM::READ);
        }

        DM::View toAppend =  viewsmap[viewname];
        if (attributename.compare(nameOfNewAttribute) != 0)
            toAppend.getAttribute(attributename);
        else
            toAppend.modifyAttribute(attributename);
        viewsmap[viewname] = toAppend;

    }
    std::vector<DM::View> data;
    for (std::map<std::string, DM::View>::const_iterator it = viewsmap.begin();
         it != viewsmap.end();
         ++it) {
        data.push_back(it->second);
    }
    this->addData("Data", data);
    int i = 0;
    i++;
}

void  AttributeCalculator::getLinkedAttribute(std::vector< mup::Value> * varaible_container, Component *currentcmp, std::string name )
{
    QStringList viewNameList = QString::fromStdString(name).split(".");
    //Remove First Element, is already what comes with currentcmp
    viewNameList.removeFirst();

    Attribute * attr = currentcmp->getAttribute(viewNameList.front().toStdString());

    if (attr->getType() == Attribute::LINK)
    {
        std::string newSearchName = viewNameList.join(".").toStdString();
        foreach (LinkAttribute l, attr->getLinks())
        {
            Component * nextcmp = this->sys_in->getComponent(l.uuid);
            if(!nextcmp)
            {
                Logger(Error) << "Linked Element does not exist";
                return;
            }
            this->getLinkedAttribute(varaible_container, nextcmp, newSearchName);
        }
    }

    switch (attr->getType()) {
    case Attribute::DOUBLE:
        varaible_container->push_back( mup::Value(attr->getDouble()));
        break;
    case Attribute::STRING:
        varaible_container->push_back(mup::Value(attr->getString()));
        break;
    default:
        varaible_container->push_back(mup::Value(0));
        break;
    }
}


QString AttributeCalculator::IfElseConverter(QString expression)
{

    if (!expression.contains("if")) return expression;
    int firstif = expression.indexOf("if");
    expression = expression.remove(firstif,2);
    int first_semicolon = expression.indexOf(",");
    int else_semicolon = expression.indexOf(",",first_semicolon+1);
    int second_if = expression.indexOf("if");

    expression = expression.replace(first_semicolon, 1,"?");

    if (second_if < else_semicolon && second_if != -1) {
        expression = IfElseConverter(expression);
    }

    second_if = expression.indexOf(",");
    expression = expression.replace(second_if, 1,":");

    if (expression.contains("if")){
        second_if = expression.indexOf(",");
        expression = expression.replace(second_if, 1,":");
        expression = IfElseConverter(expression);

    }
    return expression;
}

void AttributeCalculator::run() {
    this->sys_in = this->getData("Data");
    std::map<std::string, mup::Value * > doubleVariables;
    mup::ParserX * p  = new mup::ParserX();
    foreach (std::string variable, varaibleNames)
    {
        mup::Value * d = new mup::Value(0.0);
        doubleVariables[variable] = d;
        p->DefineVar(variable, d);

        std::string nov_variable = "nov_" + variable;

        d = new mup::Value(0.0);
        doubleVariables[nov_variable] = d;
        p->DefineVar(nov_variable, d);

        std::string first_variable = "first_" + variable;

        d = new mup::Value(0.0);
        doubleVariables[first_variable] = d;
        p->DefineVar(first_variable, d);
    }
    p->DefineFun(new dm::Random);
    p->DefineFun(new dm::Round);
    p->DefineVar("counter", &mp_counter);

    //Logger(Standard) << IfElseConverter(QString::fromStdString(equation)).toStdString();
    p->SetExpr(IfElseConverter(QString::fromStdString(equation)).toStdString());

    mforeach(Component* cmp, sys_in->getAllComponentsInView(viewsmap[nameOfBaseView]))
    {
        //mp_counter= (int) this->getInternalCounter()+1;
        Group* lg = dynamic_cast<Group*>(getOwner());
        if(lg) {
            mp_counter = lg->getGroupCounter();
            DM::Logger(DM::Debug) << "counter " << lg->getGroupCounter();
        }
		else
		{
            DM::Logger(DM::Debug) << "attribute calc: counter not found";
			mp_counter = 0;
		}
        for (std::map<std::string, std::string>::const_iterator it = variablesMap.begin();
             it != variablesMap.end();
             ++it)
        {
            std::string varvalue = it->second;
            //All attributes are stored in one container that is evaluated Later.
            std::vector< mup::Value> variable_container;
            //Can be later replaced by a function
            getLinkedAttribute(&variable_container, cmp, it->first);

            double val = 0;
            QStringList string_vals;
            double nov = 0;
            foreach (mup::Value v, variable_container)
            {
                switch (v.GetType()) {
                case 's':
                    string_vals.append(QString::fromStdString(v.GetString()));
                    break;
                case 'i':
                    val += v.GetInteger();
                    break;
                case 'f':
                    val += v.GetFloat();
                    break;
                default:
                    Logger(Warning) << "Unknown type";
                }
                nov ++;
            }
            if (string_vals.size() == 0)
                *doubleVariables[varvalue] = val;
            else
                *doubleVariables[varvalue] = string_vals.join(",").toStdString();

            *doubleVariables["nov_" + varvalue] = nov;
            if (variable_container.size() > 0) *doubleVariables["first_" + varvalue] = variable_container[0];
            else *doubleVariables["first_" + varvalue] =  0;
        }

        try
        {
            mup::Value val = p->Eval();
            //Logger(Debug) << val.ToString();
            if (!this->asVector) {
                switch (val.GetType()) {
                case 's':
                    cmp->addAttribute(nameOfNewAttribute, val.GetString());
                    break;
                case 'i':
                    cmp->addAttribute(nameOfNewAttribute, val.GetInteger());
                    break;
                case 'f':
                    cmp->addAttribute(nameOfNewAttribute, val.GetFloat());
                    break;
                default:
                    Logger(Warning) << "Unknown type";
                }
            } else
            {
                DM::Attribute * attri = cmp->getAttribute(nameOfNewAttribute);
                std::vector<double> vD = attri->getDoubleVector();
                vD.push_back(val.GetFloat());
                attri->setDoubleVector(vD);
            }
        }
        catch (mup::ParserError &e)
        {
            Logger(Error) << e.GetMsg();
        }
    }

    foreach (std::string variable, varaibleNames)
        delete doubleVariables[variable];

    doubleVariables.clear();
    delete p;
}

bool AttributeCalculator::createInputDialog() {
    QWidget * w = new GUIAttributeCalculator(this);
    w->show();
    return true;
}

string AttributeCalculator::getHelpUrl()
{
    return "https://github.com/iut-ibk/DynaMind-BasicModules/blob/master/doc/AttributeCalculator.md";
}
