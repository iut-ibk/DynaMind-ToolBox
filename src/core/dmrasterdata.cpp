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
#include "dmrasterdata.h"
#include <QPointF>
#include <time.h>
#include <QMutex>
#include <dmlogger.h>
#include <math.h>
#include "dmdbconnector.h"
#include <QVariant>
#include <QSqlQuery>

using namespace DM;

const unsigned int rowOverlapp = 3;
static Cache<std::pair<QUuid,long>,QByteArray> rowCache(4096);
static std::map<std::pair<QUuid,long>,QByteArray*> rowUpdateCache;

#ifdef CACHE_PROFILING
void RasterData::PrintStatistics()
{
    Logger(Standard) << "Rasterdata row cache statistics:\t"
                     << "misses: " << (long)rowCache.misses
                     << "\thits: " << (long)rowCache.hits;
    rowCache.ResetProfilingCounters();
}
#endif


RasterData::RasterData(long width, long height,
                       double cellsizeX, double cellsizeY,
                       double xoffset, double yoffset)
    : Component(true)
{
    this->width = width;
    this->height = height;
    this->cellSizeX = cellsizeX;
    this->cellSizeY = cellsizeY;
    this->xoffset = xoffset;
    this->yoffset = yoffset;

    this->minValue = -9999;
    this->maxValue = -9999;
    this->NoValue = -9999;
    this->debugValue = 0;

	SQLInsert();
    SQLInsertField(width, height);
}
RasterData::RasterData() : Component(true)
{
    this->cellSizeX = 0;
    this->cellSizeY = 0;
    this->xoffset = 0;
    this->yoffset = 0;
    this->width = 0;
    this->height = 0;

    SQLInsert();
}
RasterData::RasterData(const RasterData &other) : Component(other, true)
{
    this->width = other.width;
    this->height = other.height;
    this->NoValue = other.NoValue;
    this->minValue = other.minValue;
    this->maxValue = other.maxValue;
    this->debugValue = other.debugValue;

    this->cellSizeX = other.cellSizeX;
    this->cellSizeY = other.cellSizeY;
    this->xoffset = other.xoffset;
    this->yoffset = other.yoffset;

	SQLInsert();
    SQLInsertField(width, height);

    for(long y=0;y<height;y++)
        this->SQLSetRow(y, other.SQLGetRow(y));
}

Components RasterData::getType()
{
	return DM::RASTERDATA;
}
QString RasterData::getTableName()
{
    return "rasterdatas";
}
double RasterData::getSum() const
{
    double sum = 0;
    for ( long i = 0; i < width; i++ )
	{
        for (  long j = 0; j < height; j++ ) 
        {
            double val = SQLGetValue(i,j);
            if ( val != NoValue)
                sum += val;
        }
    }
    return sum;
}

double RasterData::getCell(long x, long y) const
{
    if (  x >-1 && y >-1 && x < this->width && y < this->height) 
        return  SQLGetValue(x,y);
    else
        return  this->NoValue;
}

bool RasterData::setCell(long x, long y, double value)
{
    if (  x >-1 && y >-1 && x < this->width && y < this->height) 
	{
		SQLSetValue(x,y,value);

        if (minValue == this->NoValue || minValue > value)
            minValue = value;

        if (maxValue == this->NoValue || maxValue < value)
            maxValue = value;

        return true;
    } 
    return false;
}

RasterData::~RasterData()
{
    SQLDeleteField();
    Component::SQLDelete();
}

