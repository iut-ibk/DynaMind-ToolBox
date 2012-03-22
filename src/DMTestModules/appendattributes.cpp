/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich

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

#include "appendattributes.h"
#include <rasterdatahelper.h>
#include <tbvectordata.h>
#include <rasterdatahelper.h>
#include <guiappendattributes.h>

DM_DECLARE_NODE_NAME( AppendAttributes ,Modules )


AppendAttributes::AppendAttributes() {
    this->median = false;
    this->multiplier = 1;

    this->addParameter("Multiplier", DM::DOUBLE, &this->multiplier);
    this->addParameter("Median", DM::BOOL, &this->median);
    this->addParameter("NameOfRasterData", DM::STRING, &this->NameOfRasterData);
    this->addParameter("NameOfExistingView", DM::STRING, &this->NameOfExistingView);
    this->addParameter("newAttribute", DM::STRING, &this->newAttribute);

    sys_in = 0;
    this->NameOfRasterData = "";
    this->NameOfExistingView = "";
    this->newAttribute = "";

    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );


    this->addData("Data", data);




}

void AppendAttributes::run() {

    DM::System * sys = this->getData("Data");
    DM::View v_existing= sys->getViewDefinition(NameOfExistingView);
    DM::RasterData * r = this->getRasterData("Data", DM::View(NameOfRasterData, DM::READ, DM::RASTERDATA));
    foreach (std::string s, sys->getNamesOfComponentsInView(v_existing)) {
        DM::Face * f = sys->getFace(s);
        std::vector<DM::Node*> nl = TBVectorData::getNodeListFromFace(sys, f);
        double dattr = 0;
        if (median) {
            dattr = RasterDataHelper::meanOverArea(r,nl) * multiplier;
        } else {
            dattr = RasterDataHelper::sumOverArea(r,nl,0) * multiplier;
        }
        f->changeAttribute(newAttribute, dattr);
    }
}

bool AppendAttributes::createInputDialog() {
    QWidget * w = new GUIAppendAttributes(this);
    w->show();
    return true;
}

void AppendAttributes::init()
{

    sys_in = this->getData("Data");
    if (sys_in == 0)
        return;
    std::vector<std::string> views = sys_in->getViews();

    foreach (std::string s, views)
        DM::Logger(DM::Debug) << s;

    if (this->NameOfExistingView.empty())
        return;
    if (this->newAttribute.empty())
        return;
    if (newAttribute_old.compare(newAttribute) == 0)
        return;

    DM::View v = sys_in->getViewDefinition(NameOfExistingView);
    readView = DM::View(v.getName(), v.getType(), DM::READ);
    readView.addAttribute(newAttribute);

    data.push_back(readView);
    this->addData("Data", data);

    newAttribute_old = newAttribute;


}
