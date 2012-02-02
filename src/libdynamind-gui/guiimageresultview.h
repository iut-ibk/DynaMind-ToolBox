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
#ifndef GUIIMAGERESULTVIEW_H
#define GUIIMAGERESULTVIEW_H

#include <QWidget>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsView>
#include <QContextMenuEvent>
namespace Ui {
    class GUIImageResultView;
}
class QGraphicsPixmapItem;
class QGraphicsSvgItem;
class GUIImageResultView : public QWidget
{
    Q_OBJECT

public:
    explicit GUIImageResultView(QWidget *parent = 0);
    QGraphicsView * getView() {return view;}
    ~GUIImageResultView();
protected:
     void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::GUIImageResultView *ui;
    QGraphicsScene scene;
    QGraphicsView * view;
    int currentTime;
    int zvalues;
   // QVector<QGraphicsSvgItem * > images;
    QVector<QGraphicsSvgItem * > images;

public slots:
    void updateImage(QString);
    void setLastImage();
    void setFirstImage();
    void setImageUp();
    void setImageDown();
    void saveFigure();
};

#endif // GUIIMAGERESULTVIEW_H