void RasterData::getNeighboorhood(double** d, int width, int height, int x, int y) {
    int dx = (int) (width -1)/2;
    int dy = (int) (height -1)/2;
    int x_cell;
    int y_cell;

    int k = 0;
    for ( long i = x-dx; i <= x + dx; i++ ) {
        x_cell = i;
        if ( i < 0) {
            x_cell = this->width + i;
        }
        if ( i >= this->width) {
            x_cell = i - this->width;
        }
        int l = 0;
        for ( long j = y-dy; j <= y + dy; j++ ) {
            y_cell = j;
            if ( j < 0) {
                y_cell = this->height + j;
            }
            if ( j >= this->height) {
                y_cell = j - this->height;
            }
            d[k][l] = this->getCell(x_cell,y_cell);

            l++;
        }
        k++;
    }
}
void RasterData::getMoorNeighbourhood(std::vector<double> &neigh, long x, long y) {
    int counter = 0;
    for ( long j = y-1; j <= y + 1; j++ ) {
        for ( long i = x-1; i <= x + 1; i++ ) {

            neigh[counter] = this->getCell(i,j);
            counter++;
        }
    }
    if ( x-1 < 0 && y-1 < 0) {
        neigh[0] = neigh[8];
        neigh[1] = neigh[7];
        neigh[2] = neigh[8];
        neigh[3] = neigh[5];
        neigh[6] = neigh[8];

    } else if ( x+1 >=  this->width && y-1 < 0) {
        neigh[0] = neigh[6];
        neigh[1] = neigh[7];
        neigh[2] = neigh[6];
        neigh[5] = neigh[3];
        neigh[8] = neigh[6];

    } else if ( x+1 >= this->width && y+1 >= this->height) {
        neigh[2] = neigh[0];
        neigh[5] = neigh[3];
        neigh[8] = neigh[0];
        neigh[7] = neigh[1];
        neigh[6] = neigh[0];

    } else if ( x-1 < 0 && y+1 >=  this->height) {
        neigh[0] = neigh[2];
        neigh[3] = neigh[5];
        neigh[6] = neigh[2];
        neigh[7] = neigh[1];
        neigh[8] = neigh[2];

    } else if ( x-1 < 0 ) {
        neigh[0] = neigh[2];
        neigh[3] = neigh[5];
        neigh[6] = neigh[8];

    } else if ( y-1 < 0 ) {
        neigh[0] = neigh[6];
        neigh[1] = neigh[7];
        neigh[2] = neigh[8];

    } else if (x+1 >= this->width ) {
        neigh[2] = neigh[0];
        neigh[5] = neigh[3];
        neigh[8] = neigh[6];

    }
    else if (y+1 >= this->height ) {
        neigh[6] = neigh[0];
        neigh[7] = neigh[1];
        neigh[8] = neigh[2];

    }

}
std::vector<double>  RasterData::getMoorNeighbourhood(long x, long y) const {
    std::vector<double> neigh(9);
    int counter = 0;
    for ( long j = y-1; j <= y + 1; j++ ) {
        for ( long i = x-1; i <= x + 1; i++ ) {

            neigh[counter] = this->getCell(i,j);
            counter++;
        }
    }
    if ( x-1 < 0 && y-1 < 0) {
        neigh[0] = neigh[8];
        neigh[1] = neigh[7];
        neigh[2] = neigh[8];
        neigh[3] = neigh[5];
        neigh[6] = neigh[8];

    } else if ( x+1 >=  this->width && y-1 < 0) {
        neigh[0] = neigh[6];
        neigh[1] = neigh[7];
        neigh[2] = neigh[6];
        neigh[5] = neigh[3];
        neigh[8] = neigh[6];

    } else if ( x+1 >= this->width && y+1 >= this->height) {
        neigh[2] = neigh[0];
        neigh[5] = neigh[3];
        neigh[8] = neigh[0];
        neigh[7] = neigh[1];
        neigh[6] = neigh[0];

    } else if ( x-1 < 0 && y+1 >=  this->height) {
        neigh[0] = neigh[2];
        neigh[3] = neigh[5];
        neigh[6] = neigh[2];
        neigh[7] = neigh[1];
        neigh[8] = neigh[2];

    } else if ( x-1 < 0 ) {
        neigh[0] = neigh[2];
        neigh[3] = neigh[5];
        neigh[6] = neigh[8];

    } else if ( y-1 < 0 ) {
        neigh[0] = neigh[6];
        neigh[1] = neigh[7];
        neigh[2] = neigh[8];

    } else if (x+1 >= this->width ) {
        neigh[2] = neigh[0];
        neigh[5] = neigh[3];
        neigh[8] = neigh[6];

    }
    else if (y+1 >= this->height ) {
        neigh[6] = neigh[0];
        neigh[7] = neigh[1];
        neigh[8] = neigh[2];

    }
    return neigh;

}

void RasterData::setSize(long width, long height, double cellsizeX, double cellsizeY, double xoffset, double yoffset) {
    if (width != this->width || height != this->height || this->cellSizeX != cellsizeX || this->cellSizeY != cellsizeY)
    {
        if(width != 0 || height != 0)
            SQLDeleteField();

        this->width = width;
        this->height = height;
        this->cellSizeX = cellsizeX;
        this->cellSizeY = cellsizeY;
        this->xoffset = xoffset;
        this->yoffset = yoffset;
        this->NoValue = -9999;
        this->minValue = -9999;
        this->maxValue = -9999;

        SQLInsertField(width, height);
    }
}

