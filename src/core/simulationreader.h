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
#ifndef SIMULATIONREADER_H
#define SIMULATIONREADER_H

#include "compilersettings.h"
#include <QXmlDefaultHandler>
#include <QMap>
#include <QVector>

struct DM_HELPER_DLL_EXPORT ModuleEntry {
    QString ClassName;
    QString UUID;
    QString Name;
    QString GroupUUID;
    QMap<QString, QString> ParemterList;
};

class DM_HELPER_DLL_EXPORT PortEntry {
public:
    QString UUID;
    QString PortName;
    int isTuplePort;

    PortEntry () {
        isTuplePort = -1;
    }

};

struct DM_HELPER_DLL_EXPORT LinkEntry {

    PortEntry InPort;
    PortEntry OutPort;
    bool backlink;

};

struct DM_HELPER_DLL_EXPORT GroupEntry {
    QString id;
    QString desc_file;
    QString name;
};

class DM_HELPER_DLL_EXPORT SimulationReader : QXmlDefaultHandler {
public:
    SimulationReader(const QString &fileName);
    QVector<ModuleEntry> getModules()  {
        return moduleEntries;
    }
    QVector<LinkEntry> getLinks()  {
        return linkEntries;
    }
    QString getRootGroupUUID(){return this->RootUUID;}

private:
    ModuleEntry tmpNode;
    LinkEntry tmpLink;
    QString tmpParameterName;
    QString tmpValue;
    QVector<ModuleEntry> moduleEntries;
    QVector<LinkEntry> linkEntries;
    QString ParentName;
    QString RootUUID;
    long id;
    bool startElement(const QString & namespaceURI,
                      const QString & localName,
                      const QString & qName,
                      const QXmlAttributes & atts);
    bool fatalError(const QXmlParseException & exception);
    bool endElement(const QString & namespaceURI,
                    const QString & localName,
                    const QString & qName);
    bool characters(const QString & ch);
};

#endif // SIMULATIONREADER_H
