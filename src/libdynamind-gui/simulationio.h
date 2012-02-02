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
#ifndef SIMULATIONIO_H
#define SIMULATIONIO_H

#include "compilersettings.h"
//#include <QMap>
//#include <QString>
#include <QVector>
#include "moduledescription.h"
#include "modelnode.h"
#include <QXmlDefaultHandler>
struct  VIBE_HELPER_DLL_EXPORT LoadLink {
    int FromID;
    int ToID;
    QString FromName;
    QString ToName;
    int Type;
    int Back;
    bool Visibile;
};

class  VIBE_HELPER_DLL_EXPORT SimulationIO  : public QXmlDefaultHandler
{
public:
    SimulationIO();
    //static void writeSimulation(QString FileName, QMap<QString, ModuleDescription> & modules,  QVector<ModelNode * > * mnodes);
    void loadSimluation(QString FileName,  GUISimulation *simulation,  std::map<std::string, std::string> UUIDTranslation, QVector<ModelNode * > * mnodes);
    QVector<ModelNode * > * getModelNodes(){return this->mnodes;}
    QVector<LoadLink> getLinks(){return this->links;}


private:
    //Groups * groups;
    bool startElement(const QString & namespaceURI,
                      const QString & localName,
                      const QString & qName,
                      const QXmlAttributes & atts);

    bool endElement(const QString & namespaceURI,
                      const QString & localName,
                      const QString & qName);


    QVector<ModelNode * > * mnodes;

    QVector<LoadLink> links;
    std::map<std::string, std::string> UUIDTransation;
    GUISimulation * sim;
    std::string tmpUUID;
    double PosX;
    double PosY;
    bool minimized;

};

#endif // SIMULATIONIO_H

