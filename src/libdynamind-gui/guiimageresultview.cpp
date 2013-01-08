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
#include "guiimageresultview.h"
#include "ui_guiimageresultview.h"
#include <iostream>
#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <QMenu>
#include <QSvgGenerator>
#include <QFileDialog>
GUIImageResultView::GUIImageResultView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GUIImageResultView)
{
    ui->setupUi(this);
    connect(this->ui->pushButton_last, SIGNAL(clicked()), this, SLOT(setLastImage()));
    connect(this->ui->pushButton_first, SIGNAL(clicked()), this, SLOT(setFirstImage()));
    connect(this->ui->pushButton_up, SIGNAL(clicked()), this, SLOT(setImageUp()));
    connect(this->ui->pushButton_back, SIGNAL(clicked()), this, SLOT(setImageDown()));
    ui->graphicsView->setScene(&scene);
    view =  ui->graphicsView;
    currentTime = 0;
    zvalues = 0;
}

GUIImageResultView::~GUIImageResultView()
{
    delete ui;
}

void GUIImageResultView::updateImage(QString filename) {
    QGraphicsSvgItem * item = new QGraphicsSvgItem (filename);
    images.append(item);
    this->scene.addItem(item);
    this->setLastImage();

}
void GUIImageResultView::setLastImage() {

    images.last()->setZValue(zvalues++);
    this->currentTime = images.size()-1;

}
void GUIImageResultView::setFirstImage() {

    images.first()->setZValue(zvalues++);
    this->currentTime = 0;
}
void GUIImageResultView::setImageUp() {
    if (this->currentTime < images.size() -1 ) {
        this->currentTime++;
        images[currentTime]->setZValue(zvalues++);
    }
}
void GUIImageResultView::setImageDown() {
    if (this->currentTime > 0) {
        this->currentTime--;
        images[currentTime]->setZValue(zvalues++);
    }
}
void GUIImageResultView::saveFigure() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("PNG File"), "", tr("PNG Files (*.png)"));
    if (!fileName.isEmpty()) {
        if (!fileName.contains(".png"))
            fileName+=".png";
    }

     QImage img(4000,3000,QImage::Format_ARGB32_Premultiplied);
     QPainter painter(&img);
     painter.setRenderHint(QPainter::Antialiasing);
     this->scene.render(&painter);
     painter.end();
     img.save(fileName);

}

void GUIImageResultView::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu;
    QAction  * a_save = menu.addAction("save");
    connect( a_save, SIGNAL(triggered() ), this, SLOT( saveFigure() ), Qt::DirectConnection);
    menu.exec(event->globalPos());

}
