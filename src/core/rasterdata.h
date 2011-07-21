/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#ifndef RASTERDATA_H
#define RASTERDATA_H

#include "compilersettings.h"
#include "vectordata.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace   ublas = boost::numeric::ublas;

class VIBE_HELPER_DLL_EXPORT RasterData
{
public:
    RasterData(long  width, long  height, double  cellSize);
    RasterData(const RasterData &other);
    RasterData();
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
    ublas::vector<double> getMoorNeighbourhood(long x, long y) const;
    void setSize(long width, long height, double cellsize);
    void getNeighboorhood(double** d, int width, int height, int x, int y);
    void setName(std::string name) {this->name = name;}
    std::string  getName() const{return this->name;}

    void setUUID(std::string UUID){this->uuid = UUID;}
    std::string  getUUID() const {return this->uuid;}
    void getMoorNeighbourhood(ublas::vector<double> & neigh, long x, long y);
private:

    std::string name;    
    std::string uuid;
    long width;
    long height;
    double cellSize;
    double NoValue;

    double minValue;
    double maxValue;
    double ** data;


};
#endif // RASTERDATA_H
