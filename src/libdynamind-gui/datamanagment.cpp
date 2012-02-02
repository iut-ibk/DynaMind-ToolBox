/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#include "datamanagment.h"
#include <iostream>
#include <QString>
#include <module.h>
#include <boost/foreach.hpp>
#include <sstream>

void DataManagment::observeNewVectorData(DM::Module * module, std::string name) {

   //if (name.compare("VectorDataTo_85") == 0 || name.compare("VectorData_out_117") == 0 || name.compare("VectorData_out_118") == 0 ) {
        /*QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(name));
        QTreeWidgetItem * itemPoints = new QTreeWidgetItem();
        itemPoints->setText(0, "Points");
        item->addChild(itemPoints);
        QTreeWidgetItem * itemLines = new QTreeWidgetItem();
        itemLines->setText(0, "Lines");
        item->addChild(itemLines);

        VectorData vec = module->getVectorData(name);

        BOOST_FOREACH( std::string s, vec.getPointsNames() ) {
            QTreeWidgetItem * itemPoint = new QTreeWidgetItem();
            itemPoint->setText(0, QString::fromStdString(s));
            itemPoints->addChild(itemPoint);
        }

        BOOST_FOREACH( std::string s, vec.getEdgeNames() ) {
            QTreeWidgetItem * itemLine = new QTreeWidgetItem();
            itemLine->setText(0, QString::fromStdString(s));
            itemLines->addChild(itemLine);
        }
        this->vec_data[name] = vec;
        rootVectorItem->addChild(item);*/

   //}


}
DataManagment::DataManagment(QTreeWidgetItem * rootItem, Plot *plot)
{
    x = 10;
    this->rootVectorItem = new QTreeWidgetItem();
    this->rootVectorItem->setText(0, "VectorData");
    this->rootRasterItem = new QTreeWidgetItem();
    this->rootRasterItem->setText(0, "RasterData");

    rootItem->addChild(rootVectorItem);
    rootItem->addChild(rootRasterItem);
    this->plot = plot;

}
DataManagment::~DataManagment() {
    //rootItem->removeChild(this->rootVectorItem);
    //rootItem->removeChild(this->rootRasterItem);
    deleteTreeWidgetItem (this->rootVectorItem);
    deleteTreeWidgetItem(this->rootRasterItem);
}

void DataManagment::deleteTreeWidgetItem(QTreeWidgetItem* item)
{
    while(item->childCount() > 0)
    {
        deleteTreeWidgetItem(item->child(0));
        item->removeChild(0);
    }

    delete item;
}

void DataManagment::observeNewRasterData(DM::Module * module, std::string name) {
   //if (name.compare("MarkerMap_67") == 0 || name.compare("MarkerMap_74") == 0) {
         //if (name.compare("MarkerMap_67") == 0 || name.compare("Street") == 0 || name.compare("Agents") == 0 ) {
        /*RasterData rData = module->getRasterData(name);

        if (name.compare("Agents") == 0 || name.compare("Tracks") == 0) {
            double val = rData.getSum();
            if (val > 0)  {
                //plot->appendData(module->getT(), rData.getSum());
            }
        }

        std::stringstream s;
        s  << " " << name;
        name = s.str();

        QTreeWidgetItem * item = new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(name));
        rootRasterItem->addChild(item);

        this->r_data[name] = rData;*/

    //}

}

void DataManagment::observerDeletedRasterData(DM::Module * module) {
}

void DataManagment::observerDeletedVectorData(DM::Module *   module) {
}
void DataManagment::setCurrentScene(std::string scene, int ID){
    /*this->currentScene = scene;
    if ( ID == VECTORDATA)
        viewer->setVectorData(this->vec_data[scene]);
    if ( ID == RASTERDATA)
        viewer->setRasterData(this->r_data[scene]);*/

}
