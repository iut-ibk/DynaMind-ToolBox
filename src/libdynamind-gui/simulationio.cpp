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
#include "simulationio.h"

#include <QTextStream>
#include <QFile>
#include <iostream>
#include "linknode.h"
#include "guilink.h"
#include <QStringList>
#include <QVariant>
#include <QDataStream>
#include <QByteArray>
#include <map>
#include <groupnode.h>
#include <guisimulation.h>

GuiSimulationReader::GuiSimulationReader(QIODevice* source)
{
	source->open(QIODevice::ReadOnly);
	QXmlSimpleReader r;
    r.setContentHandler(this);
    r.parse(QXmlInputSource(source));
	source->close();
}
/*
void GuiSimulationReader::loadSimluation(QString FileName,  GUISimulation *simulation,  std::map<std::string, std::string> UUIDTranslation)
{
	
    this->sim = simulation;
    this->UUIDTransation = UUIDTranslation;
    QXmlSimpleReader r;
    if (!QFile::exists(FileName)) {
        return;
    }
    QFile f(FileName);
    r.setContentHandler(this);
    r.parse(QXmlInputSource(&f));

    //CreateAllGroups



    int i = 0;
}
*/

bool GuiSimulationReader::startElement(const QString & namespaceURI,
                                const QString & localName,
                                const QString & qName,
                                const QXmlAttributes & atts) 
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)
    if (qName == "GUI_UUID") {
        tmpUUID = atts.value("value");
        return true;
    }
    if (qName == "GUI_PosX") {
        PosX = atts.value("value").toDouble();
        return true;
    }
    if (qName == "GUI_PosY") {
        PosY = atts.value("value").toDouble();
        return true;
    }
    if (qName == "GUI_Minimized") {
        minimized = (bool) atts.value("value").toInt();
        return true;
    }
    return true;

}
bool GuiSimulationReader::endElement(const QString & namespaceURI,
                              const QString & localName,
                              const QString & qName) 
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)
    if (qName == "GUI_Node") 
	{
		ModuleExEntry *e = &entries[tmpUUID];
		e->minimized = minimized;
		e->posX = PosX;
		e->posY = PosY;

        /*DM::Module * DynaMindm;
        DynaMindm = sim->getModuleWithUUID(UUIDTransation[tmpUUID]);
        if (DynaMindm != 0) {
            LoadModule lm;
            lm.minimized = minimized;
            lm.tmpUUID = tmpUUID;
            lm.PosX = PosX;
            lm.PosY = PosY;
            this->modules.push_back(lm);

        }

        return true;*/
    }
    return true;

}
