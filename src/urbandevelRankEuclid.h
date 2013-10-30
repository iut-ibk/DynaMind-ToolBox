/**
 * @file
 * @author  Christian Mikovits <christian.mikovits@uibk.ac.at>
 * @version 0.1a
 * @section LICENSE
 * Module for [temporal] ranking of areas for development
   Copyright (C) 2013 Christian Mikovits

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**/

#ifndef urbandevelRankEuclid_H
#define urbandevelRankEuclid_H

#include <dmmodule.h>

class urbandevelRankEuclid: public DM::Module
{
    DM_DECLARE_NODE(urbandevelRankEuclid)
public:
    urbandevelRankEuclid();
    ~urbandevelRankEuclid();

    void run();
    void init();
private:
    bool usedevelyears;
    bool prefersmallareas;

    DM::View superblock;
    DM::View superblock_centroids;
    DM::View city;
};

#endif // urbandevelRankEuclid_H
