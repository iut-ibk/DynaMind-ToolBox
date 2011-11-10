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
 * The class contains provides the VIBe2 Database
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "compilersettings.h"
#include <iostream>
#include <rasterdata.h>
#include <vectordata.h>
#include <map>
#include <dataobserver.h>
#include <idatabase.h>

namespace vibens {
    class Module;

    class VIBE_HELPER_DLL_EXPORT  DataBase : public IDataBase
    {

    private:

        std::map<std::string, RasterData *> RasterDataMaps_0;
        std::map<std::string, RasterData *> RasterDataMaps_1;
        std::map<std::string,int> RasterDataMaps_Switch;

        std::map<std::string, VectorData *> VectorDataMaps_0;
        std::map<std::string, VectorData *> VectorDataMaps_1;
        std::map<std::string,int> VectorDataMaps_Switch;


        std::map<std::string, double> DoubleDataMaps_0;
        std::map<std::string, double> DoubleDataMaps_1;



        std::string createName(std::string UUID, std::string Name);

        std::vector<DataObserver*> observer;

    public:
        DataBase();
        ~DataBase();

        void setRasterData(std::string UUID, std::string Name,  RasterData & r);
        RasterData & getRasterData(std::string UUID, std::string Name, bool read = true, bool fromBack = false);
        RasterData & createRasterData(std::string UUID, std::string Name);

        void setVectorData(std::string UUID, std::string Name, VectorData & v);
        VectorData & getVectorData(std::string UUID, std::string Name, bool read = true, bool fromBack = false);
        VectorData & createVectorData(std::string UUID, std::string Name);
        VectorData & getVectorData(std::string Name);
        void setDoubleData(std::string UUID, std::string Name, double v);
        double getDoubleData(std::string UUID, std::string Name, bool read = true, bool fromBack = false);
        void createDoubleData(std::string UUID, std::string Name);

        void resetDataBase();
        void registerDataObserver(DataObserver * ob) {this->observer.push_back(ob);}

        std::vector<std::string> getRegisteredDatasets() const;


        std::vector<std::string> getRegisteredDatasetForModule(std::string) const;

    };
}
#endif // DATABASE_H
