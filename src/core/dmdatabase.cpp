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

#include "dmdatabase.h"
#include <vibe_logger.h>
#include <QStringList>


DMDatabase::DMDatabase()
{
    this->system = new DM::System("root", "id");
}
DMDatabase::~DMDatabase()
{
    this->resetDataBase();
}
//RasterData
void DMDatabase::setRasterData(std::string UUID, std::string Name,  RasterData &r) {
    /*Logger(Debug) << "Set Switch" << Name;

    RasterData * r0 =  this->RasterDataMaps_0[createName(UUID, Name)];
    RasterData * r1 =  this->RasterDataMaps_1[createName(UUID, Name)];


    this->RasterDataMaps_1[createName(UUID, Name)] = r0;
    this->RasterDataMaps_0[createName(UUID, Name)] = r1;
    std::stringstream ss;
    ss << r0;
    Logger(Debug) << ss.str();
    std::stringstream ss1;
    ss1 << r1;
    Logger(Debug) << ss1.str();


    foreach(DataObserver * ob, observer)
        ob->observeNewRasterData(*this->RasterDataMaps_1[createName(UUID, Name)]);*/
}

std::vector<std::string> DMDatabase::getRegisteredDatasets() const {
    std::vector<std::string> names;

    /*for (std::map<std::string, VectorData *>::const_iterator it = this->VectorDataMaps_0.begin();
         it != this->VectorDataMaps_0.end();
         ++it) {
        std::string name = it->first;
        names.push_back(name);

    }
    return names;*/
}

RasterData & DMDatabase::getRasterData(std::string UUID, std::string Name, bool read, bool fromBack) {


   /*if (fromBack) {
        Logger(Debug) << "Split RasterData" << Name;
        RasterData * r0 = RasterDataMaps_0[createName(UUID, Name)];
        RasterData * r1 = RasterDataMaps_1[createName(UUID, Name)];

        if (r1 == r0) {

            RasterData * r = new RasterData();
            RasterDataMaps_1[createName(UUID, Name)] = r;
            r1 = r;
            r1->setSize(r0->getWidth(), r0->getHeight(), r0->getCellSize());
            r1->setNoValue(r0->getNoValue());
            r1->setUUID(r0->getUUID());
            r1->setName(r0->getName());

            //Copy RasterData
            for (int i=0; i < r0->getWidth(); i++) {
                for (int j=0 ; j < r0->getHeight(); j++) {
                    r1->setValue(i,j,r0->getValue(i,j));
                }
            }
        }
        std::stringstream ss;
        Logger(Debug) << "Splitted Data";
        ss << r0;
        Logger(Debug) << ss.str();
        std::stringstream ss1;
        ss1 << r1;
        Logger(Debug) << ss1.str();
        Logger(Debug) << "End Data";
    }
    if (read) {
        std::stringstream ss;
        ss << RasterDataMaps_1[createName(UUID, Name)];
        Logger(Debug) << ss.str();
        return *(RasterDataMaps_1[createName(UUID, Name)]);
    }
    return *(RasterDataMaps_0[createName(UUID, Name)]);*/



}
RasterData & DMDatabase::createRasterData(std::string UUID, std::string Name) {
    /*RasterData * r = new RasterData();
    r->setName(Name);
    r->setUUID(UUID);
    RasterDataMaps_0[createName(UUID, Name)] = r;
    RasterDataMaps_1[createName(UUID, Name)] = r;
    RasterDataMaps_Switch[createName(UUID, Name)] = 0;
    return *(RasterDataMaps_0[createName(UUID, Name)]);*/

}

//VectorData
/*VectorData & DMDatabase::getVectorData(std::string Name) {
    QStringList sl = QString::fromStdString(Name).split("_");
    QString rest = QString::fromStdString(Name).remove(sl[0]+ "_");
    return this->getVectorData(sl[0].toStdString(),rest.toStdString());
}*/

