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
#include "simulationio.h"
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
#include <guisimulationobserver.h>
#include <guisimulation.h>
#include <rootgroupnode.h>
#include "preferences.h"
#include "projectviewer.h"
#include "guihelpviewer.h"
#include "guiaboutdialog.h"

#include <dmmoduleregistry.h>
#include <qgraphicsview.h>

void outcallback( const char* ptr, std::streamsize count, void* pTextBox )
{
    (void) count;
    QPlainTextEdit * p = static_cast< QPlainTextEdit* >( pTextBox );

    if (*(ptr) > 37 ) {
        int c = *(ptr);
        QString s = QString::fromAscii(ptr);
        p->appendPlainText( ptr );
    }
}

void DMMainWindow::ReloadSimulation() {
	/*
    this->simulation->reloadModules();
    createModuleListView();
	*/
}

void DMMainWindow::startEditor() {
    //DM::PythonEnv::getInstance()->startEditra();
}
void DMMainWindow::removeGroupWindows(QString uuid) {
	/*
    DM::Logger(DM::Debug) << "Remove ProjectWindow " << uuid.toStdString();
    //Check if already exists
    QMap<int, ProjectViewer * > groupscenes_tmp;
    int inew = 0;
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        if (pv != 0) {

            if ((pv->getUUID()).compare(uuid) == 0) {

                ui->tabWidget_4->removeTab(ui->tabWidget_4->indexOf(tabmap[pv]));

                delete pv;

            } else {
                groupscenes_tmp[inew++] = pv;
            }
        }

    }
    this->groupscenes = groupscenes_tmp;*/
}
void DMMainWindow::addNewGroupWindows(GroupNode * g) {
	/*
    //Check if already exists
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        DM::Module * rg = pv->getRootNode()->getDMModel();
        if (!rg)
            return;
        if ((pv->getRootNode()->getDMModel()->getUuid()).compare(g->getDMModel()->getUuid()) == 0) {
            return;
        }
    }

    ProjectViewer * newgroup = new ProjectViewer(g );


    if (g->getDMModel()->getUuid().compare(simulation->getRootGroup()->getUuid()) == 0)
        this->simulation->addSimulationObserver(new GUISimulationObserver(newgroup));

    connect(simulation, SIGNAL(addedGroup(GroupNode*)), newgroup, SLOT(addGroup(GroupNode*)));
    connect(simulation, SIGNAL(GroupNameChanged(GroupNode*)), this, SLOT(renameGroupWindow(GroupNode*)));
    connect(simulation, SIGNAL(addedModule(ModelNode*)), newgroup, SLOT(addModule(ModelNode*)));
    connect(newgroup, SIGNAL(NewModule(QString,QPointF, DM::Module *)), simulation, SLOT(GUIaddModule(QString,QPointF, DM::Module  *)));
    connect(simulation, SIGNAL(showHelpForModule(std::string, std::string)), this, SLOT(showHelp(std::string, std::string)));
    newgroup->setResultViewer(this);



    QGraphicsView * gv = new QGraphicsView(newgroup);
    gv->setRenderHints(QPainter::Antialiasing);
    gv->setAcceptDrops(true);

    QString name = QString::fromStdString(g->getDMModel()->getName());
    if (name.isEmpty()) {
        name = g->getName();
    }
    this->groupscenes[ui->tabWidget_4->addTab(gv,name)] = newgroup;
    tabmap[newgroup] = gv;
	*/
}

void DMMainWindow::renameGroupWindow(GroupNode * g) {
	/*
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        if ((pv->getRootNode()->getDMModel()->getUuid()).compare(g->getDMModel()->getUuid()) == 0) {
            ui->tabWidget_4->setTabText(i, QString::fromStdString(g->getDMModel()->getName()));
        }
    }
	*/
}

