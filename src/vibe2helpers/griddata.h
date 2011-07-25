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


#ifndef GRIDDATA_H
#define GRIDDATA_H
#include "compilersettings.h"
#include <vector>
#include <QString>
#include <iostream>
#include "vectordata.h"
#include "ILinkedVectorData.h"

class VIBE_HELPER_DLL_EXPORT GridData:public ILinkedVectorData
{
private:

    QString CurrenAttribute;
    int Year;

    VectorData vec;
    std::vector<float> VectorCoordinates;
    std::vector<float> VectorColor;
public:
    GridData();
    void setCurrentAttribute(QString name){this->CurrenAttribute = name;}
    void setYear(int y){this->Year = y;}
    void setVectorData(VectorData vec){this->vec = vec;}
    double Sum;
    const std::vector<float> & getVectorCoordinates() {return this->VectorCoordinates;}
    const std::vector<float> & getVectorColor() {return this->VectorColor;}
    const int & getYear(){return this->Year;}
    std::vector<std::string>  getAttributesInGridCell(int id);
    void createDataForViewer();
};

#endif // GRIDDATA_H
