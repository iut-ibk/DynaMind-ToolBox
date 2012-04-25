/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * DAnCE4Water (Dynamic Adaptation for eNabling City Evolution for Water) is a
 * strategic planning tool ti identify the transtion from an from a conventional
 * urban water system to an adaptable and sustainable system at a city scale
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
 * @section DESCRIPTION
 *
 * The class contains the main windows for the DAnCE4Water GUI
 */

#include "mainwindow.h"
//#include <boost/python.hpp>

#include "QDir"
#include "QThread"
#include "QTreeWidgetItem"
#include "QFileDialog"
#include "QSettings"
#include "modulereader.h"

#include "simulationio.h"
#include "guilink.h"
#include "StdRedirector.h"
#include "cstdio"
#include <QTreeWidget>
#include "simulationmanagment.h"
//#include "pythonenv.h"
#include <projectviewer.h>
#include <modelnode.h>
#include "datamanagment.h"
#include <dmmodule.h>
#include <dmgroup.h>
#include <boost/foreach.hpp>
#include <dmsimulation.h>
#include <dmdatamanagement.h>
#include <groupnode.h>
#include <QWidget>
#include <QThreadPool>
#include <guiimageresultview.h>
#include <dmlog.h>
#include <guilogsink.h>
#include <guidataobserver.h>
#include <plot.h>
#include <dmpythonenv.h>
#include <guisimulationobserver.h>
#include <guisimulation.h>
#include <rootgroupnode.h>
#include "preferences.h"
#include "projectviewer.h"
using namespace boost;
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
void MainWindow::updateResultImage(QString filename) {
}
void MainWindow::registerResultWindow(GUIResultObserver * ress) {
    GUIImageResultView * w = new GUIImageResultView();

    this->tabWidget_2->addTab(w, QString::fromStdString(this->simulation->getModuleWithUUID(ress->getUUID())->getName()));
    QString f = QString::fromStdString(ress->getfirstImage());
    w->updateImage(f);

    //Update Windows
    connect(ress, SIGNAL(updateImage(QString)), w, SLOT(updateImage(QString)), Qt::BlockingQueuedConnection);
}
void MainWindow::updatePlotData(double d) {

}

void MainWindow::registerPlotWindow(GUIResultObserver * ress, double x, double y) {
    /* GUIImageResultView * w = new GUIImageResultView();


    QString f = QString::fromStdString(ress->getfirstImage());
    w->updateImage(f);

    //Update Windows*/
    //Plot * p = new Plot();
    // QDockWidget *dw = new QDockWidget(this);
    //dw->setAllowedAreas(Qt::BottomDockWidgetArea);

    //dw->setWidget(p);
    //this->tabWidget->addTab(dw, QString::fromStdString(this->simulation->getModuleWithUUID(ress->getUUID())->getName()));
    //this->addDockWidget(Qt::BottomDockWidgetArea, dw);
    // dw->show();

    //this->tabWidget->addTab(p,QString::fromStdString(this->simulation->getModuleWithUUID(ress->getUUID())->getName()) );

    //DM::Logger(DM::Debug) << "Register Plot";
    //connect(ress, SIGNAL(newDoubleDataForPlot(double, double)), p, SLOT(appendData(double, double)), Qt::BlockingQueuedConnection);
    //p->appendData( x,y);
}



void MainWindow::ReloadSimulation() {
    this->simulation->reloadModules();
    createModuleListView();
}

