/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of Calimero
 *
 * Copyright (C) 2011  Michael Mair

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
#include <Python.h>
#include <PyFunctionLoader.h>
#include <QDir>
#include <Exception.h>
#include <CalibrationEnv.h>
#include <PyEnv.h>

using namespace std;

void PyFunctionLoader::loadScripts(const string &path)
{
    QDir dir(QString::fromStdString(path));
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();
    PyEnv::getInstance()->addPythonPath(path);

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);

        QString file = fileInfo.fileName();

        if(!file.endsWith(".py"))
            continue;

        try
        {
            file = file.replace(".py","");
            Logger(Debug) << "Loading python module: " << file.toStdString();
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getCalibrationAlgReg(),file.toStdString(),true);
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getObjectiveFunctionReg(),file.toStdString(),false);
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getModelSimulatorReg(),file.toStdString(),false);
            PyEnv::getInstance()->registerFunctions(CalibrationEnv::getInstance()->getResultHandlerReg(),file.toStdString(),false);
        }
        catch (CalimeroException e)
        {
            Logger(Error) << e.exceptionmsg;
        }
    }
}
