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

#define SWIG_PYTHON_THREADS

#include <Python.h>
#include "dmpythonenv.h"
#include <dmmoduleregistry.h>
#include <QDir>
#include <string>
#include <dmlog.h>
#include <dmlogger.h>
#include <dmlogsink.h>
#include <QSettings>
#include <algorithm>
#include <QProcessEnvironment>
#include <swigruntime.h>
#include <sstream>

using namespace DM;

void init() {
	DM::OStreamLogSink *sink = new DM::OStreamLogSink(cout);
	DM::Log::init(sink, DM::Debug);
}

void logdebug(std::string msg) {
	DM::Logger(DM::Debug) << msg;
}

void logwithlevel(std::string msg, DM::LogLevel logl) {
	DM::Logger(logl) << msg;
}

namespace DM
{
struct PythonEnvPriv {
	PyObject *main_module, *main_namespace;
};
}

PythonEnv *PythonEnv::instance = 0;

PythonEnv::PythonEnv() {
	priv = new PythonEnvPriv();

	if(!Py_IsInitialized()) {
		Py_Initialize();
		SWIG_PYTHON_INITIALIZE_THREADS;
		PyThreadState *pts = PyGILState_GetThisThreadState();
		PyEval_ReleaseThread(pts);
	} else {
		SWIG_PYTHON_INITIALIZE_THREADS;
	}

	SWIG_PYTHON_THREAD_BEGIN_BLOCK;
	PyObject *main = PyImport_ImportModule("__main__");
	priv->main_namespace = PyModule_GetDict(main);
	Py_DECREF(main);
	SWIG_PYTHON_THREAD_END_BLOCK;

#if defined(SWIG_PYTHON_THREADS)
	Logger(Standard) << "Pythonthreads enabled";
#else
	Logger(Standard) << "Pythonthreads disabled";
#endif
}

PythonEnv::~PythonEnv()
{
	if(!Py_IsInitialized())
		Py_Finalize();
	delete priv;
}

PythonEnv *PythonEnv::getInstance() {
	if (instance == 0) {
		instance = new PythonEnv();
	}

	return instance;
}

bool PythonEnv::addOverWriteStdCout() {
	QProcessEnvironment envvars =  QProcessEnvironment::systemEnvironment();

	if(envvars.contains("DYNAMIND_PYTHON"))
		Logger(Standard) << "DYNAMIND_PYTHON set";
	else
		Logger(Debug) << "DYNAMIND_PYTHON not set";

	addPythonPath(envvars.value("DYNAMIND_PYTHON","./").toStdString());
	addPythonPath("/usr/local/bin");
	addPythonPath("/usr/bin");
	addPythonPath("/usr/local/lib");
	addPythonPath("/usr/lib");

	ostringstream script;
#if defined(_WIN32)
	script << "import os\n";
	script << "if \"DYNAMIND_DIR\" in os.environ.keys():\n";
	script << "    os.environ[\"PATH\"] = os.environ[\"PATH\"]+\";\" + os.environ[\"DYNAMIND_DIR\"] + \"/DLLS\"\n";

#endif
	script << "import sys\n";
	script << "import pydynamind\n";
	script << "class Logger:\n";
	script << "    def __init__(self, stdout,error):\n";
	script << "        self.stdout = stdout\n";
	script << "        self.error=error\n";
	script << "        self.currentstring=\"\"\n";
	script << "\n";
	script << "    def flush(self):\n";
	script << "        pass\n";
	script << "\n";
	script << "    def write(self, text):\n";
	script << "        self.stdout.write(str(text))\n";
	script << "        self.currentstring = self.currentstring + \" \" + str(text)\n";
	script << "\n";
	script << "        if text.rfind(\"\\n\") == -1:\n";
	script << "                return\n";
	script << "\n";
	script << "        self.currentstring=self.currentstring.replace(\"\\n\",\"\")\n";
	script << "        self.currentstring=self.currentstring.replace(\"\\r\",\"\")\n";
	script << "        if self.error:\n";
	script << "                pydynamind.log(self.currentstring,pydynamind.Error)\n";
	script << "        else:\n";
	script << "                pydynamind.log(self.currentstring,pydynamind.Standard)\n";
	script << "        self.currentstring=\"\"\n";
	script << "\n";
	script << "    def close(self):\n";
	script << "        self.stdout.close()\n";
	script << "\n";
	script << "if not isinstance(sys.stdout,Logger):\n";
	script << "        sys.stdout=Logger(sys.stdout,False)\n";
	script << "        sys.stderr=Logger(sys.stderr,True)\n";
	script << "print \"Redirect python stdout and stderr\"\n";
	//script << "print sys.path\n";
	/*script << "import reimport\n";
	script << "import netCDF4\n";
	script << "import numpy\n";
	script << "import scipy\n";*/
	script << "print \"Python environment is ready for use\"\n";

	SWIG_PYTHON_THREAD_BEGIN_BLOCK;
	PyRun_String(script.str().c_str(), Py_file_input, priv->main_namespace, 0);
	if (PyErr_Occurred()) {
		printf("Error in PythonEnv::addOverWriteStdCout():\n");
		PyErr_Print();
		SWIG_PYTHON_THREAD_END_BLOCK;
		return false;
	}
	SWIG_PYTHON_THREAD_END_BLOCK;
	return true;
}

