/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair, Markus Sengthaler

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

#include <dmmodule.h>
#include <dmcomponent.h>
#include <dmattribute.h>
#include <QVariant>
#include "dmdbconnector.h"

using namespace DM;

QByteArray GetBinaryValue(std::vector<double> v)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << QVector<double>::fromStdVector(v);
	return bytes;
}

QByteArray GetBinaryValue(std::vector<std::string> v)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << QVector<QString>::fromStdVector(DBConnector::ToStdString(v));
	return bytes;
}
QByteArray GetBinaryValue(std::vector<LinkAttribute> v)
{
	QByteArray bytes;
	QDataStream s(&bytes, QIODevice::WriteOnly);
	s << (int)v.size();
	foreach(LinkAttribute it, v)
	{
		s << QString::fromStdString(it.uuid);
		s << QString::fromStdString(it.viewname);
	}
	return bytes;
}

std::vector<LinkAttribute> ToLinkVector(QVariant q)
{
	std::vector<LinkAttribute> result;
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	int size;
	s >> size;
	QString uuid;
	QString view;
	LinkAttribute la;
	for(int i=0;i<size;i++)
	{
		s >> uuid;
		s >> view;
		la.uuid = uuid.toStdString();
		la.viewname = view.toStdString();
		result.push_back(la);
	}
	return result;
}

std::vector<double> ToDoubleVector(QVariant q)
{
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	QVector<double> v;
	s >> v;
	return v.toStdVector();
}

std::vector<std::string> ToStringVector(QVariant q)
{
	QByteArray bytes = q.toByteArray();
	QDataStream s(&bytes, QIODevice::ReadOnly);
	QVector<QString> qv;
	s >> qv;
	std::vector<std::string> sv;
	foreach(QString qstr, qv)
		sv.push_back(qstr.toStdString());

	return sv;
}

Attribute::Attribute()
{
    this->name="";
    //this->type = Attribute::NOTYPE;

	DBConnector::getInstance();
	SQLInsertThis(NOTYPE);
}
Attribute::Attribute(const Attribute &newattribute)
{
    this->name=newattribute.name;
    //this->doublevalue=newattribute.doublevalue;
    //this->stringvalue=newattribute.stringvalue;
    //this->doublevector=newattribute.doublevector;
    //this->stringvector=newattribute.stringvector;
    //this->type = newattribute.type;
	SQLInsertThis(newattribute.getType());
}

Attribute::Attribute(std::string name)
{
    this->name=name;
    //this->type = Attribute::NOTYPE;
	SQLInsertThis(NOTYPE);
}


Attribute::Attribute(std::string name, double val)
{
    //this->doublevalue = val;
    this->name=name;

    //this->type = Attribute::DOUBLE;
	SQLInsertThis(DOUBLE);
	this->setDouble(val);
}
Attribute::Attribute(std::string name, std::string val)
{
    //this->stringvalue = val;
    this->name=name;

    //this->type = Attribute::STRING;
	SQLInsertThis(STRING);
	setString(val);
}

Attribute::~Attribute()
{
	SQLDeleteThis();
}

Attribute::AttributeType Attribute::getType() const
{
	QSqlQuery q;
	q.prepare("SELECT type FROM attributes WHERE uuid=?");
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
	if(q.next())	return (AttributeType)q.value(0).toInt();
    return NOTYPE;
}

void Attribute::setName(std::string name)
{
    this->name=name;
	SQLSetName(name);
}

std::string Attribute::getName()
{
    return name;
}

void Attribute::setDouble(double v)
{
    //doublevalue=v;
    //this->type = Attribute::DOUBLE;
	SQLSetValue(DOUBLE, v);
}

double Attribute::getDouble()
{
    //return doublevalue;
	QVariant value;
	if(SQLGetValue(value))	return value.toDouble();
	return 0;
}

void Attribute::setString(std::string s)
{
    //stringvalue=s;
    //this->type = Attribute::STRING;
	SQLSetValue(STRING, QString::fromStdString(s));
}

std::string Attribute::getString()
{
    //return stringvalue;
	QVariant value;
	if(SQLGetValue(value))	return value.toString().toStdString();
	return "";
}

void Attribute::setDoubleVector(std::vector<double> v)
{
    //doublevector=v;
    //this->type = Attribute::DOUBLEVECTOR;
	SQLSetValue(DOUBLEVECTOR, GetBinaryValue(v));
}

std::vector<double> Attribute::getDoubleVector()
{
    //return doublevector;
	QVariant value;
	if(SQLGetValue(value))	return ToDoubleVector(value);
	return std::vector<double>();
}

