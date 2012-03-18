/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
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
#include "dmdatamanagement.h"
#include <dmidatabase.h>
#include <dmlogger.h>
namespace DM {

    DataManagement *DataManagement::instance = 0;

    DataManagement::DataManagement()
    {
    }
    DataManagement::~DataManagement()
    {
    };
    DataManagement& DataManagement::getInstance()
    {
        if (!instance) {
            Logger(Error) << "ERROR no Database instance registerd";
        }
        return *(instance);
    }

    void DataManagement::registerDataBase(IDataBase *database) {
        this->database = database;
    }
    IDataBase *DataManagement::getDataBase() {
        return this->database;
    }
    void DataManagement::init(){
        instance = new DataManagement();
    }
}
