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
	/** @brief creates a rasterdata based on sql data */
    RasterData(QByteArray qba);

    RasterData(long  width, long  height, double  cellSize);

    RasterData(const RasterData &other);
    RasterData();
	Components getType();
    virtual ~RasterData();
    void clear();
    double getValue(long x, long y) const;
    bool setValue(long x, long y, double value);
    unsigned long getWidth()const {return width;}
    unsigned long getHeight() const {return height;}
    double getCellSize() const {return cellSize;}
    double getNoValue() const {return NoValue;}
    void setNoValue(double NoValue)  {this->NoValue = NoValue;}
    double getMinValue() const {return minValue;}
    double getMaxValue() const {return maxValue;}
    double getSum() const;
    std::vector<double> getMoorNeighbourhood(long x, long y) const;
    void setSize(long width, long height, double cellsize);
    void getNeighboorhood(double** d, int width, int height, int x, int y);

    void getMoorNeighbourhood(std::vector<double> & neigh, long x, long y);
    void setDebugValue(int val){debugValue = val;}
    int getDebugValue() {return debugValue;}

    Component * clone();
private:
	
	virtual QByteArray GetValue();

    long width;
    long height;
    double cellSize;
    double NoValue;

    double minValue;
    double maxValue;
    double ** data;

    int debugValue;
    bool isClone;

    void createNewDataSet();


};
typedef std::map<std::string, DM::RasterData*> RasterDataMap;
}
#endif // RASTERDATA_H
