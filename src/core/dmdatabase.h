/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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

#ifndef DMDATABASE_H
#define DMDATABASE_H

#include "dmcompilersettings.h"
#include <iostream>
#include <dmrasterdata.h>
#include <map>
#include <dmdataobserver.h>
#include <dmidatabase.h>


#include <dmcomponent.h>
#include <dmnode.h>
#include <dmedge.h>
#include <dmsystem.h>


using namespace DM;
using namespace DM;



class Module;

class DM_HELPER_DLL_EXPORT  DMDatabase : public IDataBase
{


private:

    std::map<std::string, RasterData *> RasterDataMaps_0;
    std::map<std::string, RasterData *> RasterDataMaps_1;
    std::map<std::string,int> RasterDataMaps_Switch;


    std::map<std::string, double> DoubleDataMaps_0;
    std::map<std::string, double> DoubleDataMaps_1;



    std::string createName(std::string UUID, std::string Name);

    std::vector<DataObserver*> observer;


    DM::System * system;

public:
    DMDatabase();
    ~DMDatabase();

    void setRasterData(std::string UUID, std::string Name,  RasterData & r);
    RasterData & getRasterData(std::string UUID, std::string Name, bool read = true, bool fromBack = false);
    RasterData & createRasterData(std::string UUID, std::string Name);

    void setDoubleData(std::string UUID, std::string Name, double v);
    double getDoubleData(std::string UUID, std::string Name, bool read = true, bool fromBack = false);
    void createDoubleData(std::string UUID, std::string Name);

    void resetDataBase();
    void registerDataObserver(DataObserver * ob) {this->observer.push_back(ob);}

    std::vector<std::string> getRegisteredDatasets() const;


    std::vector<std::string> getRegisteredDatasetForModule(std::string) const;

};



#endif // DMDATABASE_H
