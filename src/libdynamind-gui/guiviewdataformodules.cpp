#include "guiviewdataformodules.h"
#include "ui_guiviewdataformodules.h"
#include <dmmodule.h>
#include <dmport.h>
#include <dm.h>
typedef std::map<std::string, std::vector<DM::View> > view_map;

GUIViewDataForModules::GUIViewDataForModules(DM::Module * m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIViewDataForModules)
{
    ui->setupUi(this);
    this->m = m;


    DM::Logger(DM::Debug) << this->m->getName();

    //Views Stored in System
    std::map<std::string, std::vector<DM::View> > views = m->getViews();
    this->ui->treeWidget_views->setColumnCount(3);
    this->ui->treeWidget_views->setColumnWidth(0,200);
    QTreeWidgetItem * headerItem = this->ui->treeWidget_views->headerItem();
    headerItem->setText(0, "Data Stream");
    headerItem->setText(1, "Type");
    headerItem->setText(2, "Access");

    for (view_map::const_iterator it = views.begin(); it != views.end(); ++it) {
        QTreeWidgetItem * root_port = new QTreeWidgetItem();
         this->ui->treeWidget_views->addTopLevelItem(root_port);
        root_port->setText(0, QString::fromStdString(it->first));

        std::vector<DM::View> vs = it->second;
        foreach (DM::View v, vs) {
            if (v.getName().compare("dummy") == 0)
                continue;
            QTreeWidgetItem * item_view = new QTreeWidgetItem();
            item_view->setText(0, QString::fromStdString(v.getName()));
            int type = v.getType();

            if (type == DM::NODE)
                item_view->setText(1, "Nodes");
            if (type == DM::EDGE)
                item_view->setText(1, "Edges");
            if (type == DM::FACE)
                item_view->setText(1, "Faces");
            if (type == DM::SYSTEM)
                item_view->setText(1, "Systems");
            if (type == DM::RASTERDATA)
                item_view->setText(1, "Raster Data");

            if (v.getAccessType() == DM::WRITE)
                item_view->setText(2, "write");
            if (v.getAccessType() == DM::READ)
                item_view->setText(2, "read");
            if (v.getAccessType() == DM::MODIFY)
                item_view->setText(2, "modify");

            root_port->addChild(item_view);

            foreach (std::string s, v.getReadAttributes()) {
                QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
                item_attribute->setText(0, QString::fromStdString(s));
                item_attribute->setText(2, "read");
                item_view->addChild(item_attribute);
            }
            foreach (std::string s, v.getWriteAttributes()) {
                QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
                item_attribute->setText(0, QString::fromStdString(s));
                item_attribute->setText(2, "write");
                item_view->addChild(item_attribute);
            }
            this->ui->treeWidget_views->expandItem(root_port);
        }
    }

    this->ui->treeWidget->setColumnCount(2);
    this->ui->treeWidget->setColumnWidth(0,200);
    headerItem = this->ui->treeWidget->headerItem();
    headerItem->setText(0, "Data Stream");
    headerItem->setText(1, "Type");

    foreach (DM::Port * p, this->m->getOutPorts())
    {
        std::string dataname = p->getLinkedDataName();
        DM::Logger(DM::Debug) << dataname;
        DM::System * sys = this->m->getData(dataname);
        if (sys == 0) {
            continue;
        }
        QTreeWidgetItem * root_port = new QTreeWidgetItem();

        root_port->setText(0, QString::fromStdString(dataname));
        this->ui->treeWidget->addTopLevelItem(root_port);



        foreach (std::string name, sys->getNamesOfViews()) {
            if (name.compare("dummy") == 0)
                continue;
            DM::Logger(DM::Debug) << name;
            DM::View * view = sys->getViewDefinition(name);
            QTreeWidgetItem * item_view = new QTreeWidgetItem();
            item_view->setText(0, QString::fromStdString(name));
            int type = view->getType();

            if (type == DM::NODE)
                item_view->setText(1, "Nodes");
            if (type == DM::EDGE)
                item_view->setText(1, "Edges");
            if (type == DM::FACE)
                item_view->setText(1, "Faces");
            if (type == DM::SYSTEM)
                item_view->setText(1, "Systems");
            if (type == DM::RASTERDATA)
                item_view->setText(1, "Raster Data");

            root_port->addChild(item_view);

            DM::Component * c = sys->getComponent(view->getIdOfDummyComponent());
            if (c == 0) {
                continue;
            }
            std::map<std::string,DM::Attribute*> attributes = c->getAllAttributes();
            for (std::map<std::string,DM::Attribute*>::const_iterator it  = attributes.begin(); it != attributes.end(); ++it) {
                DM::Logger(DM::Debug) << it->first;
                QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
                item_attribute->setText(0, QString::fromStdString(it->first));
                item_view->addChild(item_attribute);
            }
        }
        this->ui->treeWidget->expandItem(root_port);
    }





}

GUIViewDataForModules::~GUIViewDataForModules()
{
    delete ui;
}