void MainWindow::startEditor() {
    DM::PythonEnv::getInstance()->startEditra();
}
void MainWindow::removeGroupWindows(GroupNode * g) {

    //Check if already exists
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        if ((pv->getRootNode()->getVIBeModel()->getUuid()).compare(g->getVIBeModel()->getUuid()) == 0) {
            this->tabWidget_4->removeTab(i);
            delete pv;
        }
    }
}
void MainWindow::addNewGroupWindows(GroupNode * g) {

    //Check if already exists
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        if ((pv->getRootNode()->getVIBeModel()->getUuid()).compare(g->getVIBeModel()->getUuid()) == 0) {
            return;
        }
    }

    ProjectViewer * newgroup = new ProjectViewer(g );

    this->simulation->addSimulationObserver(new GUISimulationObserver(newgroup));

    connect(simulation, SIGNAL(addedGroup(GroupNode*)), newgroup, SLOT(addGroup(GroupNode*)));
    connect(simulation, SIGNAL(GroupNameChanged(GroupNode*)), this, SLOT(renameGroupWindow(GroupNode*)));
    connect(simulation, SIGNAL(addedModule(ModelNode*)), newgroup, SLOT(addModule(ModelNode*)));
    connect(newgroup, SIGNAL(NewModule(QString,QPointF, DM::Module *)), simulation, SLOT(GUIaddModule(QString,QPointF, DM::Module  *)));

    newgroup->setResultViewer(this);



    QGraphicsView * gv = new QGraphicsView(newgroup);
    gv->setRenderHints(QPainter::Antialiasing);
    gv->setAcceptDrops(true);

    QString name = QString::fromStdString(g->getVIBeModel()->getName());
    if (name.isEmpty()) {
        name = g->getName();
    }
    this->groupscenes[this->tabWidget_4->addTab(gv,name)] = newgroup;

}

void MainWindow::renameGroupWindow(GroupNode * g) {
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        if ((pv->getRootNode()->getVIBeModel()->getUuid()).compare(g->getVIBeModel()->getUuid()) == 0) {
            this->tabWidget_4->setTabText(i, QString::fromStdString(g->getVIBeModel()->getName()));
        }
    }
}

MainWindow::MainWindow(QWidget * parent)
{
    log_updater = new GuiLogSink();
    //#ifdef _DEBUG
    DM::Log::init(log_updater,DM::Debug);
    //#else
    //    DM::Log::init(log_updater,DM::Standard);
    //#endif
    running =  false;
    setupUi(this);
    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    env->addOverWriteStdCout();






    //this->graphicsView->setViewport(new QGLWidget());

    this->simulation = new GUISimulation();

    connect(this->simulation, SIGNAL(addedGroup(GroupNode*)), this, SLOT(addNewGroupWindows(GroupNode*)));
    this->simulation->registerRootNode();
    this->simulation->addModulesFromSettings();



    //addNewGroup((DM::Group*)this->simulation->getRootGroup());


    this->mnodes = new QVector<ModelNode*>();
    this->gnodes = new QVector<GroupNode*>();
    //this->groups = Groups(this);
    log_widget->connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(appendPlainText(QString)), Qt::QueuedConnection);
    connect( actionRun, SIGNAL( activated() ), this, SLOT( runSimulation() ), Qt::DirectConnection );
    connect( actionPreferences, SIGNAL ( activated() ), this, SLOT(preferences() ), Qt::DirectConnection );
    //connect(buildsim, SIGNAL(clicked()), this , SLOT(buildSimulation()), Qt::DirectConnection);
    connect(runSim, SIGNAL(clicked()), this , SLOT(runSimulation()), Qt::DirectConnection);
    connect(actionSave, SIGNAL(activated()), this , SLOT(saveSimulation()), Qt::DirectConnection);
    connect(actionSaveAs, SIGNAL(activated()), this , SLOT(saveAsSimulation()), Qt::DirectConnection);
    connect(actionOpen, SIGNAL(activated()), this , SLOT(loadSimulation()), Qt::DirectConnection);
    connect(actionNew, SIGNAL(activated()), this , SLOT(clearSimulation()), Qt::DirectConnection);
    connect(actionImport, SIGNAL(activated()), this , SLOT(importSimulation()), Qt::DirectConnection);
    connect(actionEditor, SIGNAL(activated()), this , SLOT(startEditor()), Qt::DirectConnection);
    connect(actionReload_Modules, SIGNAL(activated()), this , SLOT(ReloadSimulation()), Qt::DirectConnection);
    connect(edit_groups, SIGNAL(clicked()), this , SLOT(editGroup()), Qt::DirectConnection);

    //runSim->setDisabled(true);
    //treeWidget->setColumnCount(1);
    currentDocument = "";



    QSettings settings("IUT", "DYNAMIND");
    //settings.beginGroup("Preferences");
    if(settings.value("pythonModules").toString().isEmpty()) {
        counter++;
        this->preferences();
    }


    //this->simulation->addSimulationObserver(this->simobserver );

    this->simmanagment = new SimulationManagment();


    connect(this->simmanagment, SIGNAL(valueChanged(int)), this->progressBar, SLOT(setValue(int)));
    //connect(this->simmanagment, SIGNAL(valueChanged(int)), this, SLOT(test(int)));
    this->progressBar->setRange(0,1);
    this->progressBar->setValue(0);

    createModuleListView();

    this->rootItemModelTree = new QTreeWidgetItem();
    this->rootItemModelTree->setText(0, "Groups");
    this->rootItemModelTree->setText(1, "");
    this->rootItemModelTree->setExpanded(true);
    this->modelTree->setColumnCount(2);
    this->modelTree->addTopLevelItem(this->rootItemModelTree);
    this->modelTree->headerItem()->setText(0, "Module");
    this->modelTree->headerItem()->setText(1, "");
    //this->scene->setModules(& this->modules);
    //this->scene->setModelNodes(this->mnodes);
    //this->scene->setGroupNodes(this->gnodes);
    //this->scene->setModuleRegistry(& this->registry);
    //this->graphicsView->setScene(root);
    //this->graphicsView->setAcceptDrops(true);

    // this->points = new QwtPlotMarker();
    // this->points->attach(this->PlotWindow);

    //QThreadPool::setMaxThreadCount(1);

    //StdRedirector<> *  myRedirector = new StdRedirector<>( std::cout, outcallback, this->plainTextEdit );




    //Init ModuleRegistry


    // create3DViewer();



}

