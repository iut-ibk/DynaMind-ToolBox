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
#ifndef DMRASTERDATA_H
#define DMRASTERDATA_H

#include "dmcompilersettings.h"
#include <dmcomponent.h>
#include "dmdbconnector.h"
// cache sizes defined in dmdbconnector.h

namespace DM {

/** @class DM::RasterData
  * @ingroup DynaMind-Core
  * @brief Provides a raster data set
*/
class DM_HELPER_DLL_EXPORT RasterData : public Component
{
public:
	/** @brief constructor initializing a new field */
    RasterData(long width, long height, double cellsizeX, double cellsizeY, double xoffset, double yoffset);
	/** @brief constructor initializing via a other field */
    RasterData(const RasterData &other);
	/** @brief constructor, does not initialize a field */
    RasterData();
	/** @brief overloaded function for returning the internal type DM::RASTERDATA */
	Components getType();
	/** @brief destructor deletes the field and its values too */
    ~RasterData();
	/** @brief sets the field to NoValue */
    void clear();
	/** @brief get the value at the specific coordinate 
		offset will be substracted from the position declaration */
    double getValue(long x, long y) const;
	/** @brief get the value in the specific cell */
    double getCell(long x, long y) const;
	/** @brief set the value at the specific coordinate 
		offset will be substracted from the position declaration  */
    bool setValue(long x, long y, double value);
	/** @brief get the value in the specific cell */
    bool setCell(long x, long y, double value);
	/** @brief returns the number of cells in horizontal direction */
    unsigned long getWidth()const {return width;}
	/** @brief returns the number of cells in vertical direction */
    unsigned long getHeight() const {return height;}
	/** @brief returns the width of the cell */
    double getCellSizeX() const {return cellSizeX;}
	/** @brief returns the height of the cell */
    double getCellSizeY() const {return cellSizeY;}
	/** @brief returns the default value of the field */
    double getNoValue() const {return NoValue;}
	/** @brief sets the default value in the field
		does NOT update the field values */
    void setNoValue(double NoValue)  {this->NoValue = NoValue;}
	/** @brief returns the lower limit of field values */
    double getMinValue() const {return minValue;}
	/** @brief returns the upper limit of field values */
    double getMaxValue() const {return maxValue;}
	/** @brief returns the sum over all cells */
    double getSum() const;
	/** @brief returns the offset in horizontal direction */
    double getXOffset(){return xoffset;}
	/** @brief returns the offset in vertical direction */
    double getYOffset(){return yoffset;}
	/** @brief sets the offset in horizontal direction */
    void setXOffset(double value){this->xoffset=value;}
	/** @brief sets the offset in vertical direction */
    void setYOffset(double value){this->yoffset=value;}
	/** @brief resizes the field */
    void setSize(long width, long height, double cellsizeX, double cellsizeY, double xoffset, double yoffset);

    std::vector<double> getMoorNeighbourhood(long x, long y) const;
    void getMoorNeighbourhood(std::vector<double> & neigh, long x, long y);
    void getNeighboorhood(double** d, int width, int height, int x, int y);
	
	/** @brief sets the debug value */
    void setDebugValue(int val){debugValue = val;}
	/** @brief returns the debug value */
    int getDebugValue() {return debugValue;}
	
	/** @brief overloaded clone operator
		@internal */
    Component * clone();
	/** @brief updates the field values 
		@internal */
    //void ForceUpdate() const;

#ifdef CACHE_PROFILING
    //static void PrintStatistics();
#endif
private:
    long width;
    long height;
    double cellSizeX;
    double cellSizeY;
    double NoValue;
    double xoffset;
    double yoffset;
    double minValue;
    double maxValue;
    int debugValue;

    void SQLInsert();
	void SQLDeleteField();
    void SQLInsertField(long width, long height);
    double SQLGetValue(long x, long y) const;
    //QByteArray* SQLGetRow(long y) const;
    //QByteArray* SQLForceGetRow(long y) const;
    //void SQLSetRow(long y, QByteArray *data);
    void SQLSetValue(long x, long y, double value);
	void SQLCopyField(const RasterData *ref);

    /** @brief return table name */
    QString getTableName();
	void Synchronize();

	class RasterBlockLabel
	{
	public:
		long x; 
		long y;
		const RasterData *backRef;
		bool isInserted;
		/*
		RasterBlock(const RasterData *parent, const long x, const long y)
		{
			backRef = parent;
			this->x = x;
			this->y = y;
		}*/
		QByteArray* LoadFromDb();
		void SaveToDb(QByteArray *qba);
		/*RasterBlock& operator->() {
			return *this;
		}*/
	};
	RasterBlockLabel *blockLabels;
	DbCache<RasterBlockLabel*, QByteArray> *cache;
};
typedef std::map<std::string, DM::RasterData*> RasterDataMap;
}
#endif // RASTERDATA_H