DMMainWindow::DMMainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::DMMainWindow)
{
    Q_INIT_RESOURCE(icons);
    ui->setupUi(this);
    log_updater = new GuiLogSink();
#if defined DEBUG || _DEBUG
    DM::Log::init(log_updater,DM::Debug);
#else
    DM::Log::init(log_updater,DM::Standard);
#endif
    running =  false;
    this->setParent(parent);
    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    env->addPythonPath(QApplication::applicationDirPath().toStdString());
    env->addOverWriteStdCout();

    this->simulation = new GUISimulation();
	
    //connect(this->simulation, SIGNAL(addedGroup(GroupNode*)), this, SLOT(addNewGroupWindows(GroupNode*)));
    //this->simulation->registerRootNode();
    this->simulation->loadModulesFromDefaultLocation();
    this->simulation->loadModulesFromSettings();
    this->helpviewer = new GUIHelpViewer(this->simulation);

	// init root tab
	SimulationTab* tab = new SimulationTab(parent, this->simulation);
	tabs.append(tab);
	ui->tabWidget_4->addTab(tab->getQGViewer(),"Root Tab");
	

    ui->log_widget->connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(appendPlainText(QString)), Qt::QueuedConnection);
    connect( ui->actionRun, SIGNAL( triggered() ), this, SLOT( runSimulation() ), Qt::DirectConnection );
    connect( ui->actionPreferences, SIGNAL ( triggered() ), this, SLOT(preferences() ), Qt::DirectConnection );
    connect(ui->actionSave, SIGNAL(triggered()), this , SLOT(saveSimulation()), Qt::DirectConnection);
    connect(ui->actionSaveAs, SIGNAL(triggered()), this , SLOT(saveAsSimulation()), Qt::DirectConnection);
    connect(ui->actionOpen, SIGNAL(triggered()), this , SLOT(loadSimulation()), Qt::DirectConnection);
    connect(ui->actionNew, SIGNAL(triggered()), this , SLOT(clearSimulation()), Qt::DirectConnection);
    connect(ui->actionImport, SIGNAL(triggered()), this , SLOT(importSimulation()), Qt::DirectConnection);
    connect(ui->actionEditor, SIGNAL(triggered()), this , SLOT(startEditor()), Qt::DirectConnection);
    connect(ui->actionReload_Modules, SIGNAL(triggered()), this , SLOT(ReloadSimulation()), Qt::DirectConnection);
    connect(ui->actionUpdate, SIGNAL(triggered()), this , SLOT(updateSimulation()), Qt::DirectConnection);
    //connect(ui->actionReset, SIGNAL(triggered()), this->simulation , SLOT(reset()), Qt::DirectConnection);
    currentDocument = "";

    //this->simmanagment = new SimulationManagment();

    createModuleListView();
	/*
    this->rootItemModelTree = new QTreeWidgetItem();
    this->rootItemModelTree->setText(0, "Groups");
    this->rootItemModelTree->setText(1, "");
    this->rootItemModelTree->setExpanded(true);*/
	

}

void DMMainWindow::createModuleListView() 
{
    ui->treeWidget->clear();
    //std::list<std::string> mlist = (this->simulation->getModuleRegistry()->getRegisteredModules());
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
	/*
    //Add VIBe Modules
    QStringList filters;
    filters << "*.dyn";
    QSettings settings;
    QStringList moduleDirs = settings.value("VIBeModules",QStringList()).toString().replace("\\","/").split(",");

	foreach(QString strDir, moduleDirs)
	{
    //for (int index = 0; index < moduleDirs.size(); index++) 
	//{
        QDir dir = QDir(strDir);
        QStringList list = dir.entryList(filters);
        QString module_name = dir.absolutePath().split("/").last();
        mMap.clear();
        foreach(QString name, list) {
            std::vector<std::string> vec = mMap[module_name.toStdString()];
            vec.push_back(name.toStdString());
            mMap[module_name.toStdString()]  = vec;
        }

        for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) {
            QTreeWidgetItem * items = new QTreeWidgetItem(ui->treeWidget);
            std::string name = it->first;
            items->setText(0, QString::fromStdString(name));
            std::vector<std::string> names = it->second;
            std::sort(names.begin(), names.end());

            foreach(std::string name, names) {
                QTreeWidgetItem * item;
                item = new QTreeWidgetItem(items);
                item->setText(0,QString::fromStdString(name));
                item->setText(1,"VIBe");
                stringstream filename;
                filename << moduleDirs[0].toStdString() << "/" << name;
                item->setText(2,QString::fromStdString(filename.str()));
            }
        }
    }
	*/
}

void DMMainWindow::runSimulation() {
    //simulation->start();
}
void DMMainWindow::updateSimulation() {
	/*
    this->simulation->reloadModules();
    createModuleListView();
    this->simulation->updateSimulation();
	*/
}

void DMMainWindow::SimulationFinished() {

}

void DMMainWindow::preferences() 
{	
    Preferences *p =  new Preferences(this);
    p->exec();
}

void DMMainWindow::setRunning() {
    //this->running = false;
}

