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
#include "plot.h"
#include <qwt_symbol.h>

/*Plot::Plot(QWidget * parent)
{

    this->counter = 0;
    this->setParent(parent);
    setTitle("Simulation Results");
    insertLegend(new QwtLegend(), QwtPlot::RightLegend);

    // Set axis titles
    setAxisTitle(xBottom, "x -->");
    setAxisTitle(yLeft, "y -->");

    // Insert new curves
    data = new QwtPlotCurve("Data");

    data->setRenderHint(QwtPlotItem::RenderAntialiased);

    data->setPen(QPen(Qt::red));
    data->setStyle(QwtPlotCurve::NoCurve);

    const QColor &c = Qt::red;
    data->setSymbol(QwtSymbol(QwtSymbol::XCross,
        QBrush(c), QPen(c), QSize(5, 5)) );
    data->attach(this);
    QObject::connect(this,SIGNAL(update(void)),this,SLOT(replot(void)));
}

void Plot::appendData( double x, double y) {
    d_x.append(x);
    d_y.append(y);
    data->setData(d_x, d_y);
    emit update();
    QCoreApplication::processEvents();
}*/

