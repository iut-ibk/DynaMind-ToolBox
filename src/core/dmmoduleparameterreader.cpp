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
#include "dmmoduleparameterreader.h"
#include <QXmlSimpleReader>
#include <iostream>
#include <sstream>

ModuleParameterReader::ModuleParameterReader(QString fileName, QString workingDir) {
    this->workingDir = workingDir.toStdString();
    QXmlSimpleReader r;
    if (!QFile::exists(fileName)) {
        std::cout << "Error: File" << fileName.toStdString() << " not found" << std::endl;
        return;
    }
    QFile f(fileName);
    r.setContentHandler(this);
    r.parse(&f);
}

bool ModuleParameterReader::startElement(const QString & namespaceURI,
                                         const QString & localName,
                                         const QString & qName,
                                         const QXmlAttributes & atts) {
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)

    if (qName == "parameter") {
        //Q_ASSERT(atts.index("name") >= 0);
        ////Q_ASSERT(atts.index("value") >= 0);
        name = atts.value("name").toStdString();
        if (atts.index("value") >= 0) {
            value = atts.value("value").toStdString();
            parameters[name] = value;
        }
        else if (atts.index("file") >= 0) {
            std::stringstream s;
            s << workingDir << "/" << atts.value("file").toStdString();
            value = s.str();
            parameters[name] = value;
        } else {
            value = "";
        }

    }

    return true;
}

bool ModuleParameterReader::endElement(const QString & namespaceURI,
                                       const QString & localName,
                                       const QString & qName) {

    if (qName == "parameter") {
        //Q_ASSERT(name.length() > 0);
        //Q_ASSERT(value.length() > 0);
        value = QString::fromStdString(value).trimmed().toStdString();
        parameters[name] = value;
    }
    return true;
}

bool ModuleParameterReader::characters(const QString & ch) {
    value = value + ch.toStdString();
    return true;
}

typedef std::pair<std::string, std::string> mpair;

std::map<std::string, std::string> ModuleParameterReader::getParameters() const {
    return parameters;
}
