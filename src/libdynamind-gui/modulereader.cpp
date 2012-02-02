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
#include "modulereader.h"
#include "QXmlSimpleReader"

#include <iostream>
#include <sstream>
#include <QVariant>
#include <QMap>



ModuleReader::ModuleReader(QString fileName)
{
    QXmlSimpleReader r;
    if (!QFile::exists(fileName)) {
        std::cout << "Error: File" << fileName.toStdString() << " not found" << std::endl;
        return;
    }
    QFile f(fileName);
    r.setContentHandler(this);
    r.parse(QXmlInputSource(&f));
}


bool ModuleReader::startElement(const QString & namespaceURI,
                                const QString & localName,
                                const QString & qName,
                                const QXmlAttributes & atts) {
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)
    if (qName == "description")
        return true;
    if (qName == "help") {
        if (atts.index("url") >= 0) {
            m.HelpUrl = atts.value("url");
        }
    }
    if (qName == "module") {
        //Q_ASSERT(atts.index("name") >= 0);
        if (atts.index("name") >= 0) {
            m.name = atts.value("name");
            m.used = false;
        } else {
            value = "";
        }
        if (atts.index("type") >= 0) {
            m.Type = atts.value("type");
        }
        if (atts.index("filename") >= 0) {
            m.filename= atts.value("filename");
        }
        return true;
    }
    if (qName == "parameter") {
        QString name;
        int id;
        QStringList options;
        if (atts.index("name") >= 0) {
            name = atts.value("name");
        }
        if (atts.index("options") >= 0) {
            options = atts.value("options").split(QRegExp("\\s+"));
        }
        if (atts.index("value") >= 0) {
            QString value = atts.value("value");
            if (value.compare("Integer")== 0) {
                id = QVariant::Int;
            }
            if (value.compare("Double")== 0) {
                id = QVariant::Double;
            }
            if (value.compare("Filename") == 0) {
                id = QVariant::String;

            }

            if (value.compare("String") == 0) {
                id = QVariant::String;
            }

            if (value.compare("Points") == 0) {
                id = QVariant::UserType+1;
            }
            if (value.compare("UserDefinedInputRasterData") == 0) {
                id = QVariant::UserType+2;
            }
            if (value.compare("UserDefinedOutputRasterData") == 0) {
                id = QVariant::UserType+3;
            }
            if (value.compare("UserDefinedFormula") == 0) {
                id = QVariant::UserType+4;
            }
            if (value.compare("StringList") == 0) {
                id = QVariant::UserType+5;
            }
            if (value.compare("Bool") == 0) {
                id = QVariant::Bool;
            }
            if (value.compare("FilenameIn") == 0) {
                id = QVariant::UserType+6;

            }
            if (value.compare("UserDefinedInputVectorData") == 0) {
                id = QVariant::UserType+7;
            }
            if (value.compare("UserDefinedOutputVectorData") == 0) {
                id = QVariant::UserType+8;
            }


        }
        QVariant va;
        if (atts.index("default") >= 0) {
            if (id == QVariant::Int) {
                va = QVariant( atts.value("default").toInt());
            }
            if (id == QVariant::Double){
                va =QVariant( atts.value("default").toDouble());
            }
            if (id == QVariant::String){
                va =QVariant( atts.value("default"));
            }
            if (id == QVariant::Bool){
                QVariant v(false);
                QString s = atts.value("default");
                if (s.compare("true") == 0) {
                    v = true;
                }

                va = v;
            }
            if (id == QVariant::UserType+6){
                va =QVariant( atts.value("default"));
            }

            if (id == QVariant::UserType+1){
                Points p;
                QStringList list = atts.value("default").split(QRegExp("\\s+"));
                p.VectorDataName =  list[0];
                if (list.size() > 1) {
                    p.PointsName = list[1];
                }
                va.setValue(p);
            }
            if (id == QVariant::UserType+2){
                QStringList l;
                va.setValue(l);
            }
            if (id == QVariant::UserType+3){
                QStringList l;
                va.setValue(l);
            }
            if (id == QVariant::UserType+4){
                QMap<QString, QVariant> l;
                va.setValue(l);
            }
            if (id == QVariant::UserType+5){
                QMap<QString, QVariant> map;

                QStringList list = atts.value("default").split(QRegExp("\\s+"));
                if (list.size() > 1) {
                    foreach (QString s, list) {
                        map[s] = false;
                    }
                }
                va.setValue(map);
            }
            if (id == QVariant::UserType+7){
                QStringList l;
                va.setValue(l);
            }
            if (id == QVariant::UserType+8){
                QStringList l;
                va.setValue(l);
            }

        }



        m.parameter.insert(name, id);
        m.paramterValues.insert(name, va);
        m.parameterOptions.insert(name, options);
        m.unsortedKeys.push_back(name);
        return true;

    }
    if (qName == "input") {

        if (atts.index("RasterData") >= 0) {
            m.inputRasterData.append(atts.value("RasterData"));
        }
        if (atts.index("DoubleData") >= 0) {
            m.inputDouble.append(atts.value("DoubleData"));
        }
        if (atts.index("VectorData") >= 0) {
            VectorDataDescription vdes;
            vdes.Name = atts.value("VectorData");
            if (atts.index("points") >= 0) {
                vdes.Points = atts.value("points").split(QRegExp("\\s+"));
            }
            if (atts.index("edges") >= 0) {
                vdes.Edges = atts.value("edges").split(QRegExp("\\s+"));
            }
            if (atts.index("faces") >= 0) {
                vdes.Edges = atts.value("faces").split(QRegExp("\\s+"));
            }
            if (atts.index("links") >= 0) {
                vdes.Links = atts.value("links").split(QRegExp("\\s+"));
            }
            if (atts.index("attributes") >= 0) {
                vdes.Attributes = atts.value("attributes").split(QRegExp("\\s+"));
            }
            m.inputVectorData.append(vdes);
        }
        return true;

    }

    if (qName == "output") {

        if (atts.index("RasterData") >= 0) {
            m.outputRasterData.append(atts.value("RasterData"));
        }
        if (atts.index("DoubleData") >= 0) {
            m.outputDouble.append(atts.value("DoubleData"));
        }
        if (atts.index("VectorData") >= 0) {
            VectorDataDescription vdes;
            vdes.Name = atts.value("VectorData");
            if (atts.index("points") >= 0) {
                vdes.Points = atts.value("points").split(QRegExp("\\s+"));
            }
            if (atts.index("edges") >= 0) {
                vdes.Edges = atts.value("edges").split(QRegExp("\\s+"));
            }
            if (atts.index("faces") >= 0) {
                vdes.Edges = atts.value("faces").split(QRegExp("\\s+"));
            }
            if (atts.index("links") >= 0) {
                vdes.Links = atts.value("links").split(QRegExp("\\s+"));
            }
            if (atts.index("attributes") >= 0) {
                vdes.Attributes = atts.value("attributes").split(QRegExp("\\s+"));
            }
            m.outputVectorData.append(vdes);
        }

        return true;

    }
    return false;

}



