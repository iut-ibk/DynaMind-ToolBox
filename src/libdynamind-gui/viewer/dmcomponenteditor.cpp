#include "dmcomponenteditor.h"
#include "ui_dmcomponenteditor.h"

#include "dmcomponent.h"
#include "dmattribute.h"

#include <QTreeWidgetItem>

namespace DM {

ComponentEditor::ComponentEditor(Component *c, QWidget *parent) :
    QDialog(parent),
    c(c),
    ui(new Ui::ComponentEditor) {
    ui->setupUi(this);
    
    
    QMap<std::string, DM::Attribute *> attributes(c->getAllAttributes());
    
    foreach (DM::Attribute *attr, attributes.values()) {
        QStringList strings;
        strings << QString::fromStdString(attr->getName());
        
        if (attr->hasDouble()) {
            strings << "Double";
            strings << QString("%1").arg(attr->getDouble());
        }
        //if (attr->hasDouble()) strings << "Double";
        if (attr->hasString()) {
            strings << "Double";
            strings << QString::fromStdString(attr->getString());
        }
        
        if (attr->hasDoubleVector()) {
            strings << "Double Vector";
            QString s;
            foreach (double d , attr->getDoubleVector()) {
                s.append(QString("%1; ").arg(d));
            }
            strings << s;
        }
        
        if (attr->hasDoubleVector()) {
            strings << "Double Vector";
            QString s;
            foreach (std::string sv , attr->getStringVector()) {
                s.append(QString::fromStdString(sv));
            }
            strings << s;
        }
        
        ui->attributeList->insertTopLevelItem(0, new QTreeWidgetItem(strings));
    }
    
}

ComponentEditor::~ComponentEditor() {
    delete ui;
}

} // namespace DM
