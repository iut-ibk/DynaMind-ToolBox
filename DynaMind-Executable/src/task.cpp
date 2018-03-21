#include "task.h"


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
#include <QUuid>

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


bool removeDir(const QString & dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeDir(info.absoluteFilePath());
			}
			else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(dirName);
	}
	return result;
}

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
			if(QFile::exists(sourcefile))
			{
				QFile::copy(sourcefile,targetfile);
				QFile::remove(sourcefile);
				DM::Logger(DM::Standard) << "Copy: " << sourcefile.toStdString() << " -> " << targetfile.toStdString();
			}
        }
    }
}

string replacestrings(string &replace, string projectfilepath)
{
    QStringList replacelist = QString::fromStdString(replace).split(";");
    QString tmpfile = QString::fromStdString(projectfilepath)+"_tmp_" + QUuid::createUuid().toString().mid(1,36) + ".dyn";

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
            std::cout << "Replace in inputfile: " << source.toStdString() << " to " << target.toStdString() << std::endl;
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
        QStringList keyvalueList = overloading.split(QRegExp("\\="), QString::SkipEmptyParts);
        QStringList overloadingList;

        if(keyvalueList.size() == 2)
        {
            overloadingList = keyvalueList[0].split(QRegExp("\\."), QString::SkipEmptyParts);

            QString value = "";
            for(int index=1; index < keyvalueList.size(); index++)
                value+=keyvalueList.at(index);

            overloadingList.push_back(value);
        }

        // used for modules with '=' in values e.g. "Export GIS data" with postGIS.sink=PG:dbname=dbName host=127.0.0.1 port=5432 user=postgre password=password)
        //otherwise the '=' would be removed
        if(keyvalueList.size() > 2)
        {

            QString key = overloading.section(QRegExp("\\="),0,0);
            overloadingList = key.split(QRegExp("\\."), QString::SkipEmptyParts);
            QString value = overloading.section(QRegExp("\\="),1);
            overloadingList.push_back(value);
        }

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

void showParameters(DM::Simulation* sim)
{
    cout << "--parameters ";
    foreach(DM::Module* modIt, sim->getModules())
    {
        std::string modName = modIt->getName();
        foreach(DM::Module::Parameter* p, modIt->getParameters())
            cout << modName << "." << p->name << "=" << modIt->getParameterAsString(p->name) << "\n";
    }
    cout << endl;
}



std::string Task::unzipDataStructure(const string &workspace_uuid, const std::string & simulationfile)
{
	QProcess p;
	QStringList arguments;
	arguments << QString::fromStdString(simulationfile);
	arguments << "-d /tmp/"  + QString::fromStdString(workspace_uuid);
	DM::Logger(DM::Error) << arguments.join(" ").toStdString();
	p.start("/usr/bin/unzip " + arguments.join(" "));
	p.waitForFinished();

	QString cleanFileName = QString::fromStdString(simulationfile).split("/").last();
	//Create new simualtion_filename
	DM::Logger(DM::Warning) <<  QFile(QString::fromStdString(simulationfile)).fileName().toStdString();
	QString updatedSimulationFolder =  "/tmp/"
			+  QString::fromStdString(workspace_uuid)
			+ "/" + cleanFileName;
	updatedSimulationFolder.replace(".zip", "");
	return updatedSimulationFolder.toStdString();
}

Task::~Task()
{
	if (this->unzippedFolder.empty())
			return;
	DM::Logger(DM::Standard) << "Remove data folder " << this->unzippedFolder;
	removeDir(QString::fromStdString(this->unzippedFolder));

}

void Task::run()
{
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
            ("logpath", po::value<string>(), "Save path for log file")
            ("ompthreads", po::value<int>(), "number of threads used by omp")
            ("settings", po::value<string>(), "set an environment variable")
            ("show-settings", "show environment variables")
            //("python-modules", po::value<vector <string> >(), "set path to python modules")
            ("parameter", po::value<string>(), "overwrites a parameter: ([modulename].[parametername]=[value];")
            ("parameterlist", "shows the available parameters for this file")
            ("with-status-updates", "print custom status updates")
            ("version", "shows the current version of the dynamind core")
            ("license", "shows used license of the dynamind core")
            ("author", "shows the names of the core programmers")
            ;


    std::string simulationfile, realsimulationfile;
    std::vector<std::string> pythonModules;
	std::string workspace_uuid = QUuid::createUuid().toString().replace("{", "").replace("}", "").toStdString();
    int repeat = 1;
    bool verbose = false;
    bool withStatusUpdates = false;
    string cpfile = "";
    string replace = "";
    string parameteroverloads = "";
    int numThreads = 1;
    string lf = "";

    DM::LogLevel ll = DM::Standard;
    try
    {
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        verbose = vm.count("verbose");
        withStatusUpdates = vm.count("with-status-updates");

        if (vm.count("settings")) {
            setSettings(vm["settings"].as<string>());
            QCoreApplication::exit(1);
            return;
        }

        if (vm.count("help"))
        {
            cout << desc << "\n";
            QCoreApplication::exit(1);
            return;
        }

        if (vm.count("show-settings"))
        {
            showSettings();
            QCoreApplication::exit(1);
            return;
        }

        if (vm.count("version"))
        {
            cout << DM::Core::getCoreVersion() << "\n";
            QCoreApplication::exit(1);
            return;
        }

        if (vm.count("author"))
        {
            cout << DM::Core::getAuthor() << "\n";
            QCoreApplication::exit(1);
            return;
        }

        if (vm.count("license"))
        {
            cout << DM::Core::getLicense() << "\n";
            QCoreApplication::exit(1);
            return;
        }




        if (vm.count("input-file"))
			simulationfile =  vm["input-file"].as<string>();
        else
        {
            std::cout << "Simulation file not set" << std::endl;
            cout << desc << "\n";
            QCoreApplication::exit(-1);
            return;
        }
        //if (vm.count("python-modules")) {
        //    pythonModules =  vm["python-modules"].as<vector <string> >();
        //}
        if (vm.count("repeat"))		repeat = vm["repeat"].as<int>();

        if (vm.count("cpfile"))     cpfile = vm["cpfile"].as<string>();

        if (vm.count("replace"))     replace = vm["replace"].as<string>();

        if (vm.count("parameter"))     parameteroverloads = vm["parameter"].as<string>();

        if (vm.count("loglevel"))		ll = (DM::LogLevel)vm["loglevel"].as<int>();

        if (vm.count("logpath"))		lf = vm["logpath"].as<string>();

        QDateTime time = QDateTime::currentDateTime();
        QString logfilepath = QDir::tempPath() + "/dynamind" + time.toString("_yyMMdd_hhmmss_zzz")+".log";

        if(QString::fromStdString(lf)!="")
            logfilepath = QString::fromStdString(lf);

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
        cout << "unknown option:" << desc << "\n";
        QCoreApplication::exit(-1);
        return;
    }

    if(!QFile::exists(QString::fromStdString(simulationfile)))
    {
        DM::Logger(DM::Error) << "simulation file not found";
        QCoreApplication::exit(-1);
        return;
    }

	bool loadedFromArchive;
	loadedFromArchive = false;


	if(simulationfile.substr(simulationfile.size() - 4, simulationfile.size()) == ".zip") {
		DM::Logger(DM::Standard) << "Extract simulation file " << simulationfile;
		this->unzippedFolder = Task::unzipDataStructure(workspace_uuid, simulationfile);
		DM::Logger(DM::Standard) << "Using simulation folder " << this->unzippedFolder;
		QString cleanFileName = QString::fromStdString(simulationfile).split("/").last();
		simulationfile = this->unzippedFolder + "/" + cleanFileName.replace(".zip", ".dyn").toStdString();
		DM::Logger(DM::Standard) << "Dynamind name "<< simulationfile;
		loadedFromArchive = true;

	}

#ifdef _OPENMP
    omp_set_num_threads(numThreads);
#endif

    DM::Simulation s;
    if (withStatusUpdates) {
        stringstream path;
        path << "dynamindstatus-" << QCoreApplication::applicationPid() << ".log";
        s.installStatusUpdater(path.str());
    }

    s.registerModulesFromDefaultLocation();
    s.registerModulesFromSettings();
    realsimulationfile = replacestrings(replace, simulationfile);

	if (loadedFromArchive) {
		DM::Logger(DM::Standard) << "Load modules from " << unzippedFolder + "/modules";
		s.registerModulesFromDirectory(QDir(QString::fromStdString(unzippedFolder + "/modules")));
	}

    s.loadSimulation(realsimulationfile);
    OverloadParameters(&s, parameteroverloads);


    if (vm.count("parameterlist"))
    {
        showParameters(&s);

        if (simulationfile != realsimulationfile)
            QFile::remove(QString::fromStdString(realsimulationfile));

        QCoreApplication::exit(1);
        return;
    }

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

    if(repeat)
        avg /= times.size();

    qint64 sigma = 0;
    foreach(qint64 i, times)
        sigma += (avg-i)*(avg-i)/max(1,repeat-1);
    sigma = sqrt((float)sigma);

    DM::Logger(DM::Standard) << ">>>> finished simulation at an average of " << (long)avg << "+-"<< (long)sigma << "ms";

    // cleanup
    if(simulationfile!=realsimulationfile)
        QFile::remove(QString::fromStdString(realsimulationfile));

    QThreadPool::globalInstance()->waitForDone();

    emit finished();
}

