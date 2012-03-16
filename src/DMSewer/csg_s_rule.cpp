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

#include "csg_s_rule.h"

namespace csg_s
{
rule::rule(DM::dataLayer *layer_)//:ruleLayer()
{	
    neighbours.setLayer(layer_);
    setRuleLayer(layer_);


    //init pointer for
    //neighbourhood::Moore3x3(int x, int y,dataLayer *pLayer)
    //in r_neighbourhood.cpp
    nMoore3x3 = new double[9];
    //-------------------------//
    for (int h=0;h<9;h++)//8 neighbours + 1 element
    {
        nMoore3x3[h]=0;
    }
    //-------------------------//

};

rule::~rule()
{
    //cout << "delete Moore\n";
    //
    delete []nMoore3x3;
    nMoore3x3=0;

};

void rule::applySwitch_1()
{
    if (layer_->showSwitch()==2)
    {
        layer_->setSwitch(1);
    }
};


void rule::applySwitch_2()
{
    //layer switch - only one rule applicable
    if (layer_->showSwitch()==1)
    {
        layer_->setSwitch(0);
    }
    else if (layer_->showSwitch()==0)
    {
        layer_->setSwitch(1);
    }
    else
    {
        std::cout<<"error layerswitch rule04\n";
    }
    //-------------------------//


};
}