void DMMainWindow::sceneChanged() {


}
void DMMainWindow::saveAsSimulation() {
	/*
   QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save DynaMind File"));
    if (!fileName.isEmpty()) {
        if (!fileName.contains(".dyn"))
            fileName+=".dyn";
        this->simulation->writeSimulation(fileName.toStdString());
        this->writeGUIInformation(fileName);
        this->currentDocument = fileName;
    }
	*/
}
void DMMainWindow::saveSimulation() {
	/*
    if (!this->currentDocument.isEmpty()) {
        this->simulation->writeSimulation(this->currentDocument.toStdString());
        this->writeGUIInformation(currentDocument);
    } else {
        this->saveAsSimulation();
    }
	*/
}
void DMMainWindow::writeGUIInformation(QString FileName) {
	/*
    //Find upper left corner;
    float minx;
    float miny;

    //Write GUI Informations for every Groupscene

    QFile file(FileName);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out << "<DynaMindGUI>" << "\n";
    out << "\t"<<"<GUI_Nodes>" << "\n";

    foreach (int scene, this->groupscenes.keys() ) {
        ProjectViewer * viewer = this->groupscenes[scene];


        for (int i = 0; i < viewer->getRootNode()->getChildNodes().size(); i++) {
            ModelNode * m = viewer->getRootNode()->getChildNodes()[i];
            if (i == 0)        {
                minx = m->pos().x();
                miny = m->pos().y();
                continue;
            }
            if (minx > m->pos().x()) {
                minx =  m->pos().x();
            }
            if (miny > m->pos().y()) {
                miny =  m->pos().y();
            }
        }



        foreach(ModelNode * m, viewer->getRootNode()->getChildNodes()) {
            out  << "\t" << "\t"<<"<GUI_Node>" << "\n";
            out << "\t" << "\t"<< "\t" << "<GUI_UUID value=\""
                << QString::fromStdString(m->getDMModel()->getUuid()) << "\"/>" << "\n";

            out << "\t" << "\t"<< "\t" << "<GUI_PosX value=\""
                << m->scenePos().x() - minx << "\"/>" << "\n";



            out << "\t" << "\t"<< "\t" << "<GUI_PosY value=\""
                << m->scenePos().y() - miny << "\"/>" << "\n";

            out << "\t" << "\t"<< "\t" << "<GUI_Minimized value=\""
                << m->isMinimized() << "\"/>" << "\n";

            out  << "\t" << "\t"<<"</GUI_Node>" << "\n";

        }
    }
    out << "\t"<<"</GUI_Nodes>" << "\n";

    out << "</DynaMindGUI>" << "\n";
    out << "</DynaMind>"<< "\n";

    file.close();
	*/
}

void DMMainWindow::clearSimulation() {
	/*
    this->simulation->clearSimulation();
    this->currentDocument = "";
	*/
}

void DMMainWindow::importSimulation(QString fileName, QPointF offset) {
	/*
    if (fileName.compare("") == 0)
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open DynaMind File"), "", tr("DynaMind Files (*.dyn)"));

    if (fileName.isEmpty())
        return;


    std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
    SimulationIO simio;
    simio.loadSimluation(fileName, this->simulation, UUID_Translation);
    this->loadGUIModules((DM::Group *)this->simulation->getRootGroup(), UUID_Translation, simio.getPositionOfLoadedModules());

    this->loadGUILinks(UUID_Translation);
	*/

}

void DMMainWindow::loadGUIModules(DM::Group * g, std::map<std::string, std::string> UUID_Translation, QVector<LoadModule> posmodules) {
	/*
    std::map<std::string, std::string> reveredUUID_Translation;
    for (std::map<std::string, std::string>::const_iterator it = UUID_Translation.begin();
         it != UUID_Translation.end();
         ++it) {
        reveredUUID_Translation[it->second] = it->first;
    }
    ProjectViewer * currentView = 0;
    foreach(int i, this->groupscenes.keys()) {
        ProjectViewer * view = this->groupscenes[i];
        if (g->getUuid().compare( view->getRootNode()->getDMModel()->getUuid()) == 0  )
            currentView = view;
    }
    if (currentView == 0)
        return;

    foreach (DM::Module * m , g->getModules()) {

        //Look if exists in Translation list
        if(reveredUUID_Translation.find(m->getUuid()) == reveredUUID_Translation.end())
            continue;


        //GetPos
        QPointF p;
        foreach (LoadModule lm, posmodules) {
            if (lm.tmpUUID.compare(reveredUUID_Translation[m->getUuid()]) == 0) {
                p.setX(lm.PosX);
                p.setY(lm.PosY);
            }
        }


        this->simulation->GUIaddModule(m, p);

        if (m->isGroup())
            this->loadGUIModules((DM::Group * )m,  UUID_Translation,posmodules);

    }
	*/
}

