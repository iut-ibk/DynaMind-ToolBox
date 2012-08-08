#include "dmcomponenteditor.h"
#include "ui_dmcomponenteditor.h"

#include "dmcomponent.h"
#include "dmattribute.h"

#include <QTreeWidgetItem>
#include <QVector>
#include "qcustomplot.h"

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
        
        strings << attr->getTypeName();
        
        if (attr->getType() == Attribute::DOUBLE) {
            strings << QString("%1").arg(attr->getDouble());
        }
        
        if (attr->getType() == Attribute::STRING) {
            strings << QString::fromStdString(attr->getString());
        }
        
        if (attr->getType() == Attribute::STRINGVECTOR) {
            QString s;
            foreach (double d , attr->getDoubleVector()) {
                s.append(QString("%1; ").arg(d));
            }
            strings << s;
        }
        
        if (attr->getType() == Attribute::TIMESERIES ||
            attr->getType() == Attribute::DOUBLEVECTOR) {
            strings << "...";
        }
        
        ui->attributeList->insertTopLevelItem(0, new QTreeWidgetItem(strings));
    }
    
}

ComponentEditor::~ComponentEditor() {
    delete ui;
}

struct inc {
    inc(int start = 0) : start(start) {
        
    }
    
    double operator()() {
        return (double) start++;
    }
    int start;
};

void ComponentEditor::on_attributeList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
    if (ui->plot->graphCount()) {
        ui->plot->removeGraph(0);
        ui->plot->replot();
    }
    if (!current) return;
    
    std::string name = current->text(0).toStdString();
    Attribute *attr = c->getAttribute(name);
    if (!(attr->getType() == Attribute::TIMESERIES ||
          attr->getType() == Attribute::DOUBLEVECTOR)) {
        return;
    }
    
    ui->plot->addGraph();
    QVector<double> y = QVector<double>::fromStdVector(attr->getDoubleVector());
    QVector<double> x(y.size());
    std::generate_n(x.begin(), y.size(), inc());
    ui->plot->graph(0)->setData(x, y);
    ui->plot->graph(0)->rescaleAxes();
    ui->plot->replot();
}

} // namespace DM

