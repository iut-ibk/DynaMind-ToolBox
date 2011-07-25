/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * rdata file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich

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


double RasterDataHelper::meanOverAreaWithBlocker (RasterData * rdata, std::vector<Point>   & points, RasterData * blocker) {
    double sum = 0;
    long counter = 0;
    double cellSize = rdata->getCellSize();

    QPolygonF area;
    foreach(Point p , points ) {
        area.append(QPointF(p.x, p.y));
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

        for ( double x = minx; x < maxx; x = x+cellSize ) {
            for ( double y = miny; y < maxy; y = y+cellSize ) {
                if ( area.containsPoint(QPointF(x,y), Qt::WindingFill) == true ) {
                    if ( blocker != 0 ) {
                        if ( blocker->getValue((int) x/cellSize , (int)  y/cellSize) <  1 ) {
                            sum = sum + rdata->getValue((int) x/cellSize, (int)  y/cellSize);
                            blocker->setValue((int) x/cellSize, (int) y/cellSize, 2);
                            counter++;

                        }
                    } else {

                        if ( rdata->getNoValue() != rdata->getValue((int) x/cellSize, (int)  y/cellSize)) {
                            sum = sum + rdata->getValue((int) x/cellSize, (int)  y/cellSize);
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

double RasterDataHelper::meanOverArea (RasterData * rdata, std::vector<Point>   & points) {
    return RasterDataHelper::meanOverAreaWithBlocker(rdata, points,0);

}


double RasterDataHelper::sumOverArea (RasterData * rdata, std::vector<Point>   & points, RasterData * blocker) {
    double sum = 0;
    double cellSize = rdata->getCellSize();

    QPolygonF area;
    foreach(Point p , points ) {
        area.append(QPointF(p.x, p.y));
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

        for ( double x = minx; x < maxx; x = x+cellSize ) {
            for ( double y = miny; y < maxy; y = y+cellSize ) {
                if ( area.containsPoint(QPointF(x,y), Qt::WindingFill) == true ) {
                    if ( blocker != 0 ) {
                        if ( blocker->getValue((int) x/cellSize , (int)  y/cellSize) <  1 ) {
                            sum = sum + rdata->getValue((int) x/cellSize, (int)  y/cellSize);
                            blocker->setValue((int) x/cellSize, (int) y/cellSize, 2);

                        }
                    } else {
                        sum = sum + rdata->getValue((int) x/cellSize, (int)  y/cellSize);
                    }

                }

            }

        }

    }

    return sum;
}