/*VectorData & DMDatabase::getVectorData(std::string UUID, std::string Name, bool read, bool fromBack) {
    if (fromBack) {
        Logger(Debug) << "From Back";
        VectorData * v0 = VectorDataMaps_0[createName(UUID, Name)];
        VectorData * v1 = VectorDataMaps_1[createName(UUID, Name)];

        if (v1 == v0) {
            Logger(Debug) << "Create Split Back " << createName(UUID, Name);
            VectorData * v = new VectorData();
            VectorDataMaps_1[createName(UUID, Name)] = v;
            v1 = v;
            v1->setUUID(v0->getUUID());
            v1->setName(v0->getName());
            *(v1) = *(v0);
        }
    }
    if (read) {
        Logger(Debug) <<"Read" << createName(UUID, Name);
        VectorData * v = VectorDataMaps_1[createName(UUID, Name)];
        return *(VectorDataMaps_1[createName(UUID, Name)]);
    }
    Logger(Debug) <<"Read" << createName(UUID, Name);


    return *(VectorDataMaps_0[createName(UUID, Name)]);
}

VectorData & DMDatabase::createVectorData(std::string UUID, std::string Name) {
    VectorData * v = new VectorData();
    v->setName(Name);
    v->setUUID(UUID);
    VectorDataMaps_0[createName(UUID, Name)] = v;
    VectorDataMaps_1[createName(UUID, Name)] = v;
    VectorDataMaps_Switch[createName(UUID, Name)] = 0;
    return *(VectorDataMaps_0[createName(UUID, Name)]);
}

void DMDatabase::setVectorData(std::string UUID, std::string Name,  VectorData &v) {
    Logger(Debug) << "Set" << UUID <<  "  " << Name << "VectorData Switch";
    VectorData * v0 =  this->VectorDataMaps_0[createName(UUID, Name)];
    VectorData * v1 =  this->VectorDataMaps_1[createName(UUID, Name)];


    this->VectorDataMaps_1[createName(UUID, Name)] = v0;
    this->VectorDataMaps_0[createName(UUID, Name)] = v1;
}*/

//DoubleData
double DMDatabase::getDoubleData(std::string UUID, std::string Name, bool read, bool fromBack) {
    if (read) {
        Logger(Debug) << "GetDoubleData" << DoubleDataMaps_0[createName(UUID, Name)];
        return DoubleDataMaps_0[createName(UUID, Name)];
    }
    return DoubleDataMaps_1[createName(UUID, Name)];
}

void DMDatabase::createDoubleData(std::string UUID, std::string Name) {
    DoubleDataMaps_0[createName(UUID, Name)] = 0;
    DoubleDataMaps_1[createName(UUID, Name)] = 0;
}

void DMDatabase::setDoubleData(std::string UUID, std::string Name,  double v) {
    Logger(Debug) << "SetDoubleData" << Name << v;
    this->DoubleDataMaps_0[createName(UUID, Name)] = v;
    double v0 =  this->DoubleDataMaps_0[createName(UUID, Name)];


    this->DoubleDataMaps_1[createName(UUID, Name)] = v0;
    this->DoubleDataMaps_0[createName(UUID, Name)] = v;
}

void DMDatabase::resetDataBase() {
    Logger(Debug) << "ResetDataBase";
    for (std::map<std::string, RasterData *>::iterator it = this->RasterDataMaps_0.begin(); it != this->RasterDataMaps_0.end(); ++it) {
        std::string name = it->first;
        RasterData * r = it->second;
        if (this->RasterDataMaps_1[name] != r) {
            delete this->RasterDataMaps_1[name];
        }

        delete r;
    }
    this->RasterDataMaps_0.clear();
    this->RasterDataMaps_1.clear();
    /*for (std::map<std::string, VectorData *>::iterator it = this->VectorDataMaps_0.begin(); it != this->VectorDataMaps_0.end(); ++it) {
        std::string name = it->first;
        VectorData * r = it->second;
        if (this->VectorDataMaps_1[name] != r) {
            delete this->VectorDataMaps_1[name];
        }
        delete r;
    }
    this->VectorDataMaps_0.clear();
    this->VectorDataMaps_1.clear();*/
}

std::string DMDatabase::createName(std::string UUID, std::string Name) {
    std::stringstream ss;
    ss <<  UUID;
    ss << "_";
    ss << Name;
    return ss.str();
}

std::vector<std::string> DMDatabase::getRegisteredDatasetForModule(std::string identifier) const {
    std::vector<std::string> names;

    /*for (std::map<std::string, VectorData *>::const_iterator it = this->VectorDataMaps_0.begin();
         it != this->VectorDataMaps_0.end();
         ++it) {
        std::string name = it->first;

        int pos = name.find("_");
        std::string id = name.substr(0,pos);
        if (id.compare(identifier) == 0)
            names.push_back(name.substr(pos+1, name.size()-1));
    }*/
    return names;
}


