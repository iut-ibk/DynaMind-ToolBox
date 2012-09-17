/**
 * @file
 * @author  Gregor Burger <burger.gregor@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Gregor Burger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef VIEWERWINDOW_H
#define VIEWERWINDOW_H
#include <dmcompilersettings.h>
#include <QWidget>
#include <QTimer>

#include "dmcolorramp.h"

namespace Ui {
class ViewerWindow;
}

class QListWidgetItem;

namespace DM {


struct LayerSpec {
    std::string view;
    std::string attribute;
    ColorRamp ramp;
    double height;
};

class System;
class Layer;

class DM_HELPER_DLL_EXPORT ViewerWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit ViewerWindow(System *system, QWidget *parent = 0);
    ~ViewerWindow();
    
    void addLayers(std::list<LayerSpec> specs);
    
private slots:
    void addLayerFromSpec(LayerSpec spec);
    void addLayer(Layer *layer);
    void on_actionAdd_Layer_triggered();
    void offsetChanged();
    
    void on_x_layerOffset_valueChanged(double arg1);
    void on_y_layerOffset_valueChanged(double arg1);
    void on_z_layerOffset_valueChanged(double arg1);
    
    void on_actionReset_View_triggered();
    
    void on_names_currentIndexChanged(int);
    void on_playButton_clicked();
    
    void on_layer_listWidget_itemChanged(QListWidgetItem *);
    
    void timerShot();
    
private:
    ::Ui::ViewerWindow *ui;
    DM::System *system;
    QTimer timer;
};

}
#endif // VIEWERWINDOW_H
