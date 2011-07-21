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

#ifndef PYTHONENV_H
#define PYTHONENV_H
#include "compilersettings.h"
#include <string>
#include <vector>
//using namespace std;
namespace vibens {
class ModuleRegistry;
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}

struct PythonEnvPriv;

class VIBE_HELPER_DLL_EXPORT  ScopedGILRelease
{
public:
    ScopedGILRelease()
    {
             m_gstate = PyGILState_Ensure();
    }

    ~ScopedGILRelease()
    {
            PyGILState_Release(m_gstate);
    }

private:
    PyGILState_STATE m_gstate;
};

class VIBE_HELPER_DLL_EXPORT PythonEnv {
public:
        virtual ~PythonEnv();
        static PythonEnv *getInstance();
        static void freeInstance();
        void addPythonPath(std::string path);
        std::string registerNodes(ModuleRegistry *registry,
                                  const std::string &module);
        void addOverWriteStdCout();
        void startEditra(std::string = "");
private:
        PythonEnv();
        PythonEnvPriv *priv;
        static PythonEnv *instance;
        std::vector<std::string> loadedModules;
};

void handle_python_exception();
}
#endif
#endif // PYTHONENV_H
