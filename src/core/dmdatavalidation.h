/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
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
#ifndef DMDATABVALIDATION_H
#define DMDATABVALIDATION_H

#include <vector>
namespace DM {
    class View;
    class Component;
}


namespace DM {
/** @ingroup DynaMind-Core
  * @brief Helper class for data validation of views
  */
class  DataValidation
{
public:
    /** @brief Returns true if one of the AccessType of the component or one of the Attributes is Read or Modify*/
    static bool isVectorOfViewRead(std::vector<DM::View>);

     /** @brief Returns true if one of the AccessType of the component or one of the Attributes is Write or Modify*/
    static bool isVectorOfViewWrite(std::vector<DM::View>);

    /** @brief Returns if all data from a view are avalible in a component */
    //static bool isAvalibleInComponent(const DM::View &,  const DM::Component * );
};
}
#endif // DMDATABVALIDATION_H
