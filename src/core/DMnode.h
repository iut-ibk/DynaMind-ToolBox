/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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

#ifndef NODE_H
#define NODE_H

#include <compilersettings.h>

#ifdef SWIG
#define DM_HELPER_DLL_EXPORT
#endif

#include <string>

namespace DM {
    class  Component;

    class DM_HELPER_DLL_EXPORT Node : public Component
    {
    private:
        double x;
        double y;
        double z;

    public:
        Node( double x, double y, double z );

        Node(const Node& n);
        double getX() const;
        double getY() const;
        double getZ() const;
        void setX(double x);
        void setY(double y);
        void setZ(double z);

        bool operator==(const Node & other) const;
        Node operator-(const Node & other) const;
        Node operator+(const Node & other) const;
        bool compare2d(const Node &other, double round = 0) const;
        bool compare2d(const Node * other, double round = 0) const;

        Component* clone();
    };
}
#endif // NODE_H
