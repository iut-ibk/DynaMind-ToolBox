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
#include <Python.h>
#include "pythonenv.h"
#include <moduleregistry.h>
#include "pythonnodefactory.h"
//#include <pymodule.h>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <QDir>
#include <boost/python.hpp>
#include <string>
#include <vibe_log.h>
#include <vibe_logger.h>
#include <vibe_logsink.h>
#include <QSettings>
#include <algorithm>

#include <swigruntime.h>

extern "C" {
void init_pydynamite(void);
}

using namespace boost::python;

void init() {
    vibens::OStreamLogSink *sink = new vibens::OStreamLogSink(cout);
    vibens::Log::init(sink, vibens::Debug);
}

void logdebug(std::string msg) {
    vibens::Logger(vibens::Debug) << msg;
}

void logwithlevel(std::string msg, vibens::LogLevel logl) {
    vibens::Logger(logl) << msg;
}

namespace vibens {


struct PythonEnvPriv {
    PyObject *main_module, *main_namespace;
};

PythonEnv *PythonEnv::instance = 0;

PythonEnv::PythonEnv() {
    priv = new PythonEnvPriv();

    if(!Py_IsInitialized()) {
        Py_Initialize();
        SWIG_PYTHON_INITIALIZE_THREADS;
        init_pydynamite();
        PyThreadState *pts = PyGILState_GetThisThreadState();
        PyEval_ReleaseThread(pts);
    }

    SWIG_PYTHON_THREAD_BEGIN_BLOCK;
    PyObject *main = PyImport_ImportModule("__main__");
    priv->main_namespace = PyModule_GetDict(main);
    Py_DECREF(main);

    #if defined(SWIG_PYTHON_THREADS)
        Logger(Standard) << "Pythonthreads enabled";
    #else
        Logger(Standard) << "Pythonthreads disabled";
    #endif

    #if defined(OPENMP_ENABLED)
        Logger(Standard) << "OMP enabled";
    #else
        Logger(Standard) << "OMP disabled";
    #endif
}

PythonEnv::~PythonEnv() {
    delete priv;
}

PythonEnv *PythonEnv::getInstance() {
    if (instance == 0) {
        instance = new PythonEnv();
    }

    return instance;
}

void PythonEnv::freeInstance() {
    /*if (instance == 0)
        return;
    ScopedGILRelease scoped;
    exec("import gc\n"
         "print gc.collect()\n"
         "print 'where is the garbage'\n"
         "print gc.garbage\n",
         instance->priv->main_namespace,instance->priv->main_namespace);

    delete instance;
    Py_Finalize();
    */
}

void PythonEnv::addOverWriteStdCout() {
    /*
    boost::format fmt( "import sys\n"
                      "import pyvibe\n"
                      "class Logger_VIBe:\n"
                      "    def __init__(self, stdout,error):\n"
                      "        self.stdout = stdout\n"
                      "        self.error=error\n"
                      "        self.currentstring=\"\"\n"
                      "\n"
                      "    def write(self, text):\n"
                      "        self.stdout.write(text)\n"
                      "        self.currentstring = self.currentstring + \" \" + text\n"
                      "\n"
                      "        if text.rfind(\"\\n\") == -1:\n"
                      "                return\n"
                      "\n"
                      "        self.currentstring=self.currentstring.replace(\"\\n\",\"\")\n"
                      "        self.currentstring=self.currentstring.replace(\"\\r\",\"\")\n"
                      "        if self.error:\n"
                      "                pyvibe.log(str(self.currentstring))\n"
                      "        else:\n"
                      "                pyvibe.log(str(self.currentstring))\n"
                      "        self.currentstring=\"\"\n"
                      "\n"
                      "    def close(self):\n"
                      "        self.stdout.close()\n"
                      "    def flush(self):\n"
                      "         pass\n"
                      "\n"
                      "if not isinstance(sys.stdout,Logger_VIBe):\n"
                      "        sys.stdout=Logger_VIBe(sys.stdout,False)\n"
                      "        sys.stderr=Logger_VIBe(sys.stderr,True)\n");

    SWIG_PYTHON_THREAD_BEGIN_BLOCK;
    PyRun_String(fmt.str().c_str(), Py_file_input, priv->main_namespace, 0);
    if (PyErr_Occurred()) {
        PyErr_Print();
        return;
    }
    */
}

void PythonEnv::addPythonPath(std::string path) {
    boost::format fmt("import sys\n"
                      "sys.path.append('%1%')\n");
    fmt % path;
    SWIG_PYTHON_THREAD_BEGIN_BLOCK;
    PyRun_String(fmt.str().c_str(), Py_file_input, priv->main_namespace, 0);
    if (PyErr_Occurred()) {
        PyErr_Print();
        return;
    }
}
void PythonEnv::startEditra(std::string filename) {
    SWIG_PYTHON_THREAD_BEGIN_BLOCK;
    QSettings settings("IUT", "VIBe2");
    QString PathtoEditra = settings.value("Editra").toString().replace("\\","/");
    if (PathtoEditra[PathtoEditra.size()-1] == '/') {
        PathtoEditra.remove(PathtoEditra.size()-1,1);
    }
    object main_module = import("__main__");
    object main_namespace = main_module.attr("__dict__");

    ostringstream skript;
    skript << "import cStringIO\n";
    skript << "import sys\n";
    skript << "import os\n";
    skript << "import pyvibe\n";
    skript << "import site\n";

    skript << "sys.stderr = cStringIO.StringIO()\n";
    skript << "sys.path.append('" << PathtoEditra.toStdString() << "')\n";
    skript << "import site\n";
    skript << "import src.Editra\n";
    skript << "src.Editra.Main('"<< filename <<"')\n";


    SWIG_PYTHON_THREAD_BEGIN_BLOCK;
    PyRun_String(skript.str().c_str(), Py_file_input, priv->main_namespace, 0);
    if (PyErr_Occurred()) {
        PyErr_Print();
        return;
    }
}

std::string PythonEnv::registerNodes(ModuleRegistry *registry, const string &module)
{
    SWIG_PYTHON_THREAD_BEGIN_BLOCK;
    PyObject *pydynamite_module = PyImport_ImportModule("pydynamite");
    if (PyErr_Occurred()) {
        Logger(Error) << "Could not import pycalimero module";
        PyErr_Print();
        return module;
    }

    bool exists = false;
    if (std::find(this->loadedModules.begin(), this->loadedModules.end(), module) !=  this->loadedModules.end()) {
        Logger(Debug) << "Moduel Already Loaded";
        exists = true;
    }

    QSettings settings("IUT", "VIBe2");
    QString PathtoUrbanSim = settings.value("UrbanSim").toString().replace("\\","/");
    if (PathtoUrbanSim[PathtoUrbanSim.size()-1] == '/') {
        PathtoUrbanSim.remove(PathtoUrbanSim.size()-1,1);
    }
    ostringstream skript;
    skript << "import reimport\n";
    skript << "import cStringIO\n";
    skript << "import sys\n";
    skript << "import os\n";
    skript << "import pydynamite\n";
    skript << "import site\n";
    skript << "import inspect\n";
    skript << "sys.stderr = cStringIO.StringIO()\n";
    skript << "sys.path.append('" << PathtoUrbanSim.toStdString() << "/src/')\n";
    skript << "sys.path.append('" << PathtoUrbanSim.toStdString() << "/src/opus_core/tools')\n";
    skript << "os.environ['PYTHONPATH']   = '" << PathtoUrbanSim.toStdString() << "/src/'\n";
    skript << "os.environ['OPUS_HOME']   = '" << PathtoUrbanSim.toStdString() << "/src/'\n";

    std::string f = QDir::currentPath().toStdString();
    skript << "site.addsitedir(\""<< QDir::currentPath().toStdString() << "\") \n";
    if (exists){
        skript << "reimport.reimport(*reimport.modified())\n";
    }
    else
        skript << "__import__('" << module << "')\n";
    skript << "clss = pydynamite.Module.__subclasses__()\n";
    //skript << "print clss\n";

    PyRun_String(skript.str().c_str(), Py_file_input, priv->main_namespace, 0);
    if (PyErr_Occurred())
    {
        PyErr_Print();
        //throw CalimeroException("Error in python module: " + module);
        return module;
    }


    PyObject *pydynamite_dict = PyModule_GetDict(pydynamite_module);
    Py_XDECREF(pydynamite_module);
    PyObject *callback = PyDict_GetItemString(pydynamite_dict, "registerFunctions");

    swig_type_info *reg_type_info = SWIG_TypeQuery("ModulRegistry *");

    PyObject *py_this = SWIG_NewPointerObj(registry, reg_type_info, 0);

    PyObject *args = Py_BuildValue("(O)", py_this);

    PyObject *res = PyObject_Call(callback, args, Py_None);
    Py_XDECREF(res);
    Py_XDECREF(py_this);
    Py_XDECREF(args);
    if (PyErr_Occurred())
    {
        PyErr_Print();
        return module;
    }

    loadedModules.push_back(module);

    return module;
}

void handle_python_exception() {
    try {
        throw ;
    } catch(boost::python::error_already_set const &) {
        PyObject* type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);
        boost::python::handle<> ty(type), v(value), tr(traceback);
        boost::python::str format("%s|%s|%s");//fucking dirty hack because python error handling sucks soooo much
        boost::python::object ret = format % boost::python::make_tuple(ty, v, tr);
        string error = boost::python::extract<string>(ret);
        Logger(Error) << error;
        std::cout  << error << std::endl;
        throw ;
    }
}
}
