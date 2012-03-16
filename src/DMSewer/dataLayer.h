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

#include "dmcompilersettings.h"

#ifndef __datalayer
#define __datalayer

#include <ctime>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

#include <QtGui>
#include <QLine>
#include <QPainterPath>

#include "def.h"
//#include "vSys.h"

namespace DM{

class vSys;

/** @brief raster data layer
  *
  *	administrates data in a width x height array
  */
class DM_HELPER_DLL_EXPORT dataLayer
{

 public:
	dataLayer(int width_, int height_, int cellSize_);
	dataLayer();
	~dataLayer();

	void setVal(int x, int y, double val);
        double getVal(int x, int y) const;
	void addVal(int x, int y, double addVal);
        void clean();
	double randVal(double rangeLow, double rangeHigh, double median);

        double neighbour(int x, int y) const;

        int getcellSize() const {return cellSize;};
        int getheight() const {return height;};
        int getwidth() const {return width;};
	void setmHeight(int m) {mHeight=m;}
	int getmHeight() {return mHeight;}
	void setSwitch(int s) {layerswitch=s;};
	int showSwitch() {return layerswitch;};

    void saveDataToFile( QTextStream *in );
    void loadDataFromFile( QTextStream *out );


	dataLayer* createArea();
	dataLayer* rotatedArea(int alpha);

	dataLayer* Area_;
	dataLayer* rotArea_;

	 
protected:
	int width;
	int height;
	int mHeight;
	int cellSize;
	double *data;
	double *bdata;
	vSys *parentVSys;
	int layerswitch;


};

};


#endif
