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
#ifndef GuiSimulationReader_H
#define GuiSimulationReader_H

#include "dmcompilersettings.h"
#include <QVector>
#include "moduledescription.h"
#include "modelnode.h"
#include <QXmlDefaultHandler>

struct DM_HELPER_DLL_EXPORT ModuleExEntry {
	double posX, posY;
	bool minimized;
};

class  DM_HELPER_DLL_EXPORT GuiSimulationReader  : public QXmlDefaultHandler
{
public:
	GuiSimulationReader(QIODevice* source);
	std::map<QString, ModuleExEntry> getEntries(){return entries;};
private:
	bool startElement(const QString & namespaceURI,
		const QString & localName,
		const QString & qName,
		const QXmlAttributes & atts);

	bool endElement(const QString & namespaceURI,
		const QString & localName,
		const QString & qName);

	QString tmpUUID;
	double PosX;
	double PosY;
	bool minimized;

	std::map<QString, ModuleExEntry>	entries;
};

#endif // GuiSimulationReader_H

