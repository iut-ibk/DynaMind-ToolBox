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
#include "csg_s_operations.h"
#include "csg_s_r_gradient.h"
#include "csg_s_position.h"
#include <QVector>
//#include <ofstream>


int csg_s::csg_s_operations::returnPositionX(int Position)
{
    	int X;
	switch (Position){
			case 0:
				X = -1;
				break;
			case 1:
				X = 0;
		
				break;
			case 2:
				X = 1;
				break;
			case 3:
				X = -1;
				break;
			case 4:
				X = 0;
				break;
			case 5:
				X = 1;
				break;
			case 6:
				X = -1;
				break;
			case 7:
				X = 0;
				break;
			case 8:
				X = 1;
				break;
		
		}
	return X;

}


int csg_s::csg_s_operations::returnPositionY(int Position)
{
	int Y;
	switch (Position){
			case 0:
				Y = -1;
				break;
			case 1:
				Y = -1;				
				break;
			case 2:
				Y = -1;				
				break;
			case 3:
				Y = 0;
				break;
			case 4:
				Y = 0;				
				break;
			case 5:
				Y = 0;				
				break;
			case 6:
				Y = 1;				
				break;
			case 7:
				Y = 1;				
				break;
			case 8:
				Y = 1;				
				break;
		
		}
	return Y;
}

void csg_s::csg_s_operations::addRadiusValue(int x, int y, DM::dataLayer * layer, int rmax)
{
	csg_s_operations::addRadiusValue(x, y, layer, rmax, 0.);
}

void csg_s::csg_s_operations::addRadiusValue(int x, int y, DM::dataLayer * layer, int rmax, double value) {
        int level = rmax;
        if (rmax > 500) {
                rmax = 500;
        }
        int i = x - rmax;
        int j = y - rmax;

        if (i < 0) i = 0;
        if (j < 0) j = 0;

        int imax = 10;
        int jmax = 10;
        int k = 0;

        for (i; i < rmax+x;  i++) {
                j = y - rmax;

                for ( j;  j < rmax+y; j++) {

                        if (i != x || j != y) {
                                double r = sqrt(double((i-x)*(i-x) + (j-y)*(j-y)));
                                double val = (-level/10. * 1./r*value);
                                if (layer->getVal(i,j) > val ) {
                                        layer->setVal(i,j,val );
                                }
                        } else {
                                double val = (-level/10. *( 2.) * value);
                                if (layer->getVal(i,j) > val) {
                                        layer->setVal(i,j,val);
                                }
                        }
                }
        }
}


void csg_s::csg_s_operations::addRadiusValueADD(int x, int y, DM::dataLayer * layer, int rmax, double value)
{
	int level = rmax;
        if (rmax > 500) {
                rmax = 500;
	}
	int i = x - rmax;
        int j = y - rmax;

        if (i < 0) i = 0;
        if (j < 0) j = 0;

        int imax = 10;
        int jmax = 10;
        int k = 0;
        double z = rmax*rmax;
        for (i; i < rmax+x;  i++) {
                j = y - rmax;

                for ( j;  j < rmax+y; j++) {
                    double r = double((i-x)*(i-x) + (j-y)*(j-y));
                    double val = ((r-z)/10.)*(value);
                     if (layer->getVal(i,j) > val )
                        layer->setVal(i,j, val);
                }
        }
}

void csg_s::csg_s_operations::exportRaster(QString name, DM::dataLayer * layer_, double multiplikator)
{
        QString extension=".txt";

        QString fullFileName = name + extension;
        fstream txtout;
        txtout.open(fullFileName.toAscii(),ios::out);

        //header for ARCGIS import
        txtout<<"ncols "<<layer_->getwidth()<<"\n";
        txtout<<"nrows "<<layer_->getheight()<<"\n";
        txtout<<"xllcorner 0"<<"\n";
        txtout<<"yllcorner 0"<<"\n";
        txtout<<"cellsize "<<layer_->getcellSize()<<"\n";
        txtout<<"nodata_value 0"<<"\n";

        for (int j=0; j<layer_->getheight(); j++)
        {
                for (int i=0; i<layer_->getwidth(); i++)
                {
                        txtout<<layer_->getVal(i,j) * multiplikator<< " ";
                }
                txtout<<"\n";

        }
        txtout.close();


}

void csg_s::csg_s_operations::writePath(string Text, QString name)
{
        QString extension=".dat";

        QString fullFileName = name +"_Path"+ extension;


        ofstream txtout (fullFileName.toAscii(),ios_base::app);

        txtout << Text;
        txtout << flush;

        txtout.close();


}

void csg_s::csg_s_operations::findSewer(DM::dataLayer * Input, DM::dataLayer * Output)
{
	R_Gradient r_gradient(Input);
	for ( int i = 10; i < Input->getheight()-10; i++ ) {
		for (int j = 10; j < Input->getwidth()-10; j++) {
			Position *pos = new Position(j,i);
			QVector<int> directions =r_gradient.number_Field(pos);
			delete pos;
			if (directions.size() > 5) {
				Output->addVal(j,i, 1.0);
			}
		}
	}

}

void csg_s::csg_s_operations::intSewerNetwork(DM::dataLayer * InitSewer, DM::dataLayer * ResultField)
{
    cout << InitSewer->getheight() << "\n"<< flush;
    for (int i = 0; i < InitSewer->getheight(); i++) {
		for (int j = 0; j < InitSewer->getwidth(); j++){

			int val = InitSewer->getVal(j,i);
			if (val > 0) {
				csg_s_operations::addRadiusValue(j,i, ResultField, val*10);
			}
		}
	}
}
