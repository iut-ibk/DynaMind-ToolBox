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
#ifndef GUIMODELNODE_H
#define GUIMODELNODE_H

#include <QWidget>
#include <QMap>
#include <QGridLayout>
#include <module.h>
#include <modelnode.h>

#include "compilersettings.h"
class ModuleDescription;



class DM_HELPER_DLL_EXPORT  GUIModelNode : public QWidget
{
    Q_OBJECT
private:
    QMap<QString, QWidget *> elements;
    QGridLayout *layout1 ;
    ModuleDescription * moduleDescription;
    DM::Module * module;
    QMap<QString, QGridLayout * > UserDefinedContainer;
    QString GroupID;
    ModelNode * modelnode;




public:
    GUIModelNode(DM::Module * m, ModelNode * mn = 0, QWidget * parent = 0);

    ~GUIModelNode();


public slots:
    void accept();
    void reject();
    void help();
    void addUserDefinedItem();
    void addUserDefinedDoubleItem();
    void addUserDefinedTuple();
    void addTuplePort();
    void openFileDialog();
    void removeUserDefinedItem();
    void removeUserDefinedTuple();

signals:
    void selectFiles(QString s);

};

#endif // GUIMODELNODE_H