void MainWindow::createModuleListView() {
    this->treeWidget->clear();
    std::list<std::string> mlist = (this->simulation->getModuleRegistry()->getRegisteredModules());
    std::map<std::string, std::vector<std::string> > mMap (this->simulation->getModuleRegistry()->getModuleMap());
    this->treeWidget->setColumnCount(1);
    for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) {
        QTreeWidgetItem * items = new QTreeWidgetItem(this->treeWidget);
        std::string name = it->first;
        items->setText(0, QString::fromStdString(name));
        std::vector<std::string> names = it->second;
        std::sort(names.begin(), names.end());
        BOOST_FOREACH(std::string name, names) {
            QTreeWidgetItem * item;
            item = new QTreeWidgetItem(items);
            item->setText(0,QString::fromStdString(name));
            item->setText(1,"Module");
        }
    }



    //Add VIBe Modules
    QStringList filters;
    filters << "*.dyn";
    QSettings settings("IUT", "DYNAMIND");
    QStringList moduleshome = settings.value("VIBeModules",QStringList()).toString().replace("\\","/").split(",");
    for (int index = 0; index < moduleshome.size(); index++) {
        QDir d = QDir(moduleshome[index]);
        QStringList list = d.entryList(filters);
        QString module_name = d.absolutePath().split("/").last();
        mMap.clear();
        foreach(QString name, list) {
            std::vector<std::string> vec = mMap[module_name.toStdString()];
            vec.push_back(name.toStdString());
            mMap[module_name.toStdString()]  = vec;
        }

        for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) {
            QTreeWidgetItem * items = new QTreeWidgetItem(this->treeWidget);
            std::string name = it->first;
            items->setText(0, QString::fromStdString(name));
            std::vector<std::string> names = it->second;
            std::sort(names.begin(), names.end());

            BOOST_FOREACH(std::string name, names) {
                QTreeWidgetItem * item;
                item = new QTreeWidgetItem(items);
                item->setText(0,QString::fromStdString(name));
                item->setText(1,"VIBe");
                stringstream filename;
                filename << moduleshome[0].toStdString() << "/" << name;
                item->setText(2,QString::fromStdString(filename.str()));
            }
        }
    }


}
void MainWindow::editGroup() {

    QTreeWidgetItem * item = this->modelTree->currentItem () ;
    if (item == 0) {
        return;
    }


}
void MainWindow::updateRasterData(QString UUID,  QString Name) {
    //this->widget->setRasterData(UUID, Name);

}

