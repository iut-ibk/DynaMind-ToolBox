/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

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
#ifndef GUICELLULARAUTOMATA_H
#define GUICELLULARAUTOMATA_H

#include <dmcompilersettings.h>
#include <QDialog>
#include <QStringList>

namespace DM{
    class Module;
}
namespace Ui {
    class GUICellularAutomata;
}

class CellularAutomata;

class DM_HELPER_DLL_EXPORT GUICellularAutomata : public QDialog
{
    Q_OBJECT

public:
    explicit GUICellularAutomata(DM::Module * m ,QWidget *parent = 0);
    DM::Module * getModule() {return (DM::Module * )this->m;}
    ~GUICellularAutomata();


private:
    Ui::GUICellularAutomata *ui;
    CellularAutomata * m;
    QStringList variables;
    QStringList values;
    QStringList vectors;
    QStringList rules;

    bool checkIfFromOutSide(QString name);

public slots:
    void addRasterData();
    void addLandscape(QString);
    void addVariable();
    void addFormula();
    void addExpression();
    void editRule();
    void addExpressiontoVIBe(QStringList list);
    void updateEntries();
    void addRule(QStringList list);
    void accept();
    void on_pushButton_remove_landscape_clicked();
    void on_pushButton_remove_neigh_clicked();
    void on_pushButton_remove_rules_clicked();

};

#endif // GUICELLULARAUTOMATA_H
