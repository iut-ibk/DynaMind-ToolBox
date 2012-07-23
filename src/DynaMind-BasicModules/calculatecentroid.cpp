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


#include "calculatecentroid.h"
#include "tbvectordata.h"
#include "guicalculatecentroid.h"
#include <math.h>


DM_DECLARE_NODE_NAME(CalculateCentroid, Modules)

CalculateCentroid::CalculateCentroid()
{
    this->city = 0;
    this->NameOfExistingView = "";
    std::vector<DM::View> data;
    data.push_back(  DM::View ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
    this->addParameter("NameOfExistingView", DM::STRING, & this->NameOfExistingView);
    this->addData("Data", data);
    changed = true;
}


void CalculateCentroid::init()
{

    city = this->getData("Data");
    if (city == 0)
        return;
    std::vector<std::string> views = city->getNamesOfViews();

    foreach (std::string s, views)
        DM::Logger(DM::Debug) << s;

    if (this->NameOfExistingView.empty())
        return;
    DM::View * v = city->getViewDefinition(NameOfExistingView);
    DM::View writeView = DM::View(v->getName(), v->getType(), DM::READ);
    writeView.addAttribute("centroid_x");
    writeView.addAttribute("centroid_y");

    //if (changed == true) {
        std::vector<DM::View> data;
        data.push_back(writeView);
        this->addData("Data", data);
    //}
    vData = writeView;
    //changed = false;

    //this->updateParameter();
}


void CalculateCentroid::run() {
    city = this->getData("Data");
    std::vector<std::string> names =city->getUUIDsOfComponentsInView(vData);

    foreach(std::string name, names) {
        Face * f = city->getFace(name);

        Node p = TBVectorData::caclulateCentroid(this->city, f);
        double area = fabs(TBVectorData::calculateArea(this->city, f));

        f->addAttribute("centroid_x", p.getX());
        f->addAttribute("centroid_y", p.getY());
        f->addAttribute("area", area);
    }
}
bool CalculateCentroid::createInputDialog() {
    QWidget * w = new GUICalculateCentroid(this);
    w->show();
    return true;
}

void CalculateCentroid::setNameOfView(string name) {
    changed = true;
    this->NameOfExistingView = name;

}
DM::System * CalculateCentroid::getSystemIn() {
    return this->city;
}

