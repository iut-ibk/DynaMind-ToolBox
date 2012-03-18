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
#include <dmcomponent.h>
#include "dmrasterdata.h"
#include <QPointF>
#include <boost/foreach.hpp>
#include <time.h>
#include <QMutex>
#include <dmlogger.h>

namespace   ublas = boost::numeric::ublas;

using namespace boost;
using namespace DM;


RasterData::RasterData(long  width, long  height, double  cellSize) : Component()
{
    this->width = width;
    this->height = height;
    this->cellSize = cellSize;
    this->NoValue = -9999;
    this->minValue = -9999;
    this->maxValue = -9999;
    data = new double*[width];
    for (long i = 0; i < width; i++) {
        data[i] = new double[height];
    }



}
double RasterData::getSum() const {
    double sum = 0;
    for ( long i = 0; i < width; i++ ) {
        for (  long j = 0; j < height; j++ ) {
            double val = data[i][j];
            if ( val != NoValue)
                sum += val;
        }
    }

    return sum;
}
double RasterData::getValue(long x, long y) const {
    if (  x >-1 && y >-1 && x < this->width && y < this->height) {
        return  data[x][y];
    } else {

        return  this->NoValue;
    }

}

bool RasterData::setValue(long x, long y, double value) {

    if (  x >-1 && y >-1 && x < this->width && y < this->height) {
        data[x][y] = value;

        if (minValue == this->NoValue || minValue > value) {
            minValue = value;
        }

        if (maxValue == this->NoValue || maxValue < value) {
            maxValue = value;
        }



        return true;
    } else {
        return false;
    }
    return false;
}

RasterData::~RasterData() {

    for (long i = 0; i < width; i++) {
        delete this->data[i];
    }
    if (height != 0)
        delete data;

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
            d[k][l] = this->getValue(x_cell,y_cell);
       /*     if (d[k][l] < 0 ) {
                vibens::Logger( vibens::Debug) << "Error";
            }*/

            l++;
        }
        k++;
    }
}
void RasterData::getMoorNeighbourhood(ublas::vector<double> &neigh, long x, long y) {
    int counter = 0;
    for ( long j = y-1; j <= y + 1; j++ ) {
        for ( long i = x-1; i <= x + 1; i++ ) {

            neigh[counter] = this->getValue(i,j);
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
ublas::vector<double>  RasterData::getMoorNeighbourhood(long x, long y) const {
    ublas::vector<double> neigh(9);
    int counter = 0;
    for ( long j = y-1; j <= y + 1; j++ ) {
        for ( long i = x-1; i <= x + 1; i++ ) {

            neigh[counter] = this->getValue(i,j);
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


RasterData::RasterData() : Component() {
    this->cellSize = 0;
    this->width = 0;
    this->height = 0;
    this->data = 0;
}
RasterData::RasterData(const RasterData &other) : Component(other) {

    this->cellSize = 0;
    this->width = 0;
    this->height = 0;
    this->NoValue = -9999;
    this->minValue = -9999;
    this->maxValue = -9999;
    //RasterData r;
    this->setSize(other.getWidth(), other.getHeight(), other.getCellSize());

    this->setNoValue(other.getNoValue());
    for (int i = 0; i < other.getWidth(); i++) {
        for (int j = 0; j < other.getHeight();j++) {
            this->setValue(i,j, other.getValue(i,j));
        }
    }

}

void RasterData::setSize(long width, long height, double cellsize) {
    clock_t start, finish;
    start = clock();

    if (width != this->width || height != this->height || this->cellSize != cellsize) {
       // std::cout << "Resize RasterData" << std::endl;
        this->width = width;
        this->height = height;
        this->cellSize = cellsize;
        this->NoValue = -9999;
        this->minValue = -9999;
        this->maxValue = -9999;

        //this->data = new ublas::matrix<double, ublas::row_major, ublas::unbounded_array<double> >(width, height);

        data = new double*[width];
        for (long i = 0; i < width; i++) {
            data[i] = new double[height];
        }
        //this->data->clear();
        /*for ( long i = 0; i < width; i++ ) {
            for (  long j = 0; j < height; j++ ) {
                data(i,j) = 0;
            }
        }*/
        finish = clock();
        //std::cout <<   ( double (finish - start)/CLOCKS_PER_SEC ) << std::endl;


    } else {
        //std::cout << "ERROR: Can't resize existing RasterData" << std::endl;
    }
}

void RasterData::clear() {
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            this->data[x][y] = 0;
        }
    }
}

Component * RasterData::clone() {
    return new RasterData(*this);
}
