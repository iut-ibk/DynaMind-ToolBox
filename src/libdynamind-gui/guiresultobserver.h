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
#ifndef GUIRESULTOBSERVER_H
#define GUIRESULTOBSERVER_H
#include <dmresultobserver.h>
#include <QWidget>
#include <QObject>
#include <dmrasterdata.h>

#include <QVector>

class MainWindow;
class GUIResultObserver :public QObject, public DM::ResultObserver
{
    Q_OBJECT
private:
    MainWindow * ResultWidgetParent;
    bool ResultWidget;
    MainWindow * mw;
    std::string UUID;
    bool WidgetCreated;
    bool PlotWidgetCreated;
    std::string firstImage;
    std::string RasterDataName;
    QVector<std::string> PlotDoubleUUIDs;


public:
    GUIResultObserver();
    void addResultImage(std::string UUID, std::string filename);
    //void addRasterDataToViewer( QVector<RasterData>);//std::string UUID, std::string name);
    void addDoubleDataToPlot(std::string UUID, double x, double y);
    //void addVectorDataToViewer(QVector<VectorData>);

    void setResultWidget(MainWindow * widget);
    std::string getUUID(){return UUID;}
    std::string getfirstImage() {return firstImage;}

signals:
    void registerWidet(GUIResultObserver*);
    void registerPlotWidget(GUIResultObserver*, double, double);
    //void register3DWidget(GUIResultObserver*,  QVector<RasterData>);
    //void register3DWidget(GUIResultObserver*,  QVector<VectorData>);
    void updateImage(QString);
    void updateRasterData(QString, QString);
    //void newRasterData( QVector<RasterData>);
    //void newVectorData( QVector<VectorData>);
    void newDoubleDataForPlot( double x, double y);
};

#endif // GUIRESULTOBSERVER_H
