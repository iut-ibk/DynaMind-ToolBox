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

#include "parceldata.h"
#include "vectordatahelper.h"
#include <sstream>
ParcelData::ParcelData()
{
}
void ParcelData::createDataForViewer() {
std::cout << "Start Create Data for Viewer" <<std::endl;
    std::vector<std::string> names = VectorDataHelper::findElementsWithIdentifier("parcel_", vec.getFaceNames());

    //MarkParcels with buldings
    std::vector<std::string> buildings = VectorDataHelper::findElementsWithIdentifier("building_", vec.getAttributeNames());

    foreach (std::string building, buildings) {
        Attribute b = vec.getAttributes(building);
        int id =(int) b.getAttribute("parcel_id");

        std::stringstream p;
        p << "parcel_" << id;
        Attribute attr = vec.getAttributes(p.str());

        attr.setAttribute("hasBuilding", 1);
        vec.setAttributes(p.str(), attr);

    }

    foreach (std::string name,  names) {
        if (QString::fromStdString(name).contains("COLOR") == false ) {
            bool existsColor = false;
           /* std::stringstream s;
            s << "COLOR_" << name;
            foreach(std::string n, vec.getPointsNames()) {
                if (n.compare(s.str()) == 0) {
                    existsColor = true;
                    break;
                }
            }*/
            Attribute attr = vec.getAttributes(name);
            std::vector<Face> faces =  vec.getFaces(name);
            std::vector<Point> points =  vec.getPoints(name);
            std::vector<Point> colors;
            /*if (existsColor == true) {
                colors = vec.getPoints(s.str());
            }*/
            foreach( Face f, faces ) {
                int k = 0;
                foreach(int id, f.ids) {
                    if (attr.getAttribute("hasBuilding") > 0) {
                        VectorColor.push_back(1.);
                        VectorColor.push_back(0.);
                        VectorColor.push_back(0.);
                    } else {
                        VectorColor.push_back(0.);
                        VectorColor.push_back(1.);
                        VectorColor.push_back(0.);
                    }
                    VectorCoordinates.push_back((float)points[id].x);
                    VectorCoordinates.push_back((float)points[id].y);
                    VectorCoordinates.push_back((float)points[id].z);
                    k++;
                    if (k > 3)
                        break;
                }

            }
        }
    }
    std::cout << "End Create Data for Viewer" <<std::endl;
}