void DMMainWindow::loadSimulation(int id) {
	/*
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open DynaMind File"), "", tr("DynaMind Files (*.dyn)"));

    if (!fileName.isEmpty()){
        this->clearSimulation();
        this->currentDocument = fileName;
        std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
        SimulationIO simio;
        simio.loadSimluation(fileName, this->simulation, UUID_Translation);
        if (this->simulation->getSimulationStatus() == DM::SIM_FAILED_LOAD)  {
            this->simulation->clearSimulation();
            return;
        }
        UUID_Translation[this->simulation->getRootGroup()->getUuid()] = this->simulation->getRootGroup()->getUuid();
        this->loadGUIModules((DM::Group*)this->simulation->getRootGroup(),  UUID_Translation, simio.getPositionOfLoadedModules());
        this->loadGUILinks(UUID_Translation);
    }
	*/
}
void DMMainWindow::loadGUILinks(std::map<std::string, std::string> UUID_Translation) {
	/*
    std::map<std::string, std::string> reveredUUID_Translation;
    for (std::map<std::string, std::string>::const_iterator it = UUID_Translation.begin();
         it != UUID_Translation.end();
         ++it) {
        reveredUUID_Translation[it->second] = it->first;
    }

    foreach(DM::ModuleLink * l,this->simulation->getLinks()) {

        ModelNode * outmodule = 0;
        ModelNode * inmodule = 0;

        ProjectViewer * currentView = 0;

        foreach(int i, this->groupscenes.keys()) {
            ProjectViewer * view = this->groupscenes[i];
            if (l->getInPort()->getModule()->getGroup()->getUuid().compare( view->getRootNode()->getDMModel()->getUuid()) == 0  )
                currentView = view;
        }
        ProjectViewer * currentView_out = 0;
        foreach(int i, this->groupscenes.keys()) {
            ProjectViewer * view = this->groupscenes[i];
            if (l->getOutPort()->getModule()->getGroup()->getUuid().compare( view->getRootNode()->getDMModel()->getUuid()) == 0  )
                currentView_out = view;
        }



        if (currentView == 0)
            continue;

        //TODO: Remove dirty hack
        //The Problem is with links that are conncet to the root group.
        //If so the outport module can not be find with current view.
        //In this Loop the view is switch to the view of the
        //Inport and then it works.
        for (int i = 0; i < 2; i++) {
            if (i == 1)
                currentView = currentView_out;

            if (currentView->getRootNode()->getDMModel() == l->getOutPort()->getModule()) {
                outmodule = currentView->getRootNode() ;

            }

            foreach (ModelNode * mn, currentView->getRootNode()->getChildNodes()) {
                if (mn->getDMModel() == l->getOutPort()->getModule()) {
                    outmodule = mn;
                    break;
                }
            }

            if (currentView->getRootNode()->getDMModel() == l->getInPort()->getModule()) {
                inmodule = currentView->getRootNode();

            }

            foreach(ModelNode * mn, currentView->getRootNode()->getChildNodes()) {
                if (mn->getDMModel() == l->getInPort()->getModule()) {
                    inmodule = mn;
                    break;
                }
            }

            if (inmodule != 0 && outmodule != 0) {
                break;
            }
        }

        //Look if exists in Translation list
        if(reveredUUID_Translation.find(inmodule->getDMModel()->getUuid()) == reveredUUID_Translation.end())
            continue;
        if(reveredUUID_Translation.find(inmodule->getDMModel()->getUuid()) == reveredUUID_Translation.end())
            continue;


        GUILink * gui_link  = new GUILink();

        gui_link->setOutPort(outmodule->getGUIPort(l->getOutPort()));
        gui_link->setInPort(inmodule->getGUIPort(l->getInPort()));
        gui_link->setVIBeLink(l);
        gui_link->setSimulation(this->simulation);

        currentView->addItem(gui_link);
        currentView->update();

    }
	*/
}



DMMainWindow::~DMMainWindow() {
    //delete this->simulation;
	foreach(SimulationTab *w, tabs)
		delete w;
}

void DMMainWindow::on_actionZoomIn_triggered(){
    int i= ui->tabWidget_4->currentIndex();
    /*QGraphicsView * view = groupscenes[i]->views()[0];

    view->scale(1.2, 1.2);*/
}

void DMMainWindow::on_actionAbout_triggered()
{
    GUIAboutDialog * ab= new GUIAboutDialog(this->simulation, this);
    ab->show();

}

void DMMainWindow::on_actionZoomOut_triggered()
{
    /*int i= ui->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];
    view->scale(0.8, 0.8);*/
}

void DMMainWindow::on_actionZoomReset_triggered()
{
    /*int i= ui->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];
    view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);
	*/


}

void DMMainWindow::showHelp(std::string classname, std::string uuid) {

    this->helpviewer->show();
    this->helpviewer->showHelpForModule(classname, uuid);
}

void DMMainWindow::on_actionShow_Help_triggered() {
    this->helpviewer->show();
}
