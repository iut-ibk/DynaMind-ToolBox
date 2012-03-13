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
#include "userdefinedfunctions.h"
#include <iostream>
#include <cmath>
#include <cstdlib> 
#include <ctime> 
#include <cmath>
#include "dmmodule.h"

using namespace std;




double mu::numberOfValues(const double* values, int index)
{

	
    int counter = 0;
    for ( int i = 0; i < index-1; i++ ) {
        if ( std::abs(values[i] -values[index-1]) < 0.000001 )
            counter++;
    }
    return counter;
}

double mu::random(double value)
{
        //std::cout << (unsigned)time(0) << std::endl;
        //srand((unsigned)time(0));
    double val = rand() % (int) value;
    //std::cout << val << std::endl;
    return  val;
}

double mu::printValue(double value)
{

    std::cout << value << std::endl;
    return  0;
}
