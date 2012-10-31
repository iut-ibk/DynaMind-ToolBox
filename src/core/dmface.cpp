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
#include "dmedge.h"

#include "dmdbconnector.h"

using namespace DM;

Face::Face(std::vector<std::string> nodes) : Component()
{
    //this->nodes = nodes;
	SQLInsert(nodes);
}
Face::Face(const Face& e) : Component(e)
{
    //this->nodes=e.nodes;
    //this->holes=e.holes;
	SQLInsert(e.getNodes(),e.getHoles());
}
Face::~Face()
{
	SQLDelete();
}

std::vector<std::string> GetVector(QByteArray &qba)
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
    //return this->nodes;
	std::vector<std::string> nodes;
	QByteArray list;
	QSqlQuery q;
	q.prepare("SELECT nodes FROM faces WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
	{
		list = q.value(0).toByteArray();
		nodes = GetVector(list);
	}
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

std::vector<std::vector<std::string>> GetVectorVector(QByteArray &qba)
{
	QDataStream stream(&qba, QIODevice::ReadWrite);
	QString str;
	std::vector<std::vector<std::string>> result;

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
    //return holes;

	std::vector<std::vector<std::string>> holes;
	QSqlQuery q;
	q.prepare("SELECT holes FROM faces WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())
	{
		holes = GetVectorVector(q.value(0).toByteArray());
	}
	return holes;
}

void Face::addHole(std::vector<std::string> hole)
{
	std::vector<std::vector<std::string>> holes = getHoles();
	holes.push_back(hole);
	SQLSetHoles(holes);
    //this->holes.push_back(hole);
}

void Face::SQLInsert(std::vector<std::string> nodes)
{
	SQLInsertAs("face");
	SQLSetNodes(nodes);
}
void Face::SQLInsert(std::vector<std::string> nodes, std::vector<std::vector<std::string>> holes)
{
	SQLInsertAs("face");
	SQLSetValues(nodes, holes);
}
void Face::SQLDelete()
{
	SQLDeleteAs("face");
}

void Face::SQLSetValues(std::vector<std::string> nodes, std::vector<std::vector<std::string>> holes)
{
	SQLSetNodes(nodes);
	SQLSetHoles(holes);
}

QByteArray GetBytes(std::vector<std::string> stringvector)
{
	QByteArray qba;
	QDataStream stream(&qba, QIODevice::WriteOnly);
	
	stream << stringvector.size();
	for(unsigned int i=0;i<stringvector.size();i++)
		stream << QString::fromStdString(stringvector[i]);
		
	return qba;
}

void Face::SQLSetNodes(std::vector<std::string> nodes)
{
	QSqlQuery q;
	q.prepare("UPDATE faces SET nodes=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(GetBytes(nodes));
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
}

QByteArray GetBytes(std::vector<std::vector<std::string>> stringvectorvector)
{
	QByteArray qba;
	QDataStream stream(&qba, QIODevice::WriteOnly);
	
	stream << stringvectorvector.size();
	for(unsigned int i=0;i<stringvectorvector.size();i++)
	{
		stream << stringvectorvector[i].size();
		for(unsigned int j=0;j<stringvectorvector[i].size();j++)
				stream << QString::fromStdString(stringvectorvector[i][j]);
	}
	return qba;
}

void Face::SQLSetHoles(std::vector<std::vector<std::string>> holes)
{
	QSqlQuery q;
	q.prepare("UPDATE faces SET holes=? WHERE uuid LIKE ? AND stateuuid LIKE ?");
	q.addBindValue(GetBytes(holes));
	q.addBindValue(QString::fromStdString(uuid));
	q.addBindValue(QString::fromStdString(stateUuid));
	if(!q.exec())	PrintSqlError(&q);
}

	
