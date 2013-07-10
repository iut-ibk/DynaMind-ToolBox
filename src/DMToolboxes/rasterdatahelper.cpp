/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * rdata file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

 * rdata program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * rdata program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with rdata program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "rasterdatahelper.h"
#include <QPolygonF>


double RasterDataHelper::meanOverAreaWithBlocker (DM::RasterData * rdata, std::vector<DM::Node*>   & points, DM::RasterData * blocker, DM::Node * offset) {
    double sum = 0;
    long counter = 0;
    double cellSizeX = rdata->getCellSizeX();
    double cellSizeY = rdata->getCellSizeY();

    QPolygonF area;
    foreach(DM::Node * p , points ) {
        if (!offset) {
            area.append(QPointF(p->getX(), p->getY()));
        }
        else {
            area.append(QPointF(p->getX() + offset->getX(), p->getY() + offset->getY()));
        }
    }

    if ( area.size() > 0 ) {
        double minx = area[0].x();
        double maxx = area[0].x();
        double miny = area[0].y();
        double maxy = area[0].y();
        for ( int j = 0; j < area.size(); j++ ) {
            if ( area[j].x() < minx ) {
                minx = area[j].x();
            }
            if ( area[j].x() > maxx ) {
                maxx = area[j].x();
            }
            if ( area[j].y() < miny ) {
                miny = area[j].y();
            }
            if ( area[j].y() > maxy ) {
                maxy = area[j].y();
            }
        }

        for ( double x = minx; x < maxx; x = x+cellSizeX ) {
            for ( double y = miny; y < maxy; y = y+cellSizeY ) {
                if ( area.containsPoint(QPointF(x,y), Qt::WindingFill) == true ) {
                    if ( blocker != 0 ) {
                        if ( blocker->getCell((int) x/cellSizeX , (int)  y/cellSizeY) <  1 ) {
                            if ( rdata->getNoValue() != rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY)) {
                                sum = sum + rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY);
                                counter++;
                            }
                            blocker->setCell((int) x/cellSizeX, (int) y/cellSizeY, 2);
                        }
                    } else {

                        if ( rdata->getNoValue() != rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY)) {
                            sum = sum + rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY);
                            counter++;
                        }
                    }

                }

            }

        }

    }
    if(counter > 0)
        return sum/counter;
    else return 0;
}

double RasterDataHelper::meanOverArea (DM::RasterData * rdata, std::vector<DM::Node*>   & points, DM::Node * offset) {
    return RasterDataHelper::meanOverAreaWithBlocker(rdata, points,0, offset);

}


double RasterDataHelper::sumOverArea (DM::RasterData * rdata, std::vector<DM::Node*> &points,DM::RasterData * blocker, DM::Node * offset) {
    double sum = 0;
    double cellSizeX = rdata->getCellSizeX();
    double cellSizeY = rdata->getCellSizeY();

    QPolygonF area;

    foreach(DM::Node * p , points ) {
        if (!offset) {
            area.append(QPointF(p->getX(), p->getY()));
        }
        else {
            area.append(QPointF(p->getX() + offset->getX(), p->getY() + offset->getY()));
        }
    }

    if ( area.size() > 0 ) {
        double minx = area[0].x();
        double maxx = area[0].x();
        double miny = area[0].y();
        double maxy = area[0].y();
        for ( int j = 0; j < area.size(); j++ ) {
            if ( area[j].x() < minx ) {
                minx = area[j].x();
            }
            if ( area[j].x() > maxx ) {
                maxx = area[j].x();
            }
            if ( area[j].y() < miny ) {
                miny = area[j].y();
            }
            if ( area[j].y() > maxy ) {
                maxy = area[j].y();
            }
        }

        for ( double x = minx; x < maxx; x = x+cellSizeX ) {
            for ( double y = miny; y < maxy; y = y+cellSizeY ) {
                if ( area.containsPoint(QPointF(x,y), Qt::WindingFill) == true ) {
                    if ( blocker != 0 ) {
                        if ( blocker->getCell((int) x/cellSizeX , (int)  y/cellSizeY) <  1 ) {
                            if ( rdata->getNoValue() != rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY)) {
                                sum = sum + rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY);
                            }
                            blocker->setCell((int) x/cellSizeX, (int) y/cellSizeY, 2);

                        }
                    } else {
                        if ( rdata->getNoValue() != rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY)) {
                            sum = sum + rdata->getCell((int) x/cellSizeX, (int)  y/cellSizeY);
                        }
                    }
                }
            }
        }
    }

    return sum;
}
