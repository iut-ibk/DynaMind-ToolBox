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
#ifndef PYENV_H_INCLUDED
#define PYENV_H_INCLUDED

#include <string>
#include <Registry.h>
#include <IFunction.h>
#include <IObjectiveFunction.h>
#include <ICalibrationAlg.h>
#include <IModelSimulator.h>
#include <CalimeroGlob.h>
#include <set>
#include <QThread>
#include <vector>
#include <boost/python/scope.hpp>

using namespace std;

struct PyEnvPriv;

class CALIMERO_PUBLIC PyEnv {
public:
        bool pythonmainthread;

        virtual ~PyEnv();
        static PyEnv *getInstance();
        static void destroy();
        void addPythonPath(std::string path);
        void registerFunctions(IRegistry *registry, const string &module,bool import = true);

private:
        PyEnv();
        PyEnvPriv *priv;
        static PyEnv *instance;
        set<string> loadedmodules;
};
bool execIteration(vector<CalibrationVariable*> calibrationparameters);
void barrier();
#endif // PYENV_H_INCLUDED
