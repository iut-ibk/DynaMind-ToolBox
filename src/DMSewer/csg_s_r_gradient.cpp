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
#include "csg_s_r_gradient.h"
#include <iostream>

using namespace std;
namespace csg_s {

R_Gradient::R_Gradient(DM::dataLayer *layer_):rule(layer_) {
        this->depth = 0.;
};

R_Gradient::R_Gradient(DM::dataLayer *layer_, double depth):rule(layer_)
{
	this->depth = depth;

};

QVector<int> R_Gradient::possible_runoffField(Position *pos, double rel_height) {
	double *heightField = neighbours.Moore3x3(pos->getWidth(), pos->getHeight(), layer_);
	double height = heightField[4];
	double gradient = 0.;
        QVector<int> directions;
       // cout << "d" << depth << "\n";
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if ((i*3+j) != 4) {
                                double cellheight = heightField[i*3+j];

                                if (( heightField[i*3+j]-depth-height) <= (rel_height*(-1.))) {
					directions.append(i*3+j);
                                } else{
                                   //cout << cellheight << " < " << height << "de " <<depth <<"\n";
                                }
			}
		}
	}

	if (directions.size() == 0) {
		directions.append(4);
	}
	
	return directions;
}

QVector<int> R_Gradient::run_field(Position *pos) {	
	return possible_runoffField(pos, -0.0001);
};

QVector<int> R_Gradient::run_field(Position *pos, double rel_height) {
	return possible_runoffField(pos, rel_height);
};

/**
*@brief Find and Return steppest Cell
*@author Christian Urich
*/
double R_Gradient::run(Position *pos) {
	double *heightField = neighbours.Moore3x3(pos->getWidth(),pos->getHeight(), layer_);
	

	//Find Highest Gradient
	double height = heightField[4]; //Height of the Center Cell
        double gradient = 0.0000; //Presicion
	double direction = 4;
	for (int i = 0; i < 3; i++){
              //  cout << endl;
		for(int j = 0; j < 3; j++){
			if ((i*3+j) != 4) {
                             //   cout << "\t" <<  heightField[i*3+j] - height ;
				if (( heightField[i*3+j] - height) < gradient) {
					gradient = heightField[i*3+j] - height;
					direction = i*3+j;
				}
			}
		}
	}
       // cout << endl;

	return direction;
};

	
}

QVector< int > csg_s::R_Gradient::number_Field(Position * pos)
{
double *heightField = neighbours.Moore3x3(pos->getWidth(), pos->getHeight(), layer_);
	QVector<int> directions;	
	
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			if ( heightField[i*3+j] > 0.) {
					directions.append(i*3+j);
				}
			}
		}
	
	return directions;
}

