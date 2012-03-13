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
#include "guiresultobserver.h"
#include <QTabWidget>
#include <QThread>
#include <QApplication>
#include <mainwindow.h>
#include <dmlogger.h>

GUIResultObserver::GUIResultObserver()
{
    ResultWidget = false;
    PlotWidgetCreated = false;

}
void GUIResultObserver::setResultWidget(MainWindow *  mainWidget) {
    this->ResultWidgetParent = mainWidget;

    /*connect(this, SIGNAL(registerWidet(GUIResultObserver * )), this->ResultWidgetParent , SLOT(registerResultWindow(GUIResultObserver *)), Qt::QueuedConnection);
    qRegisterMetaType< QVector<RasterData> >("QVector<RasterData>");
    connect(this, SIGNAL(register3DWidget(GUIResultObserver * ,  QVector<RasterData>)), this->ResultWidgetParent , SLOT(register3DResultWindow(GUIResultObserver *,  QVector<RasterData>)), Qt::BlockingQueuedConnection);
    qRegisterMetaType< QVector<RasterData> >("QVector<VectorData>");
    connect(this, SIGNAL(register3DWidget(GUIResultObserver * ,  QVector<VectorData>)), this->ResultWidgetParent , SLOT(register3DResultWindow(GUIResultObserver *,  QVector<VectorData>)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(registerPlotWidget(GUIResultObserver*,double, double)), this->ResultWidgetParent , SLOT(registerPlotWindow(GUIResultObserver*, double, double)), Qt::BlockingQueuedConnection);*/

}

void GUIResultObserver::addResultImage(std::string UUID, std::string filename) {
    this->UUID = UUID;
    if (!ResultWidget) {
        ResultWidget = true;
        firstImage = filename;
        emit registerWidet(this);
    }
    emit updateImage(QString::fromStdString(filename));

}
void GUIResultObserver::addDoubleDataToPlot(std::string UUID, double x, double y) {
    DM::Logger(DM::Debug) <<  "Send Double Data" << x << "/" << y;

    if (this->PlotDoubleUUIDs.indexOf(this->UUID)< 0) {
        this->UUID = UUID;
        this->PlotWidgetCreated = true;
        this->PlotDoubleUUIDs.append(UUID);
        emit registerPlotWidget(this, x, y);
    }

    emit newDoubleDataForPlot( x, y);

}