void PythonEnv::addPythonPath(std::string path) {
	ostringstream script;

	script << "import sys\n";

	script << "sys.path.append('" << path << "')\n";

	SWIG_PYTHON_THREAD_BEGIN_BLOCK;
	PyRun_String(script.str().c_str(), Py_file_input, priv->main_namespace, 0);
	if (PyErr_Occurred()) {
		PyErr_Print();
		SWIG_PYTHON_THREAD_END_BLOCK;
		return;
	}
	SWIG_PYTHON_THREAD_END_BLOCK;
}

bool PythonEnv::registerNodes(ModuleRegistry *registry, const string &module)
{
	SWIG_PYTHON_THREAD_BEGIN_BLOCK;
	PyObject *pydynamind_module = PyImport_ImportModule("pydynamind");
	if (PyErr_Occurred()) {
		//Logger(Error) << "Could not import pydynamind module";
		printf("Error in python module '%s':\n", module.c_str());
		PyErr_Print();
		SWIG_PYTHON_THREAD_END_BLOCK;
		return false;
	}

	bool exists = false;
	if (std::find(this->loadedModules.begin(), this->loadedModules.end(), module) !=  this->loadedModules.end()) {
		Logger(Debug) << "Module Already Loaded";
		exists = true;
	}

	QSettings settings;
	QString PathtoUrbanSim = settings.value("UrbanSim").toString().replace("\\","/");

	if(PathtoUrbanSim.size())
	{
		if (PathtoUrbanSim[PathtoUrbanSim.size()-1] == '/') {
			PathtoUrbanSim.remove(PathtoUrbanSim.size()-1,1);
		}
	}

	ostringstream script;
	script << "import reimport\n";
	script << "import sys\n";
	script << "import os\n";
	script << "import pydynamind\n";
	script << "import site\n";
	script << "import inspect\n";
	script << "sys.path.append('" << PathtoUrbanSim.toStdString() << "/src/')\n";
	script << "sys.path.append('" << PathtoUrbanSim.toStdString() << "/src/opus_core/tools')\n";
	script << "os.environ['PYTHONPATH']  = '" << PathtoUrbanSim.toStdString() << "/src/'\n";
	script << "os.environ['OPUS_HOME']   = '" << PathtoUrbanSim.toStdString() << "/src/'\n";

	std::string f = QDir::currentPath().toStdString();
	script << "site.addsitedir(\""<< QDir::currentPath().toStdString() << "\") \n";
	if (exists){
		script << "reimport.reimport(*reimport.modified())\n";
	}
	else
	{
		script << "__import__('" << module << "')\n";
	}

	PyRun_String(script.str().c_str(), Py_file_input, priv->main_namespace, 0);
	if (PyErr_Occurred())
	{
		PyErr_Print();
		SWIG_PYTHON_THREAD_END_BLOCK;
		return false;
	}

	PyObject *pydynamind_dict = PyModule_GetDict(pydynamind_module);
	Py_XDECREF(pydynamind_module);
	PyObject *callback = PyDict_GetItemString(pydynamind_dict, "registerNodes");

	swig_type_info *reg_type_info = SWIG_TypeQuery("ModuleRegistry *");

	PyObject *py_this = SWIG_NewPointerObj(registry, reg_type_info, 0);

	PyObject *args = Py_BuildValue("(O)", py_this);

	PyObject *res = PyObject_Call(callback, args, Py_None);
	Py_XDECREF(res);
	Py_XDECREF(py_this);
	Py_XDECREF(args);
	if (PyErr_Occurred())
	{
		PyErr_Print();
		SWIG_PYTHON_THREAD_END_BLOCK;
		return false;
	}

	loadedModules.push_back(module);
	SWIG_PYTHON_THREAD_END_BLOCK;
	return true;
}
