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

using namespace DM;


RasterData::RasterData(long  width, long  height, double  cellSize) : Component()
{
    this->width = width;
    this->height = height;
    this->cellSize = cellSize;
    this->NoValue = -9999;
    this->minValue = -9999;
    this->maxValue = -9999;
    this->debugValue = 0;
    data = new double*[width];
    for (long i = 0; i < width; i++) {
        data[i] = new double[height];
    }
}
Components RasterData::getType()
{
	return DM::RASTERDATA;
}

void RasterData::getRawData(QBuffer *buf)
{
	// TODO
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
void RasterData::createNewDataSet() {
    double **data_old = this->data;
    this->minValue = 0;
    this->maxValue = 0;

    data = new double*[width];
    for (long i = 0; i < width; i++) {
        data[i] = new double[height];
    }

    for (int i = 0; i < getWidth(); i++) {
        for (int j = 0; j < getHeight();j++) {
            data[i][j] =  data_old[i][j];
        }
    }

    this->isClone = false;

}

bool RasterData::setValue(long x, long y, double value) {
    if (this->isClone == true)
        this->createNewDataSet();

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

    if (isClone)
        return;
    for (long i = 0; i < width; i++) {
        delete[] this->data[i];
    }
    if (height != 0)
        delete[] data;
    data = 0;


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
void RasterData::getMoorNeighbourhood(std::vector<double> &neigh, long x, long y) {
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
std::vector<double>  RasterData::getMoorNeighbourhood(long x, long y) const {
    std::vector<double> neigh(9);
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
    this->isClone = false;
}
RasterData::RasterData(const RasterData &other) : Component(other) {

    this->cellSize = other.cellSize;
    this->width = other.width;
    this->height = other.height;
    this->NoValue = other.NoValue;
    this->minValue = other.minValue;
    this->maxValue = other.maxValue;
    this->debugValue = other.debugValue;
    this->data = other.data;
    this->isClone = true;

}

void RasterData::setSize(long width, long height, double cellsize) {
    if (width != this->width || height != this->height || this->cellSize != cellsize) {
        this->width = width;
        this->height = height;
        this->cellSize = cellsize;
        this->NoValue = -9999;
        this->minValue = -9999;
        this->maxValue = -9999;

        data = new double*[width];
        for (long i = 0; i < width; i++) {
            data[i] = new double[height];
        }



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