void Attribute::setStringVector(std::vector<std::string> s)
{
    //stringvector=s;
    //this->type = Attribute::STRINGVECTOR;
	SQLSetValue(STRINGVECTOR, GetBinaryValue(s));
}

std::vector<std::string> Attribute::getStringVector()
{
    //return stringvector;
	QVariant value;
	if(SQLGetValue(value))	return ToStringVector(value);
	return std::vector<string>();
}

bool Attribute::hasDouble()
{
    if (getType() == Attribute::DOUBLE)
        return true;
    return false;
}
bool Attribute::hasDoubleVector()
{
    if (getType() == Attribute::DOUBLEVECTOR)
        return true;
    return false;
}
bool Attribute::hasString()
{
    if (getType() == Attribute::STRING)
        return true;
    return false;
}
bool Attribute::hasStringVector()
{
    if (getType() == Attribute::STRINGVECTOR)
        return true;
    return false;
}

void Attribute::setLink(string viewname, string uuid)
{
	/*
    this->type = Attribute::LINK;
    this->stringvector.push_back(viewname);
    this->stringvector.push_back(uuid);*/
	std::vector<LinkAttribute> links = getLinks();
	LinkAttribute att;
	att.uuid = uuid;
	att.viewname = viewname;
    links.push_back(att);
	setLinks(links);
}

void Attribute::setLinks(std::vector<LinkAttribute> links)
{
    //this->type = Attribute::LINK;
    /*this->stringvector.clear();
    foreach (LinkAttribute attr, links) {
        this->stringvector.push_back(attr.viewname);
        this->stringvector.push_back(attr.uuid);
    }*/
	SQLSetValue(LINK, GetBinaryValue(links));
}

LinkAttribute Attribute::getLink()
{
	QVariant value;
	if(SQLGetValue(value))
	{
		std::vector<LinkAttribute> linkvector = ToLinkVector(value);
		if(linkvector.size())
			return linkvector[0];
		/*
		LinkAttribute attr;
		if (stringvector.size() < 2) 
		{
			Logger(Warning) << "Couldn't find link " << this->getName();
			return LinkAttribute();
		}
		attr.viewname = stringvector[0];
		attr.uuid = stringvector[1];
		return attr;
		*/
	}
	return LinkAttribute();
}

std::vector<LinkAttribute> Attribute::getLinks()
{
	std::vector<LinkAttribute> linkvector;

	QVariant value;
	if(SQLGetValue(value))
	{
		linkvector = ToLinkVector(value);
		/*
		for (unsigned int i = 0; i < stringvector.size(); i+=2) {
			LinkAttribute attr;
			attr.viewname = stringvector[i];
			attr.uuid = stringvector[i+1];
			links.push_back(attr);
		}*/
	}
    return linkvector;
}

void Attribute::addTimeSeries(std::vector<std::string> timestamp, std::vector<double> value)
{
    if(timestamp.size()!=value.size())
    {
        DM::Logger(DM::Error) << "Length of time and value vector are not equal";
        return;
    }
	/*
    this->type = Attribute::TIMESERIES;
    this->doublevector = value;
    this->stringvector = timestamp;
	SQLUpdateValue();*/
	QByteArray qba;
	QDataStream s(&qba, QIODevice::WriteOnly);
	s << (int)value.size();
	for(unsigned int i=0;i<value.size();i++)
	{
		s << value[i];
		s << QString::fromStdString(timestamp[i]);
	}
	SQLSetValue(LINK, qba);
}

void Attribute::getTimeSeries(std::vector<std::string> *timestamp, std::vector<double> *value)
{
	QVariant data;
	SQLGetValue(data);
	QByteArray qba = data.toByteArray();
	QDataStream s(&qba, QIODevice::ReadOnly);
	int size;
	s >> size;

	if(size==0)
	{
        DM::Logger(DM::Error) << "timestamp or value are equal null";
        return;
	}
	
	QVector<double> qdoublevector;
	QVector<QString> qstringvector;
	
	double dbl;
	QString str;
	for(int i=0;i<size;i++)
	{
		s >> dbl;
		s >> str;
		value->push_back(dbl);
		timestamp->push_back(str.toStdString());
	}
	/*
	if(!timestamp || !value)
	{
        DM::Logger(DM::Error) << "timestamp or value are equal null";
        return;
	}*/
	//value = doublevector;
	//*timestamp = stringvector;
}

