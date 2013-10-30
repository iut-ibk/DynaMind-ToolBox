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

	cache = NULL;

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

	cache = NULL;
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

	cache = NULL;
	SQLInsert();
	SQLInsertField(width, height);

	SQLCopyField(&other);
}

Components RasterData::getType() const
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
	QMutexLocker ml(&mutex);

	if (  x >-1 && y >-1 && x < this->width && y < this->height) 
	{
		SQLSetValue(x,y,value);

		if (this->NoValue == value) return true;

		if (minValue == this->NoValue || minValue > value)
			minValue = value;

		if (maxValue == this->NoValue || maxValue < value)
			maxValue = value;

		return true;
	} 
	return false;
}

double RasterData::getCellSize() const
{
	Logger(Warning) << "getCellSize is deprecated use getCellSizeX and getCellSizeY";
	return cellSizeX;
}

RasterData::~RasterData()
{
	SQLDeleteField();
	Component::SQLDelete();
}

void RasterData::getNeighboorhood(double** d, int width, int height, int x, int y) 
{
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
void RasterData::getMoorNeighbourhood(std::vector<double> &neigh, long x, long y) 
{
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
std::vector<double>  RasterData::getMoorNeighbourhood(long x, long y) const 
{
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

void RasterData::setSize(long width, long height, 
						 double cellsizeX, double cellsizeY, 
						 double xoffset, double yoffset) 
{
	if (width != this->width || height != this->height 
		|| this->cellSizeX != cellsizeX 
		|| this->cellSizeY != cellsizeY)
	{
		QMutexLocker ml(&mutex);

		if(this->width != 0 || this->height != 0)
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

void RasterData::clear() 
{
	QMutexLocker ml(&mutex);

	for (int y = 0; y < this->height; y++)
		for (int x = 0; x < this->width; x++)
			SQLSetValue(x,y,NoValue);
}

Component * RasterData::clone() {
	return new RasterData(*this);
}

void RasterData::SQLInsert()
{
	isInserted = true;

	DBConnector::getInstance()->Insert("rasterdatas", uuid);
}


bool RasterData::setValue(long x, long y, double value)
{
	QMutexLocker ml(&mutex);

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

	long blWidth = width/RASTERBLOCKSIZE+1;
	long blHeight = height/RASTERBLOCKSIZE+1;

	if(cache)
		return;

	cache = new DbCache<RasterBlockLabel*, QByteArray>(RASTERBLOCKCACHESIZE);
	blockLabels = new RasterBlockLabel[blWidth*blHeight];

	double buffer[RASTERBLOCKSIZE*RASTERBLOCKSIZE];
	for(int i=0;i<RASTERBLOCKSIZE*RASTERBLOCKSIZE;i++)
		buffer[i] = NoValue;

	int buffersize = sizeof(buffer);
	char* pBuffer = (char*)&buffer;

	// we most commonly loop through rasterdata, so we reverse the cache inserts, 
	// to gain a bit performance in the first loop iteration
	for(long x = blWidth-1; x>=0; x--)
	{
		for(long y = blHeight-1; y>=0; y--)
		{
			RasterBlockLabel* pBlock = &blockLabels[x+y*blWidth];
			pBlock->backRef = this;
			pBlock->x = x;
			pBlock->y = y;
			pBlock->isInserted = false;
			QByteArray *qba = new QByteArray(pBuffer, buffersize);
			cache->add(pBlock,qba);
		}
	}
}
void RasterData::SQLDeleteField()
{
	if(width==0 || height==0)
		return;

	if(!cache)
		return;

	delete cache;
	delete blockLabels;
	cache = NULL;

	QSqlQuery *q = DBConnector::getInstance()->getQuery("DELETE FROM rasterfields WHERE owner LIKE ?");
	if(q)
	{
		q->addBindValue(uuid.toByteArray());
		DBConnector::getInstance()->ExecuteQuery(q);
	}
}

double RasterData::SQLGetValue(long x, long y) const
{
	if(!cache)	return NoValue;

	long xBl = x/RASTERBLOCKSIZE;
	long yBl = y/RASTERBLOCKSIZE;
	long blWidth = (width)/RASTERBLOCKSIZE+1;
	QByteArray *qba = cache->get(&blockLabels[xBl+yBl*blWidth]);

	return ((double*)qba->data())[(x%RASTERBLOCKSIZE) + (y%RASTERBLOCKSIZE)*RASTERBLOCKSIZE];
}


void RasterData::setBlock(long x, long y, double* data)
{
	if(!cache) return;

	long blWidth = width/RASTERBLOCKSIZE+1;
	QByteArray *qba = cache->get(&blockLabels[x+y*blWidth]);
	memcpy(qba->data(), data, sizeof(double) *RASTERBLOCKSIZE*RASTERBLOCKSIZE);
}

void RasterData::SQLSetValue(long x, long y, double value)
{
	if(!cache) return;

	long xBl = x/RASTERBLOCKSIZE;
	long yBl = y/RASTERBLOCKSIZE;
	long blWidth = width/RASTERBLOCKSIZE+1;
	QByteArray *qba = cache->get(&blockLabels[xBl+yBl*blWidth]);
	((double*)qba->data())[(x%RASTERBLOCKSIZE) + (y%RASTERBLOCKSIZE)*RASTERBLOCKSIZE] = value;
}

void RasterData::SQLCopyField(const RasterData *ref)
{
	if(!cache || !ref->cache)	return;

	long blWidth = width/RASTERBLOCKSIZE+1;
	long blHeight = height/RASTERBLOCKSIZE+1;

	for(long x = 0; x<blWidth; x++)
		for(long y = 0; y<blHeight; y++)
			*cache->get(&blockLabels[x+y*blWidth]) = *ref->cache->get(&ref->blockLabels[x+y*blWidth]);
}

QByteArray* RasterData::RasterBlockLabel::LoadFromDb()
{
	QSqlQuery *q = DBConnector::getInstance()->getQuery("SELECT data FROM rasterfields WHERE owner LIKE ? AND x=? AND y=?");
	q->addBindValue(backRef->uuid.toByteArray());
	q->addBindValue(QVariant::fromValue(x));
	q->addBindValue(QVariant::fromValue(y));
	if(!DBConnector::getInstance()->ExecuteSelectQuery(q))
	{
		delete q;
		return NULL;
	}

	QByteArray* qba = new QByteArray(q->value(0).toByteArray());
	delete q;
	return qba;
}

void RasterData::RasterBlockLabel::SaveToDb(QByteArray *qba)
{
	if(!isInserted)
	{
		QSqlQuery *q = DBConnector::getInstance()->getQuery("INSERT INTO rasterfields(owner,x,y,data) VALUES (?,?,?,?)");
		q->addBindValue(backRef->uuid.toByteArray());
		q->addBindValue(QVariant::fromValue(x));
		q->addBindValue(QVariant::fromValue(y));
		q->addBindValue(*qba);
		DBConnector::getInstance()->ExecuteQuery(q);
		isInserted = true;
	}
	else
	{
		QSqlQuery *q = DBConnector::getInstance()->getQuery("UPDATE rasterfields SET data=? WHERE owner=? AND x=? AND y=?");
		q->addBindValue(*qba);
		q->addBindValue(backRef->uuid.toByteArray());
		q->addBindValue(QVariant::fromValue(x));
		q->addBindValue(QVariant::fromValue(y));
		DBConnector::getInstance()->ExecuteQuery(q);
	}
}

void RasterData::_moveToDb()
{
	// don't do anything
}
