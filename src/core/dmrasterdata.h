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
  *
  * @brief Provides a raster data set
*/
class DM_HELPER_DLL_EXPORT RasterData : public Component
{
public:
	/** @brief constructor initializing a new field */
	RasterData(long width, long height, 
				double cellsizeX, double cellsizeY, 
				double xoffset, double yoffset);

	/** @brief constructor initializing via a other field */
	RasterData(const RasterData &other);

	/** @brief constructor, does not initialize a field */
	RasterData();

	/** @brief overloaded function for returning the internal type DM::RASTERDATA */
	Components getType() const;

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

	/** @brief returns the cell size. This method is deprecated instead getCellSizeX and getCellSizeY should be used */
	double getCellSize() const;

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
	void setSize(long width, long height, 
				double cellsizeX, double cellsizeY, 
				double xoffset, double yoffset);

	/** @brief returns the moore neighbourhood of a cell as a vector
	* The moore neighbourhoods from the cell at pos width / length are all adajanct cells
	*
	* x x x x x
	* x 0 1 2 x
	* x 3 4 5 x
	* x 6 7 8 x
	* x x x x x
	*
	* x ... non moore neighboors
	* 0 - 8 moor neighboors and their appearance in the return vector (including the current position at 4)
	*
	* At the edge of the rasterfield elements from the neighbours from the opposite side are used
	* e.g. if pos_x = -1 element at pos_x = raster_field_width - 1 is used
	*/
	std::vector<double> getMoorNeighbourhood(long x, long y) const;

	/** @brief copies the moore neighbourhood of a cell in the neigh vector for detailed description
	* see method std::vector<double> getMoorNeighbourhood(long x, long y) const;
	*/
	void getMoorNeighbourhood(std::vector<double> & neigh, long x, long y);

	/** @brief returns the  neighbourhood of a cell defined by width an height.
	*The current cell is defined as x = (int) (width -1)/2 and y = (int) (height -1)/2
	* At the edge of the rasterfield elements from the neighbours from the opposite side are used
	* e.g. if pos_x = -1 element at pos_x = raster_field_width - 1 is used
	*/
	void getNeighboorhood(double** d, int width, int height, int x, int y);

	/** @brief sets the debug value */
	void setDebugValue(int val){debugValue = val;}

	/** @brief returns the debug value */
	int getDebugValue() {return debugValue;}

	/** @brief overloaded clone operator
	@internal */
	Component * clone();

	/** @brief fill a full block of RASTERBLOCKSIZE*RASTERBLOCKSIZE with data 
	x and y represent the block coordinates, NOT the actual point coordinate */
	void setBlock(long x, long y, double* data);
	
	/** @brief exports the component to the db, it can afterwards deleted safly */
	void _moveToDb();
private:
	class RasterBlockLabel
	{
	public:
		long x; 
		long y;
		const RasterData *backRef;
		bool isInserted;
		QByteArray* LoadFromDb();
		void SaveToDb(QByteArray *qba);
	};

	/** @brief return table name */
	QString getTableName();
	void Synchronize();
	void SQLInsert();
	void SQLDeleteField();
	void SQLInsertField(long width, long height);
	double SQLGetValue(long x, long y) const;
	void SQLSetValue(long x, long y, double value);
	void SQLCopyField(const RasterData *ref);

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

	RasterBlockLabel *blockLabels;
	DbCache<RasterBlockLabel*, QByteArray> *cache;
};
typedef std::map<std::string, DM::RasterData*> RasterDataMap;
}
#endif // RASTERDATA_H
