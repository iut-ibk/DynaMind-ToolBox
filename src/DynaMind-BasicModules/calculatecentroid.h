/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

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

#ifndef CALCULATECENTROID_H
#define CALCULATECENTROID_H
#include <dmmodule.h>
#include <dm.h>
using namespace DM;
/** @brief Calculates the centroid of the given faces
  *
  * Calculates the centroid of the given faces
  * The centroid is added to the attribute
  * - Identifer
  *     - centroid_x
  *     - centroid_y
  * @ingroup Modules
  * @author Christian Urich
  */

class DM_HELPER_DLL_EXPORT CalculateCentroid : public Module
{
    DM_DECLARE_NODE(CalculateCentroid)
private:
    std::string NameOfExistingView;
    DM::System * city;
    bool changed;
    DM::View vData;
    DM::View newPoints;

public:
    CalculateCentroid();
    void run();
    void init();
    bool createInputDialog();
    void setNameOfView(std::string name);
    DM::System * getSystemIn();
    std::string getHelpUrl();
};

#endif // CALCULATECENTROID_H
