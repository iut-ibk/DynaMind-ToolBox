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
#ifndef GUIEQUATIONEDITOR_H
#define GUIEQUATIONEDITOR_H

#include <QStringList>

#include <QDialog>

namespace Ui {
    class GUIEquationEditor;
}

class GUIEquationEditor : public QDialog
{
    Q_OBJECT

public:
    explicit GUIEquationEditor(QStringList SingelValues, QStringList Vectors , QWidget *parent = 0);
    void hideName();
    void addString(QString string);

    ~GUIEquationEditor();
signals:
    void values(QStringList values);
public slots:
    void addPlus() {this->addString("+");}
    void addMinus() {this->addString("-");}
    void addMulti() {this->addString("*");}
    void addCos() {this->addString("cos(#Value#)");}
    void addSin() {this->addString("sin(#Value#)");}
    void addTan() {this->addString("tan(#Value#)");}
    void addIf() {this->addString("if(Expression, #Value#, #Value#)");}
    void addNov() {this->addString("nov(#Vector#)");}
    void addRandom() {this->addString("rand(#Value#)");}
    void addVector();
    void addValue();
    void accept();
    void setName(QString);
    void setFormula(QString);

private:
    Ui::GUIEquationEditor *ui;
};

#endif // GUIEQUATIONEDITOR_H
