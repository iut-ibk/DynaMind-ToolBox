/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * DAnCE4Water (Dynamic Adaptation for eNabling City Evolution for Water) is a
 * strategic planning tool ti identify the transtion from an from a conventional
 * urban water system to an adaptable and sustainable system at a city scale
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
 * @section DESCRIPTION
 *
 * The class contains provides the interface to the VIBe2 simulation
 */


#include "griddata.h"
#include <sstream>
#include "vectordatahelper.h"

GridData::GridData()
{
}

std::vector<std::string> GridData::getAttributesInGridCell(int id) {

    std::vector<std::string> results;
    std::vector<std::string> names = this->vec.getAttributeNames();

    foreach(std::string name, names) {
        Attribute attr = vec.getAttributes(name);
        if (id == attr.getAttribute("GRID_ID"))
            results.push_back(name);

    }

    return results;
}
//Create Drawing Lists;
void GridData::createDataForViewer() {
    foreach (std::string name, VectorDataHelper::findElementsWithIdentifier("GRID_", vec.getFaceNames()) ) {
        if (QString::fromStdString(name).contains("COLOR") == false ) {
            bool existsColor = false;
            std::stringstream s;
            s << "COLOR_" << name;
            foreach(std::string n, vec.getPointsNames()) {
                if (n.compare(s.str()) == 0) {
                    existsColor = true;
                    break;
                }
            }
            Attribute attr = vec.getAttributes(name);
            std::vector<Face> faces =  vec.getFaces(name);
            std::vector<Point> points =  vec.getPoints(name);
            std::vector<Point> colors;
            if (existsColor == true) {
                colors = vec.getPoints(s.str());
            }
            foreach( Face f, faces ) {
                int k = 0;
                foreach(int id, f.ids) {
                    if (existsColor == true) {
                        VectorColor.push_back((float)colors[0].x);
                        VectorColor.push_back((float)colors[0].y);
                        VectorColor.push_back((float)colors[0].z);
                    } else {
                        VectorColor.push_back(0);
                        VectorColor.push_back(1);
                        VectorColor.push_back(0);
                    }
                    VectorCoordinates.push_back((float)points[id].x);
                    VectorCoordinates.push_back((float)points[id].y);
                    VectorCoordinates.push_back((float)attr.getAttribute("Population"));
                    k++;
                    if (k > 3)
                        break;
                }

            }
        }
    }
}
