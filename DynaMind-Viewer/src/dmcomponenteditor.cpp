/**
 * @file
 * @author  Gregor Burger <burger.gregor@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Gregor Burger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "dmcomponenteditor.h"
#include "ui_dmcomponenteditor.h"

#include "dmcomponent.h"
#include "dmattribute.h"
#include "dmlogger.h"
#include <QDateTime>
#include <QTreeWidgetItem>
#include <QVector>
#include "qcustomplot.h"

namespace DM {

ComponentEditor::ComponentEditor(Component *c, QWidget *parent) :
	QDialog(parent),
	c(c),
	ui(new Ui::ComponentEditor) 
{
	ui->setupUi(this);

	foreach(DM::Attribute *attr, c->getAllAttributes()) 
	{
		QStringList strings;
		strings << QString::fromStdString(attr->getName());

		strings << attr->getTypeName();

		switch (attr->getType())
		{
		case Attribute::DOUBLE:
			strings << QString("%1").arg(attr->getDouble());
			break;
		case Attribute::STRING:
			strings << QString::fromStdString(attr->getString());
			break;
		case Attribute::STRINGVECTOR:
			{
				QString s;
				foreach(const std::string& str, attr->getStringVector())
					s.append(QString::fromStdString(str) + "; ");

				strings << s;
			}
			break;
		case Attribute::TIMESERIES:
			strings << "...";
			break;
		case Attribute::DOUBLEVECTOR:
			{
				QString s;
				foreach(double d, attr->getDoubleVector())
					s.append(QString("%1; ").arg(d));

				strings << s;
			}
			break;
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


	QVector<double> y = QVector<double>::fromStdVector(attr->getDoubleVector());
	QVector<double> x(y.size());

	double timedelta = 0;

	if (attr->getType() == Attribute::TIMESERIES) {
		std::vector<std::string> dates = attr->getStringVector();
		//Convert String To dates
		QDateTime startDate = QDateTime::fromString( QString::fromStdString(dates[0]), Qt::ISODate);
		QDateTime endDate = QDateTime::fromString( QString::fromStdString(dates[y.size()-1]), Qt::ISODate);
		timedelta = startDate.secsTo(endDate);
		for (int i = 0; i < y.size(); i++) {
			QDateTime date(QDateTime::fromString(QString::fromStdString(dates[i]), Qt::ISODate) );
			if (!date.isValid()) {
				DM::Logger(DM::Debug) << "Can't show data use correct time format yyyy-mm-ddThh:mm:ss";
				return;
			}
			QDateTime datetime(date);
			double val =  datetime.toMSecsSinceEpoch()/1000;
			x[i] =val;
		}
	} else {
		std::generate_n(x.begin(), y.size(), inc());
	}

	ui->plot->addGraph();
	ui->plot->graph(0)->setData(x, y);
	ui->plot->graph(0)->rescaleAxes();
	if (attr->getType() == Attribute::TIMESERIES) {
		if (timedelta > 24 * 60 * 60) {
			ui->plot->graph(0)->keyAxis()->setTickLabelType(QCPAxis::ltDateTime);
			ui->plot->graph(0)->keyAxis()->setDateTimeFormat("yyyy-MM-dd");
		}
		if (timedelta > 250 * 24 * 60 * 60) {
			ui->plot->graph(0)->keyAxis()->setTickLabelType(QCPAxis::ltDateTime);
			ui->plot->graph(0)->keyAxis()->setDateTimeFormat("yyyy-MM");
		}
		if (timedelta > 3 * 365 * 24 * 60 * 60) {
			ui->plot->graph(0)->keyAxis()->setTickLabelType(QCPAxis::ltDateTime);
			ui->plot->graph(0)->keyAxis()->setDateTimeFormat("yyyy");
		}

	}
	ui->plot->replot();
}

} // namespace DM

