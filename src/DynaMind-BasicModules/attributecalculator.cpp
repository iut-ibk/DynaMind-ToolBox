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

#include "attributecalculator.h"
#include "guiattributecalculator.h"
#include "muParser.h"
#include "userdefinedfunctions.h"
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
    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addData("Data", data);
}

void AttributeCalculator::init() {
    this->sys_in = this->getData("Data");

    if (!this->sys_in)
        return;
    if (nameOfBaseView.empty())
        return;
    if (nameOfNewAttribute.empty())
        return;
    if (equation.empty())
        return;

    viewsmap.clear();
    varaibleNames.clear();
    DM::View * baseView = this->sys_in->getViewDefinition(nameOfBaseView);
    DM::View writeView = DM::View(baseView->getName(), baseView->getType(), DM::READ);
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
            baseView = this->sys_in->getViewDefinition(viewname);
            if (!baseView)
                return;
            viewsmap[viewname] = DM::View(baseView->getName(), baseView->getType(), DM::READ);
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

void  AttributeCalculator::getLinkedAttriubte(std::vector<double> * varaible_container, Component *currentcmp,std::string name ) {
    QString viewNametotal = QString::fromStdString(name);
    QStringList viewNameList = viewNametotal.split(".");
    //Remove First Element, is already what comes with currentcmp
    viewNameList.removeFirst();
    //If viewNameList > 1
    //Unit.Area
    Attribute * attr = currentcmp->getAttribute(viewNameList.front().toStdString());

    if (attr->getType() == Attribute::LINK) {
        std::string newSearchName = viewNameList.join(".").toStdString();
        foreach (LinkAttribute l, attr->getLinks()) {
            Component * nextcmp = this->sys_in->getComponent(l.uuid);
            if(!nextcmp)  {
                Logger(Error) << "Linked Element does not exist in DB";
                return;
            }
            this->getLinkedAttriubte(varaible_container, nextcmp, newSearchName);
        }
    }

    if (attr->getType() == Attribute::DOUBLE) {
        varaible_container->push_back(attr->getDouble());
    }
}

void AttributeCalculator::run() {

    this->sys_in = this->getData("Data");
    std::map<std::string, double * > doubleVaraibles;
    mu::Parser * p  = new mu::Parser();
    foreach (std::string variable, varaibleNames) {
        double * d = new double;
        *d = 0;
        doubleVaraibles[variable] = d;
        p->DefineVar(variable, d);

        std::stringstream nov_stream;
        nov_stream << "nov_" << variable;

        d = new double;
        *d = 0;
        doubleVaraibles[nov_stream.str()] = d;
        p->DefineVar(nov_stream.str(), d);

        std::stringstream first_stream;
        first_stream << "first_" << variable;
        d = new double;
        *d = 0;
        doubleVaraibles[first_stream.str()] = d;
        p->DefineVar(first_stream.str(), d);


    }
    p->DefineFun("rand", mu::random , false);
    p->DefineFun("round", mu::round);

    double counter = 0;

    p->DefineVar("counter", &counter);
    p->SetExpr(equation);

    std::vector<std::string> uuids = this->sys_in->getUUIDsOfComponentsInView(viewsmap[nameOfBaseView]);

    foreach (std::string uuid, uuids) {
        counter++;
        //UpdateParameters
        //House.Unit.Area
        DM::Component * cmp = this->sys_in->getComponent(uuid); //House
        for (std::map<std::string, std::string>::const_iterator it = variablesMap.begin();
             it != variablesMap.end();
             ++it) {

            //All attributes are stored in one container that is evaluated Later.
            std::vector<double> * varaible_container = new std::vector<double>();

            //Can be later replaced by a function
            double val = 0;
            std::string varname = it->first;
            getLinkedAttriubte(varaible_container, cmp, varname);

            std::stringstream nov_stream;
            nov_stream << "nov_" << it->second;

            std::stringstream first_stream;
            first_stream << "first_" << it->second;

            double nov = 0;
            foreach (double v, *varaible_container) {
                val+=v;
                nov+=1;
            }



            double * var = doubleVaraibles[it->second];
            (*var) = val;
            double * nov_var = doubleVaraibles[nov_stream.str()];
            (*nov_var) =  nov;
            double * first_nov = doubleVaraibles[first_stream.str()];
            if (varaible_container->size() > 0)
                (*first_nov) = (*varaible_container)[0];
            else
                (*first_nov) = 0;
            delete varaible_container;
        }
        try {
            double d = p->Eval();
            if (!this->asVector) {
                cmp->addAttribute(nameOfNewAttribute, d);
            } else {
                DM::Attribute * attri = cmp->getAttribute(nameOfNewAttribute);
                std::vector<double> vD = attri->getDoubleVector();
                vD.push_back(d);
                attri->setDoubleVector(vD);
            }

        }
        catch (mu::Parser::exception_type &e) {
            Logger(Error) << e.GetMsg();
        }
    }

    foreach (std::string variable, varaibleNames) {
        delete doubleVaraibles[variable];
    }
    doubleVaraibles.clear();
    delete p;

}

bool AttributeCalculator::createInputDialog() {
    QWidget * w = new GUIAttributeCalculator(this);
    w->show();
    return true;
}

string AttributeCalculator::getHelpUrl()
{
    return "https://docs.google.com/document/pub?id=1rcXPDk_3jC1wYzX3yiTuG7LBQymg751rccJ0IcPXu6g";
}
