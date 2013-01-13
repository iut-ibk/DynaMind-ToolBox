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

/** @ingroup DynaMind-Core
  * @brief  Raster data datset in DynaMind
  */
class DM_HELPER_DLL_EXPORT RasterData : public Component
{
public:
    RasterData(long width, long height, double cellsizeX, double cellsizeY, double xoffset, double yoffset);

    RasterData(const RasterData &other);
    RasterData();
    virtual ~RasterData();
    void clear();
    /** @brief Retruns value from cell at real x and real y */
    double getValue(long x, long y) const;
    /** @brief Retruns value from cell at  x and  y */
    double getCell(long x, long y) const;
    /** @brief Sets value from cell at real x and real y */
    bool setValue(long x, long y, double value);
    /** @brief Sets value from cell at x and y */
    bool setCell(long x, long y, double value);
    /** @brief Retruns  Returns elements in X */
    unsigned long getWidth()const {return width;}
    /** @brief Retruns  Returns elements in Y */
    unsigned long getHeight() const {return height;}
    /** @brief Retruns  cell size  in x */
    double getCellSizeX() const {return cellSizeX;}
    /** @brief Retruns  cell size  in y */
    double getCellSizeY() const {return cellSizeY;}
    /** @brief Returns no data value */
    double getNoValue() const {return NoValue;}
    /** @brief Sets no data value */
    void setNoValue(double NoValue)  {this->NoValue = NoValue;}
    /** @brief Returns min value */
    double getMinValue() const {return minValue;}
    /** @brief Returns max value */
    double getMaxValue() const {return maxValue;}
    /** @brief Returns sum */
    double getSum() const;
    /** @brief Returns offset x */
    double getXOffset(){return xoffset;}
    /** @brief Returns offset y */
    double getYOffset(){return yoffset;}
    void setXOffset(double value){this->xoffset=value;}
    void setYOffset(double value){this->yoffset=value;}
    std::vector<double> getMoorNeighbourhood(long x, long y) const;
    void setSize(long width, long height, double cellsizeX, double cellsizeY, double xoffset, double yoffset);
    void getNeighboorhood(double** d, int width, int height, int x, int y);

    void getMoorNeighbourhood(std::vector<double> & neigh, long x, long y);
    void setDebugValue(int val){debugValue = val;}
    int getDebugValue() {return debugValue;}

    Component * clone();
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
    float ** data;

    int debugValue;
    bool isClone;

    void createNewDataSet();


};
typedef std::map<std::string, DM::RasterData*> RasterDataMap;
}
#endif // RASTERDATA_H
