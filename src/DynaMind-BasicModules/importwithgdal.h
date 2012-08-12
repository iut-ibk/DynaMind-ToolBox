/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
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

#ifndef IMPORTWITHGDAL_H
#define IMPORTWITHGDAL_H
#include <dmmodule.h>
#include <dm.h>
#include "gdal/ogrsf_frmts.h"
#include <QHash>
#include <QString>


using namespace DM;

class DM_HELPER_DLL_EXPORT ImportwithGDAL : public Module
{
    DM_DECLARE_NODE(ImportwithGDAL)
    private:
        std::string FileName;
    std::string ViewName;
    double tol;
    DM::View view;
    QHash<QString, std::vector<DM::Node* > *> nodeList;
    DM::Node * addNode(DM::System * sys, double x, double y, double z);
    void appendAttributes(DM::Component * cmp, OGRFeatureDefn *poFDefn, OGRFeature *poFeature);
    DM::Component * loadNode(DM::System * sys,  OGRFeature *poFeature);
    DM::Component * loadFace(DM::System * sys,  OGRFeature *poFeature);
    double devider;
public:
    void run();
    void init();
    ImportwithGDAL();
};

#endif // IMPORTWITHGDAL_H
