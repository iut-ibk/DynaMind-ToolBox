/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Markus Sengthaler

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

/*
"create table systems(uuid varchar(100), stateuuid varchar(100), predecessors text, sucessors text, PRIMARY KEY (uuid))"
"create table components(uuid varchar(100), stateuuid varchar(100), owner varchar(100), name text, type tinyint, value text, PRIMARY KEY (uuid))"
"create table attributes(owner varchar(100), stateuuid varchar(100), name text, type tinyint, value text, PRIMARY KEY (uuid,name))"
*/


#ifndef DMDBCONNECTOR_H
#define DMDBCONNECTOR_H

#include <dmsystem.h>
#include <QSqlQuery>

namespace DM {

class DBConnector
{
private:
	static DBConnector* instance;
	DBConnector();
	
	static int _linkID;

	/*
	void saveSystem(System *sys);
	void saveComponent(Component *comp);
	void saveAttribute(Attribute *att, std::string uuid);

	
	System* loadSystem(std::string uuid);
	Component* loadComponent(std::string uuid);
	Attribute* loadAttribute(std::string uuid);*/
public:
	static DBConnector* getInstance();
	void beginTransaction();
	void endTransaction();

	static std::vector<std::string> GetStringVector(QByteArray qba);
	static std::vector<QString> ToStdString(std::vector<std::string> v);
	static std::vector<double> GetDoubleVector(QByteArray qba);

	static QStringList GetStringList(std::vector<DM::Component*> v);
	//static QStringList GetStringList(std::vector<DM::System*> v);

	static int GetNewLinkID();
};

		/*
		QVector<QString> ConvertStringVector(std::vector<std::string> v);
		std::vector<std::string> ConvertStringVector(QVector<QString> v);
		std::list<std::string> ConvertStringVector(QStringList qsl);
		
		int ComponentsToInt(Components type);
		Components IntToComponents(int type);
		*/
		

void PrintSqlError(QSqlQuery *q);

template <typename M> void FreeMap( M & amap ) 
{
    for( typename M::iterator it = amap.begin(); it != amap.end(); ++it ) 
	{
        delete it->second;
		it->second = NULL;
    }
    amap.clear();
}
template <typename M> void FreeVector( M &amap ) 
{
	for(int i=0;i<amap.size();i++)
	{
		delete amap[i];
		amap[i] = NULL;
	}
	amap.Clear();
}
class Converter
{
public:
	static QByteArray GetBytes(std::vector<std::string> stringvector)
	{
		QByteArray qba;
		QDataStream stream(&qba, QIODevice::WriteOnly);
	
		stream << stringvector.size();
		for(unsigned int i=0;i<stringvector.size();i++)
			stream << QString::fromStdString(stringvector[i]);
		
		return qba;
	}

	static QByteArray GetBytes(std::vector<std::vector<std::string>> stringvectorvector)
	{
		QByteArray qba;
		QDataStream stream(&qba, QIODevice::WriteOnly);
	
		stream << stringvectorvector.size();
		for(unsigned int i=0;i<stringvectorvector.size();i++)
		{
			stream << stringvectorvector[i].size();
			for(unsigned int j=0;j<stringvectorvector[i].size();j++)
			{
				stream << QString::fromStdString(stringvectorvector[i][j]);
			}
		}
		
		return qba;
	}

	static std::vector<std::string> GetVector(QByteArray &qba)
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
	
	static std::vector<std::vector<std::string>> GetVectorVector(QByteArray &qba)
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

};
}


#endif
