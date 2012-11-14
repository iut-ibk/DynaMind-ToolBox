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



namespace DM {

class DM_HELPER_DLL_EXPORT RasterData : public Component
{
public:
    RasterData(long width, long height, double cellsizeX, double cellsizeY, double xoffset, double yoffset);

    RasterData(const RasterData &other);
    RasterData();
	Components getType();
    virtual ~RasterData();
    void clear();
    double getValue(long x, long y) const;
    double getCell(long x, long y) const;
    bool setValue(long x, long y, double value);
    bool setCell(long x, long y, double value);
    unsigned long getWidth()const {return width;}
    unsigned long getHeight() const {return height;}
    double getCellSizeX() const {return cellSizeX;}
    double getCellSizeY() const {return cellSizeY;}
    double getNoValue() const {return NoValue;}
    void setNoValue(double NoValue)  {this->NoValue = NoValue;}
    double getMinValue() const {return minValue;}
    double getMaxValue() const {return maxValue;}
    double getSum() const;
    double getXOffset(){return xoffset;}
    double getYOffset(){return yoffset;}
    void setXOffset(double value){this->xoffset=value;}
    void setYOffset(double value){this->yoffset=value;}
    std::vector<double> getMoorNeighbourhood(long x, long y) const;
    void setSize(long width, long height, double cellsizeX, double cellsizeY, double xoffset, double yoffset);
    void getNeighboorhood(float** d, int width, int height, int x, int y);

    void getMoorNeighbourhood(std::vector<double> & neigh, long x, long y);
    void setDebugValue(int val){debugValue = val;}
    int getDebugValue() {return debugValue;}

    Component * clone();
private:
    int _linkID;

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
    //bool isClone;

    //void createNewDataSet();

	void SQLInsert();
    //void SQLDelete();
	void SQLDeleteField();
	void SQLInsertField(long width, long height, double value);
	double SQLGetValue(long x, long y) const;
	void SQLSetValue(long x, long y, double value);
	void SQLUpdateLink(int id);

    //int GetLinkID() const;
	//void SQLSetValues();
    /** @brief return table name */
    QString getTableName();
};
typedef std::map<std::string, DM::RasterData*> RasterDataMap;
}
#endif // RASTERDATA_H