void RasterData::clear() {
    for (int y = 0; y < this->height; y++)
        for (int x = 0; x < this->width; x++)
            SQLSetValue(x,y,0);
}

Component * RasterData::clone() {
    return new RasterData(*this);
}

void RasterData::SQLInsert()
{
    DBConnector::getInstance()->Insert("rasterdatas", uuid);
}


bool RasterData::setValue(long x, long y, double value)
{
    return setCell((int)((x-xoffset)/cellSizeX),(int)((y-yoffset)/cellSizeY),value);
}

double RasterData::getValue(long x, long y) const
{
    return getCell((int)((x-xoffset)/cellSizeX),(int)((y-yoffset)/cellSizeY));
}

void RasterData::SQLInsertField(long width, long height)
{
	if(width == 0 || height == 0)
        return;

    double* buffer = new double[width];
    for(long y = 0; y < height; y++)
    {
        for(long x = 0; x < width; x++)
            buffer[x] = NoValue;
        QByteArray *qba = new QByteArray((char*)buffer, sizeof(double)*width);
		rowCache.add(std::pair<QUuid,long>(getQUUID(),y), qba);

        QSqlQuery *q = DBConnector::getInstance()->getQuery("INSERT INTO rasterfields(owner,y,data) VALUES (?,?,?)");
        q->addBindValue(uuid.toByteArray());
        q->addBindValue(QVariant::fromValue(y));
        q->addBindValue(qba->toBase64());
        DBConnector::getInstance()->ExecuteQuery(q);
    }
    delete buffer;
}
void RasterData::SQLDeleteField()
{
    if(width==0 || height==0)
        return;

    for(int y=0;y<height;y++)
    {
         rowCache.remove(std::pair<QUuid,long>(uuid,y));
         rowUpdateCache.erase(std::pair<QUuid,long>(uuid,y));
    }

    QSqlQuery *q = DBConnector::getInstance()->getQuery("DELETE FROM rasterfields WHERE owner LIKE ?");
    q->addBindValue(uuid.toByteArray());
    DBConnector::getInstance()->ExecuteQuery(q);
}

double RasterData::SQLGetValue(long x, long y) const
{
    QByteArray *qba = SQLGetRow(y);
	return ((double*)qba->data())[x];
}
QByteArray* RasterData::SQLGetRow(long y) const
{
    QByteArray* qba = rowCache.get(std::pair<QUuid,long>(uuid,y));
    if(qba != NULL)
        return qba;

    ForceUpdate();

    // get row+overlapp
    for(long i=1; i<=rowOverlapp && y-i>=0 && y-i<height; i++)
        SQLForceGetRow(y-i);
    for(long i=1; i<=rowOverlapp && y+i>=0 && y+i<height; i++)
        SQLForceGetRow(y+i);

    return SQLForceGetRow(y);
}
QByteArray* RasterData::SQLForceGetRow(long y) const
{
    QSqlQuery *q = DBConnector::getInstance()->getQuery("SELECT data FROM rasterfields WHERE owner LIKE ? AND y=?");
    q->addBindValue(uuid.toByteArray());
    q->addBindValue(QVariant::fromValue(y));
    if(!DBConnector::getInstance()->ExecuteSelectQuery(q))
        return NULL;

    QByteArray* qba = new QByteArray(QByteArray::fromBase64(q->value(0).toByteArray()));
    rowCache.add(std::pair<QUuid,long>(uuid,y),qba);
    return qba;
}


void RasterData::SQLSetValue(long x, long y, double value)
{
    QByteArray* qba = SQLGetRow(y);
    //((double*)qba.data_ptr())[x] = value;
    ((double*)qba->data())[x] = value;
    SQLSetRow(y,qba);
}
void RasterData::SQLSetRow(long y, QByteArray *data)
{
    rowUpdateCache[std::pair<QUuid,long>(uuid,y)] = data;
}

void ApplyRowUpdates()
{
    for(std::map<std::pair<QUuid,long>,QByteArray*>::const_iterator it = rowUpdateCache.begin();
        it != rowUpdateCache.end();
        ++it)
    {
        QSqlQuery *q = DBConnector::getInstance()->getQuery("UPDATE rasterfields SET data = ? WHERE owner LIKE ? AND y=?");
        q->addBindValue(it->second->toBase64());
        q->addBindValue(it->first.first.toByteArray());
        q->addBindValue(QVariant::fromValue(it->first.second));
        DBConnector::getInstance()->ExecuteQuery(q);
    }
    rowUpdateCache.clear();
}

void RasterData::ForceUpdate() const
{
    ApplyRowUpdates();
}

