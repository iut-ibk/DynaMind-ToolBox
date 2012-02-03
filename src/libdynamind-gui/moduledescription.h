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

#ifndef MDMODULE_H
#define MDMODULE_H

#include "compilersettings.h"
#include <QString>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QStringList>

class DM_HELPER_DLL_EXPORT Points {
public:

    Points(){}
    Points(const Points & other) {
        VectorDataName = other.VectorDataName;
        PointsName = other.PointsName;
    }

    QString VectorDataName;
    QString PointsName;

};
Q_DECLARE_METATYPE(Points);
struct  DM_HELPER_DLL_EXPORT VectorDataDescription {
    QString Name;
    QStringList Points;
    QStringList Edges;
    QStringList Faces;
    QStringList Links;
    QStringList Attributes;
};

class  DM_HELPER_DLL_EXPORT ModuleDescription {
public:
    QString name;
    QString filename;
    QVector<QString> outputRasterData;
    QVector<QString> inputRasterData;
    QVector<QString> outputDouble;
    QVector<QString> inputDouble;
    QVector<VectorDataDescription> inputVectorData;
    QVector<VectorDataDescription> outputVectorData;
    bool used;
    QString Type;
    QMap<QString, int> parameter;
    QMap<QString, QVariant> paramterValues;
    QMap<QString, QStringList> parameterOptions;
    QVector<QString> unsortedKeys;
    QString HelpUrl;
    bool isGroupNode;

    ModuleDescription() {
        isGroupNode = false;
    }

};


#endif // MODULE_H
