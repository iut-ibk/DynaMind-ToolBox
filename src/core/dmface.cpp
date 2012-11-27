/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich, Markus Sengthaler
 
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

#include <dmcomponent.h>
#include "dmface.h"
#include "dmnode.h"

#include "dmdbconnector.h"
#include <QByteArray>

using namespace DM;


QByteArray GetBytes(std::vector<std::string> stringvector)
{
    QByteArray qba;
    QDataStream stream(&qba, QIODevice::WriteOnly);

    stream << (int)stringvector.size();
    for(unsigned int i=0;i<stringvector.size();i++)
        stream << QString::fromStdString(stringvector[i]);

    return qba;
}
QByteArray GetBytes(std::vector<std::vector<std::string> > stringvectorvector)
{
    QByteArray qba;
    QDataStream stream(&qba, QIODevice::WriteOnly);

    stream << (int)stringvectorvector.size();
    for(unsigned int i=0;i<stringvectorvector.size();i++)
    {
        stream << (int)stringvectorvector[i].size();
        for(unsigned int j=0;j<stringvectorvector[i].size();j++)
                stream << QString::fromStdString(stringvectorvector[i][j]);
    }
    return qba;
}

Face::Face(std::vector<std::string> nodes) : Component(true)
{
    DBConnector::getInstance()->Insert("faces", uuid.toRfc4122(),
                                                QString::fromStdString(stateUuid),
                                       "nodes", GetBytes(nodes));
}
Face::Face(const Face& e) : Component(e, true)
{
    DBConnector::getInstance()->Insert("faces", uuid.toRfc4122(),
                                                QString::fromStdString(stateUuid),
                                       "nodes", GetBytes(e.getNodes()),
                                       "holes", GetBytes(e.getHoles()));
}
Face::~Face()
{
    Component::SQLDelete();
}

std::vector<std::string> GetVector(QByteArray qba)
{
	QDataStream stream(&qba, QIODevice::ReadOnly);
	QString str;
	std::vector<std::string> result;

	unsigned int len=0;
	stream >> len;
	for(unsigned int i=0;i<len;i++)
	{
		stream>>str;
		result.push_back(str.toStdString());
	}
	return result;
}	

std::vector<std::string> Face::getNodes() const
{
    std::vector<std::string> nodes;
    QVariant value;
    if(DBConnector::getInstance()->Select("faces",  uuid.toRfc4122(),
                                                    QString::fromStdString(stateUuid),
                                          "nodes", &value))
        nodes = GetVector(value.toByteArray());
	return nodes;
}

Component* Face::clone()
{
    return new Face(*this);
}

DM::Components Face::getType()
{
	return DM::FACE;
}
QString Face::getTableName()
{
    return "faces";
}
std::vector<std::vector<std::string> > GetVectorVector(QByteArray qba)
{
    QDataStream stream(&qba, QIODevice::ReadWrite);
	std::vector<std::vector<std::string> > result;

	unsigned int len=0;
	stream >> len;
	for(unsigned int i=0;i<len;i++)
	{
		std::vector<std::string> v;
		unsigned int ilen = 0;
		stream >> ilen;
		for(unsigned int i=0;i<ilen;i++)
		{
			QString qstr;
			stream >> qstr;
			v.push_back(qstr.toStdString());
		}
		result.push_back(v);
	}
	return result;
}

const std::vector<std::vector<std::string> > Face::getHoles() const
{
	std::vector<std::vector<std::string> > holes;
    QVariant value;
    if(DBConnector::getInstance()->Select("faces",  uuid.toRfc4122(),
                                                    QString::fromStdString(stateUuid),
                                          "holes", &value))
        holes = GetVectorVector(value.toByteArray());
	return holes;
}

void Face::addHole(std::vector<std::string> hole)
{
	std::vector<std::vector<std::string> > holes = getHoles();
	holes.push_back(hole);
    SQLSetHoles(holes);
}
void Face::addHole(std::vector<DM::Node*> hole)
{
    std::vector<std::string> shole;
    for (std::vector<DM::Node *>::const_iterator it = hole.begin(); it != hole.end(); ++it)
    {
        DM::Node * n = *it;
        shole.push_back(n->getUUID());
    }
    this->addHole(shole);
}

void Face::SQLSetValues(std::vector<std::string> nodes, std::vector<std::vector<std::string> > holes)
{
	SQLSetNodes(nodes);
	SQLSetHoles(holes);
}

void Face::SQLSetNodes(std::vector<std::string> nodes)
{
    DBConnector::getInstance()->Update("faces", uuid.toRfc4122(),
                                                QString::fromStdString(stateUuid),
                                       "nodes", GetBytes(nodes));
}

void Face::SQLSetHoles(std::vector<std::vector<std::string> > holes)
{
    DBConnector::getInstance()->Update("faces", uuid.toRfc4122(),
                                                QString::fromStdString(stateUuid),
                                       "holes", GetBytes(holes));
}

