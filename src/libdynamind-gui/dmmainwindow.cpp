/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * DAnCE4Water (Dynamic Adaptation for eNabling City Evolution for Water) is a
 * strategic planning tool ti identify the transtion from an from a conventional
 * urban water system to an adaptable and sustainable system at a city scale
 *
 * Copyright (C) 2011  Christian Urich, Markus Sengthaler

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
 * @section DESCRIPTION
 *
 * The class contains the main windows for the DAnCE4Water GUI
 */

#include "dmmainwindow.h"
//#include <boost/python.hpp>


#include "QDir"
#include "QThread"
#include "QTreeWidgetItem"
#include "QFileDialog"
#include "QSettings"
#include "modulereader.h"
#include <sstream>
#include "guilink.h"
#include <QTreeWidget>
#include "simulationmanagment.h"
#include <projectviewer.h>
#include <modelnode.h>
#include <dmmodule.h>
#include <dmgroup.h>
#include <dmsimulation.h>
#include <groupnode.h>
#include <QWidget>
#include <QThreadPool>
#include <dmlog.h>
#include <guilogsink.h>
#include <dmpythonenv.h>
#include <dmsimulationobserver.h>
#include <guisimulationobserver.h>
#include <guisimulation.h>
#include <rootgroupnode.h>
#include "preferences.h"
#include "projectviewer.h"
#include "guihelpviewer.h"
#include "guiaboutdialog.h"

#include <dmmoduleregistry.h>
#include <qgraphicsview.h>

void DMMainWindow::ReloadSimulation() {
	/*
    this->simulation->reloadModules();
    createModuleListView();
	*/
}

void DMMainWindow::startEditor() {
    //DM::PythonEnv::getInstance()->startEditra();
}

void DMMainWindow::newLogLine(QString line)
{
	if(line.startsWith("DEBUG"))
		ui->log_widget->setTextColor(QColor(100,255,100));
	else if(line.startsWith("ERROR"))
		ui->log_widget->setTextColor(QColor(255,50,50));
	else if(line.startsWith("WARN"))
		ui->log_widget->setTextColor(QColor(255,255,50));
	else
		ui->log_widget->setTextColor(QColor(255,255,255));

	ui->log_widget->append(line);
}

void DMMainWindow::updateProgress(float progress)
{
	ui->progressBar->setValue((int)(progress*100));
}

DMMainWindow::DMMainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::DMMainWindow)
{
	// qt init
    Q_INIT_RESOURCE(icons);
    ui->setupUi(this);
    this->setParent(parent);

	// logger init
    log_updater = new GuiLogSink();
	connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(newLogLine(QString)), Qt::QueuedConnection);
#if defined DEBUG || _DEBUG
    DM::Log::init(log_updater,DM::Debug);
#else
    DM::Log::init(log_updater,DM::Standard);
#endif
	// add log export to file
    QString logfilepath = QDir::tempPath() + "/dynamind" 
		+ QDateTime::currentDateTime().toString("_yyMMdd_hhmmss_zzz")+".log";
    if(QFile::exists(logfilepath))
        QFile::remove(logfilepath);

    outputFile = new ofstream(logfilepath.toStdString());
    DM::Log::addLogSink(new DM::OStreamLogSink(*outputFile));
	DM::Logger() << "logfile: " << logfilepath;
	
	// init python env
    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    env->addPythonPath(QApplication::applicationDirPath().toStdString());
    env->addOverWriteStdCout();

	// init simulation, we only use one instance
    this->simulation = new GUISimulation(parent, ui->tabWidget_4);
	simulationThread = NULL;

    this->simulation->registerModulesFromDefaultLocation();
    this->simulation->registerModulesFromSettings();
    createModuleListView();

    connect( ui->actionRun, SIGNAL( triggered() ), this, SLOT( runSimulation() ), Qt::DirectConnection );
    connect( ui->actionPreferences, SIGNAL ( triggered() ), this, SLOT(preferences() ), Qt::DirectConnection );
    connect(ui->actionSave, SIGNAL(triggered()), this , SLOT(saveSimulation()), Qt::DirectConnection);
    connect(ui->actionSaveAs, SIGNAL(triggered()), this , SLOT(saveAsSimulation()), Qt::DirectConnection);
    connect(ui->actionOpen, SIGNAL(triggered()), this , SLOT(loadSimulation()), Qt::DirectConnection);
    connect(ui->actionNew, SIGNAL(triggered()), this , SLOT(clearSimulation()), Qt::DirectConnection);
    connect(ui->actionEditor, SIGNAL(triggered()), this , SLOT(startEditor()), Qt::DirectConnection);
    connect(ui->actionReload_Modules, SIGNAL(triggered()), this , SLOT(ReloadSimulation()), Qt::DirectConnection);
    connect(ui->actionUpdate, SIGNAL(triggered()), this , SLOT(updateSimulation()), Qt::DirectConnection);
    connect(ui->actionReset, SIGNAL(triggered()), this , SLOT(resetSimulation()), Qt::DirectConnection);
    connect(ui->actionCancel, SIGNAL(triggered()), this , SLOT(cancelSimulation()), Qt::DirectConnection);
}

