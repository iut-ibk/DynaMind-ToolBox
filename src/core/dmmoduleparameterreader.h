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
#ifndef DMMODULEPARAMETERREADER_H
#define DMMODULEPARAMETERREADER_H
#include "dmcompilersettings.h"

#include <QXmlDefaultHandler>
#include <map>
#include <string>

class DM_HELPER_DLL_EXPORT ModuleParameterReader : public QXmlDefaultHandler
{
public:
    ModuleParameterReader(QString fileName, QString workingDir);
    virtual ~ModuleParameterReader(){}

    bool startElement(const QString & namespaceURI,
                      const QString & localName,
                      const QString & qName,
                      const QXmlAttributes & atts);
    bool endElement(const QString & namespaceURI,
                    const QString & localName,
                    const QString & qName);

    bool characters(const QString & ch);
    std::map<std::string, std::string> getParameters() const;
private:
    std::map<std::string, std::string> parameters;
    std::string value;
    std::string name;
    std::string workingDir;
};

#endif // MODULEPARAMETERREADER_H
