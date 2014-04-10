/**
* @file
* @author  Markus Sengthaler
* @version 1.0
* @section LICENSE
*
* This file is part of DynaMind
*
* Copyright (C) 2011  Markus Sengthaler

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
//#define BOOST_ALL_DYN_LINK
#include <dmcompilersettings.h>

#include <iostream>
#include <vector>
#include <boost/program_options.hpp>
//#include <boost/filesystem.hpp>

#include "dmsimulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <QThreadPool>
#include <QCoreApplication>
#include <QSettings>
#include <QStringList>

#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>
#include <dmdbconnector.h>
#include <dmpythonenv.h>

#include <fstream>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;
//using namespace boost::python;
namespace po = boost::program_options;
//using namespace vibens;
typedef boost::error_info<struct tag_errno,int> errno_info;
typedef boost::error_info<struct errinfo_type_info_name_,std::string> errinfo_type_info_name;


void copyfiles(string &cpfile, int iteration)
{
	QStringList filelist = QString::fromStdString(cpfile).split(";");

	foreach(QString file, filelist)
	{
		QStringList transfer = file.split(",");
		if(transfer.size() == 2)
		{
			QString targetfile = transfer.at(1);
			targetfile = targetfile.replace("DMITERATION",QString::number(iteration));
			QString sourcefile = transfer.at(0);
            QFile::remove(targetfile);
			QFile::copy(sourcefile,targetfile);
			DM::Logger(DM::Standard) << "Copy: " << sourcefile.toStdString() << " -> " << targetfile.toStdString();
		}
	}
}

string replacestrings(string &replace, string projectfilepath)
{
    QStringList replacelist = QString::fromStdString(replace).split(";");
    QString tmpfile = QString::fromStdString(projectfilepath)+"_tmp.dyn";

    if(replace=="")
        return projectfilepath;

    QFile file(QString::fromStdString(projectfilepath));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return projectfilepath;

    QTextStream in(&file);

    QString content = in.readAll();
    file.close();

    foreach(QString re, replacelist)
    {
        QStringList r = re.split(",");
        if(r.size() == 2)
        {
            QString target = r.at(1);
            QString source = r.at(0);
            content  = content.replace(source,target);
            std::cout << "Replace in inputfile: " << source.toStdString() << " to " << target.toStdString();
        }
    }

    QFile ofile(tmpfile);
    if (!ofile.open(QIODevice::WriteOnly | QIODevice::Text))
        return projectfilepath;

    QTextStream out(&ofile);
    out << content;
    ofile.close();

    return tmpfile.toStdString();
}

void setSettings(std::string s) 
{
	QStringList avalible_settings;
	avalible_settings << "SWMM";
	QString setting = QString::fromStdString(s);
	QStringList sl = setting.split("=");
	if (sl.size() != 2) 
	{
		std::cout << "Error: couldn't set setting " << s << " format unknown. Format should be NAMEOFSETTING=setting"<< std::endl;
		return;
	}

	if (avalible_settings.indexOf(sl[0]) == -1) 
	{
		std::cout << "Unknown setting avalible settings are:" << std::endl;
		foreach (QString as, avalible_settings)
			std::cout << as.toStdString() << std::endl;
		return;
	}
	QSettings settings;
	settings.setValue(sl[0], sl[1]);
}

void showSettings() 
{
	QSettings settings;

	QStringList avalible_settings;
	avalible_settings << "SWMM";

	foreach (QString as, avalible_settings) 
	{
		QString val = settings.value(as).toString();
		std::cout << val.toStdString() << std::endl;
	}
}

void OverloadParameters(DM::Simulation* sim, const std::string& parameteroverloads)
{
	QStringList overloadlist = QString::fromStdString(parameteroverloads).split(";", QString::SkipEmptyParts);

	foreach(const QString& overloading, overloadlist)
	{
		QStringList overloadingList = overloading.split(QRegExp("\\.|\\="), QString::SkipEmptyParts);
		if (overloadingList.size() != 3)
			DM::Logger(DM::Error) << "wrong format in parameterstring: " << overloading.toStdString() << overloadingList.size();
		else
		{
			std::string moduleName = overloadingList.at(0).toStdString();
			std::string parameterName = overloadingList.at(1).toStdString();
			std::string value = overloadingList.at(2).toStdString();

			DM::Module* module = NULL;
			foreach(DM::Module* modIt, sim->getModules())
			{
				if (modIt->getName() == moduleName)
				{
					module = modIt;
					break;
				}
			}
			if (!module)
			{
				DM::Logger(DM::Error) << "module '" << moduleName << "' not found";
				continue;
			}
			if (!module->getParameter(parameterName))
			{
				DM::Logger(DM::Error) << "parameter '" << parameterName << "' not found in module '" << moduleName << "'";
				continue;
			}
			module->setParameterValue(parameterName, value);
		}
	}
}

int main(int argc, char *argv[], char *envp[]) 
{
	QCoreApplication::setOrganizationName("IUT");
	QCoreApplication::setApplicationName("DYNAMIND");

	QThreadPool::globalInstance()->setMaxThreadCount(1);

	po::positional_options_description p;
	p.add("input-file", -1);

	po::variables_map vm;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("input-file", po::value<string>(), "set simulation file")
        ("replace", po::value<string>(), "replace string in input file: ([STRING],[STRING];)*")
		("repeat", po::value<int>(), "repeat simulation")
		("cpfile", po::value<string>(), "Copy generated files: ([SOURCEFILE],[TARGETPATH]DMITERATION[FILENAME];)* ")
		("verbose", "verbose output")
		("sqlquerystack", po::value<unsigned long>(), "sql query cache size")
		("blockwriting", po::value<unsigned long>(), "sql write block size")
		("loglevel", po::value<int>(), "logger level 0-3 (Debug-Standard-Warning-Error)")
		("ompthreads", po::value<int>(), "number of threads used by omp")
		("settings", po::value<string>(), "set an environment variable")
		("show-settings", "show environment variables")
		//("python-modules", po::value<vector <string> >(), "set path to python modules")
		("parameter", po::value<string>(), "overwrites a parameter: ([modulename].[parametername]=[value];")
		;


    std::string simulationfile, realsimulationfile;
	std::vector<std::string> pythonModules;
	int repeat = 1;
	bool verbose = false;
	string cpfile = "";
    string replace = "";
	string parameteroverloads = "";
	int numThreads = 1;

	DM::LogLevel ll = DM::Standard;
	try 
	{
		po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
		po::notify(vm);

		verbose = vm.count("verbose");

		if (vm.count("settings")) {
			setSettings(vm["settings"].as<string>());
			return 1;
		}

		if (vm.count("help"))
		{
			cout << desc << "\n";
			return 1;
		}
		if (vm.count("show-settings"))
		{
			showSettings();
			return 1;
		}

		if (vm.count("input-file"))
			simulationfile =  vm["input-file"].as<string>();
		else
		{
			std::cout << "Simulation file not set" << std::endl;
			cout << desc << "\n";
			return -1;
		}
		//if (vm.count("python-modules")) {
		//    pythonModules =  vm["python-modules"].as<vector <string> >();
		//}
		if (vm.count("repeat"))		repeat = vm["repeat"].as<int>();

		if (vm.count("cpfile"))     cpfile = vm["cpfile"].as<string>();

		if (vm.count("replace"))     replace = vm["replace"].as<string>();

		if (vm.count("parameter"))     parameteroverloads = vm["parameter"].as<string>();
		
		if (vm.count("loglevel"))		ll = (DM::LogLevel)vm["loglevel"].as<int>();

		QDateTime time = QDateTime::currentDateTime();
		QString logfilepath = QDir::tempPath() + "/dynamind" + time.toString("_yyMMdd_hhmmss_zzz")+".log";

		if(QFile::exists(logfilepath))
			QFile::remove(logfilepath);

		std::ofstream* outputFile = new ofstream();
		outputFile->open(logfilepath.toStdString().c_str());
		DM::OStreamLogSink* file_log_updater = new DM::OStreamLogSink(*outputFile);

		//ostream *out = &cout;
		DM::Log::init(new DM::OStreamLogSink(cout), ll);
		DM::Log::addLogSink(file_log_updater);
		//DM::Log::init(file_log_updater, ll);

		//Init Python
		DM::PythonEnv *env = DM::PythonEnv::getInstance();
		//env->addPythonPath(boost::filesystem::canonical(boost::filesystem::current_path()).string());
		env->addPythonPath("");

		DM::DBConnectorConfig cfg;
		if(vm.count("sqlquerystack"))	cfg.queryStackSize		 = vm["sqlquerystack"].as<unsigned long>();
		if(vm.count("blockwriting"))	cfg.cacheBlockwritingSize = vm["blockwriting"].as<unsigned long>();
		DM::DBConnector::getInstance()->setConfig(cfg);

		if(vm.count("ompthreads"))	numThreads = vm["ompthreads"].as<int>();

		if(verbose)
		{
			DM::Logger(DM::Standard) << ">>>> config:";
			DM::Logger(DM::Standard) << "sql query stack size: " << (long)cfg.queryStackSize;
			DM::Logger(DM::Standard) << "sql write block size: " << (long)cfg.cacheBlockwritingSize;
			DM::Logger(DM::Standard) << "num threads: " << numThreads;
		}
	}
	catch (po::unknown_option & e)
	{
		DM::Logger(DM::Error) << "unknown option";
		cout << desc << "\n";
	}

	if(!QFile::exists(QString::fromStdString(simulationfile)))
	{
		DM::Logger(DM::Error) << "simulation file not found";
		return -1;
	}

#ifdef _OPENMP
	omp_set_num_threads(numThreads);
#endif

	DM::Simulation s;

	s.registerModulesFromDefaultLocation();
	s.registerModulesFromSettings();
	realsimulationfile = replacestrings(replace, simulationfile);

	s.loadSimulation(realsimulationfile);
	OverloadParameters(&s, parameteroverloads);

	DM::Logger(DM::Standard) << ">>>> starting simulation";

	std::list<qint64> times;

	QElapsedTimer timer;
	for(int i=0;i<repeat;i++)
	{
		if(verbose && repeat>1)
			DM::Logger(DM::Standard) << ">>>> iteration " << i;
		s.resetSimulation();
		timer.start();
		s.run();
		times.push_back(timer.elapsed());
		copyfiles(cpfile, i);
		if(verbose && repeat>1)
			DM::Logger(DM::Standard) << ">>>> took " << (long)timer.elapsed() << "ms";
	}
	qint64 avg = 0;
	foreach(qint64 i, times)
		avg += i;
	avg /= times.size();

	qint64 sigma = 0;
	foreach(qint64 i, times)
		sigma += (avg-i)*(avg-i)/max(1,repeat-1);
	sigma = sqrt((float)sigma);

	DM::Logger(DM::Standard) << ">>>> finished simulation at an average of " << (long)avg << "+-"<< (long)sigma << "ms";

    if(simulationfile!=realsimulationfile)
        QFile::remove(QString::fromStdString(realsimulationfile));

	QThreadPool::globalInstance()->waitForDone();
}