void DMMainWindow::createModuleListView() 
{
	ui->treeWidget->clear();
	// load the module tree: first map element represents the group name as string
	// second the names of the modules as string
	std::map<std::string, std::vector<std::string> > mMap (this->simulation->getModuleRegistry()->getModuleMap());
	ui->treeWidget->setColumnCount(1);
	// iterate through groups
	for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) 
	{
		// create tree group
		QTreeWidgetItem * items = new QTreeWidgetItem(ui->treeWidget);
		items->setText(0, QString::fromStdString(it->first));
		// get modules, sort alphabetically
		std::vector<std::string> moduleNames = it->second;
		std::sort(moduleNames.begin(), moduleNames.end());
		// iterate through modules
		foreach(std::string name, moduleNames) 
		{
			QTreeWidgetItem * item;
			item = new QTreeWidgetItem(items);
			item->setText(0,QString::fromStdString(name));
			item->setText(1,"Module");
		}
	}

	QSettings settings;
	QStringList simPaths = settings.value("VIBeModules",QStringList()).toString().replace("\\","/").split(",");
	if(simPaths.size() == 0)
		return;

	QTreeWidgetItem * itSimDirs = new QTreeWidgetItem(ui->treeWidget);
	itSimDirs->setText(0, "Simulation directories");
	foreach(QString simDir, simPaths)
	{
		QTreeWidgetItem * itSimDir;
		itSimDir = new QTreeWidgetItem(itSimDirs);
		itSimDir->setText(0,simDir);
		itSimDir->setText(1,"Simulations");

		QStringList simPaths = QDir(simDir).entryList(QStringList("*.dyn"));
		foreach(QString simPath, simPaths)
		{
			QTreeWidgetItem * itSim;
			itSim = new QTreeWidgetItem(itSimDir);
			itSim->setText(0,simPath);
			itSim->setText(1,"Simulation");
		}
	}
}

void DMMainWindow::runSimulation() 
{
	ui->actionNew->setEnabled(false);
	ui->actionOpen->setEnabled(false);
	ui->actionReload_Modules->setEnabled(false);
	ui->actionReset->setEnabled(false);
	ui->actionRun->setEnabled(false);
	ui->actionReset->setEnabled(false);
	ui->actionUpdate->setEnabled(false);
	ui->actionSave->setEnabled(false);
	ui->actionSaveAs->setEnabled(false);
	
	simulationThread = new QThread;
	simulationThreadWrapper = new GuiSimulationObserver(simulation);
	connect(simulationThreadWrapper, SIGNAL(signalUpdateProgress(float)), this, SLOT(updateProgress(float)));
	connect(simulationThread, SIGNAL(started()), simulationThreadWrapper, SLOT(run()));
	connect(simulationThreadWrapper, SIGNAL(finished()), this, SLOT(simulationFinished()));
	connect(simulationThreadWrapper, SIGNAL(finished()), simulationThread, SLOT(deleteLater()));
	simulationThreadWrapper->moveToThread(simulationThread);

	simulationThread->start();
}
void DMMainWindow::updateSimulation()
{
	/*
    this->simulation->reloadModules();
    createModuleListView();
    this->simulation->updateSimulation();
	*/
}
void DMMainWindow::resetSimulation()
{
	simulation->reset();
}
void DMMainWindow::cancelSimulation()
{
	simulation->cancel();
}

void DMMainWindow::simulationFinished() 
{
	ui->actionNew->setEnabled(true);
	ui->actionOpen->setEnabled(true);
	ui->actionReload_Modules->setEnabled(true);
	ui->actionReset->setEnabled(true);
	ui->actionRun->setEnabled(true);
	ui->actionReset->setEnabled(true);
	ui->actionUpdate->setEnabled(true);
	ui->actionSave->setEnabled(true);
	ui->actionSaveAs->setEnabled(true);
}

void DMMainWindow::preferences() 
{	
    Preferences *p =  new Preferences(this);
    p->exec();
}

void DMMainWindow::saveAsSimulation() 
{
   QString fileName = QFileDialog::getSaveFileName(this, tr("Save DynaMind File"));
    if (!fileName.isEmpty()) 
	{
        if (!fileName.contains(".dyn"))
            fileName+=".dyn";
        this->simulation->writeSimulation(fileName.toStdString());
        this->getSimulation()->currentDocument = fileName;
    }
}

void DMMainWindow::saveSimulation() 
{
	if (!this->getSimulation()->currentDocument.isEmpty()) 
	{
        this->simulation->writeSimulation(this->getSimulation()->currentDocument.toStdString());
    } 
	else
        this->saveAsSimulation();
}

void DMMainWindow::clearSimulation() 
{	
    this->simulation->clearSimulation();
}

void DMMainWindow::loadSimulation(int id) 
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open DynaMind File"), 
													"", tr("DynaMind Files (*.dyn)"));

    if (!fileName.isEmpty()){
        this->clearSimulation();
        this->getSimulation()->currentDocument = fileName;
		simulation->loadSimulation(fileName.toStdString());
    }
}

DMMainWindow::~DMMainWindow() 
{
    //delete this->simulation;
	//foreach(SimulationTab *w, tabs)
	//	delete w;
	outputFile->close();
	delete simulationThreadWrapper;
}

void DMMainWindow::on_actionZoomIn_triggered()
{
	simulation->getSelectedTab()->getQGViewer()->scale(1.2, 1.2);
}

void DMMainWindow::on_actionAbout_triggered()
{
    GUIAboutDialog * ab= new GUIAboutDialog(this->simulation, this);
    ab->show();
}

void DMMainWindow::on_actionZoomOut_triggered()
{
	simulation->getSelectedTab()->getQGViewer()->scale(0.8,0.8);
}

void DMMainWindow::on_actionZoomReset_triggered()
{
	QGraphicsView* view = simulation->getSelectedTab()->getQGViewer();
    view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
}

/*void DMMainWindow::showHelp(std::string classname) 
{
    this->helpviewer->show();
    this->helpviewer->showHelpForModule(classname);
}*/

void DMMainWindow::on_actionShow_Help_triggered() 
{
    //this->helpviewer->show();
}
