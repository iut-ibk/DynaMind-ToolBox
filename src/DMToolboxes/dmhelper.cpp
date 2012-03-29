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

#include "dmhelper.h"
#include <QString>
#include <sstream>

std::map<int, int> DMHelper::convertStringMapToIntMap(std::map<std::string, std::string>  stringmap) {
    std::map<int, int> returnmap;
    for(std::map<std::string, std::string>::const_iterator it = stringmap.begin(); it != stringmap.end(); ++it) {
        returnmap[QString::fromStdString(it->first).toInt()] = QString::fromStdString(it->second).toInt();
    }

    return returnmap;
}

std::map<std::string, std::string> DMHelper::convertIntMapToStringMap(std::map<int, int>  map) {
    std::map<std::string, std::string> returnmap;
    for(std::map<int, int>::const_iterator it = map.begin(); it != map.end(); ++it) {
        std::stringstream ss1;
        std::stringstream ss2;
        ss1 << it->first;
        ss2 << it->second;
        returnmap[ss1.str()] = ss2.str();
    }

    return returnmap;
}

std::string DMHelper::convertIntMapToDMMapString(std::map<int, int>  intmap) {

    std::stringstream ss;
    for (std::map<int, int>::const_iterator it = intmap.begin(); it != intmap.end(); ++it) {
        ss << it->first << "*|*" << it->second << "*||*" ;
    }
    std::string s = ss.str();
    return ss.str();
}
