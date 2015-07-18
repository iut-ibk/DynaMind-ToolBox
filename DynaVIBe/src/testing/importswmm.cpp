/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair

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

#include <importswmm.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <sewerviewdef.h>

using namespace DM;

DM_DECLARE_NODE_NAME(ImportSWMM,Sewersystems)

ImportSWMM::ImportSWMM()
{
	this->append=false;
	this->inpfilepath = "";
	this->addParameter("SWMM input file:", DM::FILENAME, &this->inpfilepath);
	this->addParameter("AppendToExistingDataStream", DM::BOOL, &this->append);

}

void ImportSWMM::init()
{
    S::ViewDefinitionHelper sd;
    this->data = sd.getAll(DM::WRITE);

	if (this->append)
		this->data = sd.getAll(DM::MODIFY);

    this->addData("Sewer", data);
}

void ImportSWMM::run()
{
	this->sys = this->getData("Sewer");

	if(!loadProject(this->inpfilepath))
		DM::Logger(DM::Error) << "Cannot read SWMM input file";

	return;
}

bool ImportSWMM::loadProject(std::string filename)
{
	S::ViewDefinitionHelper sd;

	categories tables;

	//check file
	if(!QFile::exists(QString::fromStdString(filename)))
			return false;

	QFile file(QString::fromStdString(filename));

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	//read inp file
	QTextStream stream(&file);

	QStringList currentline = getNextValidLine(&stream);

	while(!stream.atEnd())
	{
		//check for inp sections
		if(currentline[0].startsWith('[') && currentline[0].endsWith(']'))
		{
			QString sectionname = currentline[0].mid(1,currentline[0].size()-2).toUpper();

			tables[sectionname] = new rows();

			currentline = getNextValidLine(&stream);

			//fill table with values
			while(currentline.size() > 0 && !currentline[0].startsWith('[') && !currentline[0].endsWith(']'))
			{
				tables[sectionname]->insert(currentline[0], new QStringList(currentline));
				currentline = getNextValidLine(&stream);

				continue;
			}
		}
		else
		{
			currentline = getNextValidLine(&stream);
		}
	}

	//extract to dynamind
	QMap<QString,DM::Component*> elements;

	//NODES
	addNodesFromSection("JUNCTIONS",tables,elements,sd.getCompleteView(S::JUNCTION,DM::MODIFY));
	addNodesFromSection("OUTFALLS",tables,elements,sd.getCompleteView(S::OUTFALL,DM::MODIFY));
	addNodesFromSection("STORAGE",tables,elements,sd.getCompleteView(S::STORAGE,DM::MODIFY));

	//EDGES
	addEdgesFromSection("CONDUITS",tables,elements,sd.getCompleteView(S::CONDUIT,DM::MODIFY));
	addEdgesFromSection("PUMPS",tables,elements,sd.getCompleteView(S::PUMP,DM::MODIFY));

	//cleanup
	for (categories::iterator it = tables.begin(); it != tables.end(); ++it)
		qDeleteAll(*(it.value()));

	qDeleteAll(tables);
	tables.clear();

	file.close();
	return true;
}

QStringList ImportSWMM::getNextValidLine(QTextStream *stream)
{
	while (!stream->atEnd())
	{
		QString line = stream->readLine();
		QString trimmedline = line.trimmed().simplified();

		//tokenize line
		QStringList tokens = trimmedline.split(" ");

		//skip empty line
		if( trimmedline.isEmpty() )
			continue;

		//skip comment line
		if(trimmedline.startsWith(';'))
			continue;

		if(trimmedline.split(";").size() > 1)
			tokens = trimmedline.split(";")[0].trimmed().simplified().split(" ");

		return tokens;
	}

	return QStringList();
}

bool ImportSWMM::addEdgesFromSection(QString name, ImportSWMM::categories &tables, QMap<QString,DM::Component*> &elements, DM::View view)
{
	if(tables.contains(name))
	{
		DM::Logger(DM::Standard) << "Found valid " << name.toStdString() << " section";

		rows edges = *tables[name];

		for (rows::iterator it = edges.begin(); it != edges.end(); ++it)
		{
			QString currentkey = it.key();
			QStringList cvalue = *(it.value());
			DM::Node* startnode = dynamic_cast<DM::Node*>(elements[cvalue[1]]);
			DM::Node* endnode = dynamic_cast<DM::Node*>(elements[cvalue[2]]);

			if(startnode && endnode)
			{
				if(elements.contains(currentkey+"_E"))
					DM::Logger(DM::Error) << "Edge id " << currentkey.toStdString() << " is not unique";

				elements[currentkey+"_E"] = this->sys->addEdge(startnode,endnode,view);
			}
		}
	}

	return true;
}

bool ImportSWMM::addNodesFromSection(QString name, ImportSWMM::categories &tables, QMap<QString,DM::Component*> &elements, DM::View view)
{
	if(tables.contains(name) && tables.contains("COORDINATES"))
	{
		DM::Logger(DM::Standard) << "Found valid " << name.toStdString() << " section";

		rows nodes = *tables[name];
		rows coordinates = *tables["COORDINATES"];

		for (rows::iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			QString currentkey = it.key();
			QStringList nvalue = *(it.value());
			QStringList coordinate = *(coordinates.find(currentkey).value());

			if(elements.contains(currentkey))
				DM::Logger(DM::Error) << "Node id " << currentkey.toStdString() << " is not unique";

			elements[currentkey]=this->sys->addNode(coordinate[1].toDouble(),coordinate[2].toDouble(),nvalue[1].toDouble(),view);
			elements[currentkey]->addAttribute("Name",currentkey.toStdString());
		}
	}

	return true;
}