void Attribute::setType(AttributeType type)
{
	SQLSetType(type);
    //this->type = type;
}

const char *Attribute::getTypeName() const
{
	return Attribute::getTypeName(this->getType());
}

const char *Attribute::getTypeName(Attribute::AttributeType type)
{
    const char *arr[] = {
        "NOTYPE",
        "DOUBLE",
        "STRING",
        "TIMESERIES",
        "LINK",
        "DOUBLEVECTOR",
        "STRINGVECTOR"
    };
    return arr[type];
}

/*
QByteArray Attribute::getValue(AttributeType type)
{
	QByteArray bytes;

	QDataStream s(&bytes, QIODevice::WriteOnly);
	switch(type)
	{
	case NOTYPE:
		break;
	case DOUBLE:
		s << doublevalue;
		break;
	case STRING:
		s << QString::fromStdString(stringvalue);
		break;
	case DOUBLEVECTOR:
		s << QVector<double>::fromStdVector(doublevector);
		break;
	case STRINGVECTOR:
		s << QVector<QString>::fromStdVector(DBConnector::ToStdString(stringvector));
		break;
	case TIMESERIES:
		// TODO order?
		s << QVector<double>::fromStdVector(doublevector);
		s << QVector<QString>::fromStdVector(DBConnector::ToStdString(stringvector));
		break;
	case LINK:	// TODO
		break;
	}
	return bytes;
}
void Attribute::setValue(QByteArray bytes)
{
	switch(getType())
	{
	case NOTYPE:
		break;
	case DOUBLE:
		doublevalue = bytes.toDouble();
		break;
	case STRING:
		stringvalue = QString(bytes).toStdString();
		break;
	case DOUBLEVECTOR:
		doublevector = DBConnector::GetDoubleVector(bytes);
		break;
	case STRINGVECTOR:
		stringvector = DBConnector::GetStringVector(bytes);
		break;
	case TIMESERIES:
		doublevector = DBConnector::GetDoubleVector(bytes);
		stringvector = DBConnector::GetStringVector(bytes);
		break;
	case LINK:	// TODO
		break;
	}
}*/
void Attribute::SQLInsertThis(AttributeType type)
{
	_uuid = QUuid::createUuid().toString().toStdString();
	QSqlQuery q;
	q.prepare("INSERT INTO attributes (uuid,name,type) VALUES (?,?,?)");
	q.addBindValue(QString::fromStdString(_uuid));
	q.addBindValue(QString::fromStdString(name));
	q.addBindValue((int)type);
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SQLDeleteThis()
{	
	QSqlQuery q;
	q.prepare("DELETE FROM attributes WHERE uuid=?");
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}

void Attribute::SQLUpdateValue(QByteArray qba)
{	
	QSqlQuery q;
	q.prepare("UPDATE attributes SET value=? WHERE uuid=?");
	q.addBindValue(qba);
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SetOwner(Component* owner)
{
	// TODO: make shure its not bound to another component
	SQLSetOwner(owner);
}
void Attribute::SQLSetOwner(Component* owner)
{
	QSqlQuery q;
	q.prepare("UPDATE attributes SET owner=?,stateuuid=? WHERE uuid=?");
	q.addBindValue(QString::fromStdString(owner->getUUID()));
	q.addBindValue(QString::fromStdString(owner->getStateUUID()));
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SQLSetName(std::string newname)
{	
	QSqlQuery q;
	q.prepare("UPDATE attributes SET name=? WHERE uuid=?");
	q.addBindValue(QString::fromStdString(newname));
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
void Attribute::SQLSetType(AttributeType newtype)
{
	QSqlQuery q;
	q.prepare("UPDATE attributes SET type=?,value=NULL WHERE uuid=?");
	q.addBindValue(newtype);
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}

bool Attribute::SQLGetValue(QVariant &value)
{
	QSqlQuery q;
	q.prepare("SELECT value FROM attributes WHERE uuid=?");
	q.addBindValue(QString::fromStdString(_uuid));
	if(q.exec())	
	{
		if(q.next())	
		{
			value = q.value(0);
			return true;
		}
	}
	else
		PrintSqlError(&q);
	return false;
}
void Attribute::SQLSetValue(AttributeType type, QVariant value)
{
	QSqlQuery q;
	q.prepare("UPDATE attributes SET type=?,value=? WHERE uuid=?");
	q.addBindValue((int)type);
	q.addBindValue(value);
	q.addBindValue(QString::fromStdString(_uuid));
	if(!q.exec())	PrintSqlError(&q);
}
