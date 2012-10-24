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

	
}


#endif