void MainWindow::runSimulation() {
    for (int  i = 0; i < this->mnodes->size(); i++) {
        ModelNode * m = this->mnodes->at(i);
        m->resetModel();
    }

    //connect(simobserver, SIGNAL(finished()), this , SLOT(SimulationFinished()), Qt::DirectConnection);

    for (int i = 0; i < 20; i++)
        simulation->startSimulation(true);

    simulation->start();

    return;

}
void MainWindow::SimulationFinished() {

}

void MainWindow::preferences() {
    Preferences *p =  new Preferences(this);
    p->exec();
}

void MainWindow::setRunning() {
    this->running = false;
}

void MainWindow::sceneChanged() {


}
void MainWindow::saveAsSimulation() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save DynaMind File"), "", tr("DynaMind Files (*.dyn)"));
    if (!fileName.isEmpty()) {
        if (!fileName.contains(".dyn"))
            fileName+=".dyn";
        this->simulation->writeSimulation(fileName.toStdString());
        this->writeGUIInformation(fileName);
        this->currentDocument = fileName;
    }

}
void MainWindow::saveSimulation() {
    if (!this->currentDocument.isEmpty()) {
        this->simulation->writeSimulation(this->currentDocument.toStdString());
        this->writeGUIInformation(currentDocument);
    } else {
        this->saveAsSimulation();
    }

}
void MainWindow::writeGUIInformation(QString FileName) {

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



        BOOST_FOREACH(ModelNode * m, viewer->getRootNode()->getChildNodes()) {
            out  << "\t" << "\t"<<"<GUI_Node>" << "\n";
            out << "\t" << "\t"<< "\t" << "<GUI_UUID value=\""
                << QString::fromStdString(m->getVIBeModel()->getUuid()) << "\"/>" << "\n";

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

}

void MainWindow::clearSimulation() {
    while (this->gnodes->size() > 0) {
        delete this->gnodes->at(0);
        this->gnodes->remove(0);
    }
    while (this->mnodes->size() > 0)
        delete this->mnodes->at(0);
    this->currentDocument = "";



}
void MainWindow::importSimulation(QString fileName, QPointF offset) {
    if (fileName.compare("") == 0)
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open DynaMind File"), "", tr("DynaMind Files (*.dyn)"));

    if (fileName.isEmpty())
        return;


    std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
    SimulationIO simio;
    simio.loadSimluation(fileName, this->simulation, UUID_Translation, this->mnodes);
    this->loadGUIModules((DM::Group *)this->simulation->getRootGroup(), UUID_Translation, simio.getPositionOfLoadedModules());

    this->loadGUILinks(UUID_Translation);


}

void MainWindow::loadGUIModules(DM::Group * g, std::map<std::string, std::string> UUID_Translation, QVector<LoadModule> posmodules) {

    std::map<std::string, std::string> reveredUUID_Translation;
    for (std::map<std::string, std::string>::const_iterator it = UUID_Translation.begin();
         it != UUID_Translation.end();
         ++it) {
        reveredUUID_Translation[it->second] = it->first;
    }
    ProjectViewer * currentView = 0;
    foreach(int i, this->groupscenes.keys()) {
        ProjectViewer * view = this->groupscenes[i];
        if (g->getUuid().compare( view->getRootNode()->getVIBeModel()->getUuid()) == 0  )
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

}

void MainWindow::loadSimulation(int id) {

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open DynaMind File"), "", tr("DynaMind Files (*.dyn)"));

    if (!fileName.isEmpty()){
        this->clearSimulation();
        this->currentDocument = fileName;
        std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
        SimulationIO simio;
        simio.loadSimluation(fileName, this->simulation, UUID_Translation, this->mnodes);
        UUID_Translation[this->simulation->getRootGroup()->getUuid()] = this->simulation->getRootGroup()->getUuid();
        this->loadGUIModules((DM::Group*)this->simulation->getRootGroup(),  UUID_Translation, simio.getPositionOfLoadedModules());
        this->loadGUILinks(UUID_Translation);
    }
}
void MainWindow::loadGUILinks(std::map<std::string, std::string> UUID_Translation) {

    std::map<std::string, std::string> reveredUUID_Translation;
    for (std::map<std::string, std::string>::const_iterator it = UUID_Translation.begin();
         it != UUID_Translation.end();
         ++it) {
        reveredUUID_Translation[it->second] = it->first;
    }

    BOOST_FOREACH(DM::ModuleLink * l,this->simulation->getLinks()) {

        ModelNode * outmodule = 0;
        ModelNode * inmodule = 0;

        ProjectViewer * currentView = 0;

        foreach(int i, this->groupscenes.keys()) {
            ProjectViewer * view = this->groupscenes[i];
            if (l->getInPort()->getModule()->getGroup()->getUuid().compare( view->getRootNode()->getVIBeModel()->getUuid()) == 0  )
                currentView = view;
        }
        ProjectViewer * currentView_out = 0;
        foreach(int i, this->groupscenes.keys()) {
            ProjectViewer * view = this->groupscenes[i];
            if (l->getOutPort()->getModule()->getGroup()->getUuid().compare( view->getRootNode()->getVIBeModel()->getUuid()) == 0  )
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

            if (currentView->getRootNode()->getVIBeModel() == l->getOutPort()->getModule()) {
                outmodule = currentView->getRootNode() ;

            }

            foreach (ModelNode * mn, currentView->getRootNode()->getChildNodes()) {
                if (mn->getVIBeModel() == l->getOutPort()->getModule()) {
                    outmodule = mn;
                    break;
                }
            }

            if (currentView->getRootNode()->getVIBeModel() == l->getInPort()->getModule()) {
                inmodule = currentView->getRootNode();

            }

            foreach(ModelNode * mn, currentView->getRootNode()->getChildNodes()) {
                if (mn->getVIBeModel() == l->getInPort()->getModule()) {
                    inmodule = mn;
                    break;
                }
            }

            if (inmodule != 0 && outmodule != 0) {
                break;
            }
        }

        //Look if exists in Translation list
        if(reveredUUID_Translation.find(inmodule->getVIBeModel()->getUuid()) == reveredUUID_Translation.end())
            continue;
        if(reveredUUID_Translation.find(inmodule->getVIBeModel()->getUuid()) == reveredUUID_Translation.end())
            continue;


        GUILink * gui_link  = new GUILink();

        gui_link->setOutPort(outmodule->getGUIPort(l->getOutPort()));
        gui_link->setInPort(inmodule->getGUIPort(l->getInPort()));
        gui_link->setVIBeLink(l);
        gui_link->setSimulation(this->simulation);

        currentView->addItem(gui_link);
        currentView->update();

    }

}

MainWindow::~MainWindow() {
    for ( int i = 0; i < mnodes->size(); i++ ) {
        ModelNode * m = mnodes->at(i);
        delete m;
    }
    //delete this->scene;
    delete this->mnodes;
    delete this->simulation;
}

void MainWindow::on_actionZoomIn_activated(){
    int i= this->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];

    view->scale(1.2, 1.2);

}

void MainWindow::on_actionZoomOut_activated()
{
    int i= this->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];
    view->scale(0.8, 0.8);
}

void MainWindow::on_actionZoomReset_activated()
{
    int i= this->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];
    view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);



}
