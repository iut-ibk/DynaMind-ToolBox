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
#include "dataLayer.h"
#include "csg_s_neighbourhood.h"
#include <iostream>

namespace csg_s
{

double* neighbourhood::Moore3x3(int x, int y,DM::dataLayer *pLayer)
{
		
	int i=0;	
	for(int j=0;j<3;j++)
	{
		for(int k=0;k<3;k++)
		{
			i=j*3+k;
			{				
				nMoore3x3[i]=pLayer->neighbour(((x-1)+k),(y-1)+j);
				//Neighbour definition:
				//6 7 8
				//3(4)5
				//0 1 2
			}
		}	
	}
	return nMoore3x3;
};


neighbourhood::neighbourhood()
{
	nMoore3x3 = new double[9];
};
neighbourhood::~neighbourhood()
{
        delete [] nMoore3x3;
        nMoore3x3 = 0;

};


}
