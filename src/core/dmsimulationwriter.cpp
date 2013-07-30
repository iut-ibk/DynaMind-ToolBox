/**
* @file
* @author  Chrisitan Urich <christian.urich@gmail.com>
* @version 1.0
* @section LICENSE
*
* This file is part of DynaMind
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
*/

#include "dmsimulationwriter.h"
#include <dmsimulation.h>
#include <dmmodule.h>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <dmgroup.h>
#include <sstream>

using namespace DM;
/*
SimulationWriter::SimulationWriter()
{
}

std::string SimulationWriter::writeLink(Port * p) {
std::stringstream out;
foreach(ModuleLink *l, p->getLinks()) {
out << "\t\t<Link>\n";
out << "\t\t\t<BackLink value = \"0";
out << l->isBackLink();
out << "\"/>\n";
out << "\t\t\t<InPort>\n";
out << "\t\t\t\t<UUID value = \"";
out << l->getInPort()->getModule()->getUuid();
out << "\"/>\n";
out << "\t\t\t\t<PortName value = \"";
out << l->getInPort()->getLinkedDataName();
out << "\"/>\n";
out << "\t\t\t\t<PortType value = \"";
out << l->getInPort()->isPortTuple();
out << "\"/>\n";
out << "\t\t\t</InPort>\n";

out << "\t\t\t<OutPort>\n";
out << "\t\t\t\t<UUID value = \"";
out << l->getOutPort()->getModule()->getUuid();
out << "\"/>\n";
out << "\t\t\t\t<PortName value = \"";
out << l->getOutPort()->getLinkedDataName();
out << "\"/>\n";
out << "\t\t\t\t<PortType value = \"";
out << l->getOutPort()->isPortTuple();
out << "\"/>\n";
out << "\t\t\t</OutPort>\n";
out << "\t\t</Link>\n";
}
return out.str();
}*/

void writeHead(QTextStream &out)
{
	out << "<DynaMind>\n";
	out << "\t<Info Version=\"0.3\"/>\n";
	out << "<DynaMindCore>\n";
}

void writeModule(QTextStream &out, Module* m, QDir filePath)
{
	Logger(Debug) << "saving module '" << m->getClassName() << "'";
	Module* owner = m->getOwner();

	out  << "\t\t<Node>\n";
	out << "\t\t"<< "\t<ClassName value=\""
		<< QString::fromStdString(m->getClassName()) << "\"/>\n";
	out << "\t\t"<< "\t<UUID value=\""
		//<< QString::fromStdString(m->getUuid()) << "\"/>\n";
		<< ADDRESS_TO_INT(m) << "\"/>\n";
	out << "\t\t"<< "\t<Name value=\""
		<< QString::fromStdString(m->getName()) << "\"/>\n";
	out << "\t\t"<< "\t<GroupUUID value=\""
		<< ADDRESS_TO_INT(owner) << "\"/>\n";
	out << "\t\t"<< "\t<DebugMode value=\""
		<< QString::number(0) << "\"/>\n";

	foreach(Module::Parameter* p, m->getParameters())
	{
		out <<  "\t\t\t<parameter name=\"" << QString::fromStdString(p->name) <<"\">"
			<< "\n" "\t\t\t\t<![CDATA[";

		if(p->type != DM::FILENAME)
			out <<  QString::fromStdString(m->getParameterAsString(p->name));
		else
		{
			QString path = QString::fromStdString(m->getParameterAsString(p->name));
			QString relPath = filePath.relativeFilePath(path);
			DM::Logger(Debug) << "reducing file path '" << path << "'";
			DM::Logger(Debug) << "to '" << relPath << "'";
			out << relPath;
		}

		out << "]]>\n"
			<< "\t\t\t</parameter>\n";
	}

	/*
	std::map<std::string, int> parameterList = m->getParameterList();
	for (std::map<std::string, int>::iterator it = parameterList.begin(); it != parameterList.end(); ++it) {
	Logger(Debug) << it->first;
	}

	for (std::map<std::string, int>::iterator it = parameterList.begin(); it != parameterList.end(); ++it) {
	if (it->second < DM::USER_DEFINED_INPUT) {
	out <<  "\t\t\t<parameter name=\"" << QString::fromStdString(it->first) <<"\">"
	<< "\n" "\t\t\t\t<![CDATA["
	<<  QString::fromStdString(m->getParameterAsString(it->first))
	<< "]]>\n"
	<< "\t\t\t</parameter>\n";
	}
	}*/

	out  << "\t\t</Node>\n";
}


void writeLink(QTextStream &out, Simulation::Link* l)
{
	out << "\t\t<Link>\n";
	out << "\t\t\t<BackLink value = \"0\"/>\n";
	out << "\t\t\t<InPort>\n";
	out << "\t\t\t\t<UUID value = \"" << ADDRESS_TO_INT(l->dest) << "\"/>\n";
	out << "\t\t\t\t<PortName value = \"" << QString::fromStdString(l->inPort) << "\"/>\n";
	out << "\t\t\t\t<PortType value = \"0\"/>\n";
	out << "\t\t\t</InPort>\n";

	out << "\t\t\t<OutPort>\n";
	out << "\t\t\t\t<UUID value = \"" << ADDRESS_TO_INT(l->src) << "\"/>\n";
	out << "\t\t\t\t<PortName value = \"" << QString::fromStdString(l->outPort) << "\"/>\n";
	out << "\t\t\t\t<PortType value = \"0\"/>\n";
	out << "\t\t\t</OutPort>\n";
	out << "\t\t</Link>\n";
}

void SimulationWriter::writeSimulation(QIODevice* dest, QString filePath, 
									   const std::list<Module*>& modules, 
									   const std::list<Simulation::Link*>& links,
									   Module* root) 
{
	dest->open(QIODevice::WriteOnly);
	Logger(Debug) << "Saving File";

	/*QFile file(QString::fromStdString(filename));
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);*/
	QTextStream out(dest);

	writeHead(out);

	// dump groups and modules
	out << "\t<Nodes>\n";

	out << "\t\t<RootNode>\n";
	out << "\t\t\t<UUID value=\"" << ADDRESS_TO_INT(root) << "\"/>\n";
	out << "\t\t</RootNode>\n";
	 
	QDir filedir = QFileInfo(filePath).absoluteDir();
	Logger(Debug) << "Number of Modules " << modules.size();
	foreach(Module * m, modules) 
		writeModule(out, m, filedir);

	out << "\t</Nodes>\n";

	// dump links
	out << "\t<Links>\n";

	foreach(Simulation::Link* l, links)
		writeLink(out, l);

	/*foreach(Module * m, modules) {
	foreach(Port * p, m->getOutPorts()) {
	out << QString::fromStdString(SimulaitonWriter::writeLink(p));
	}
	if (m->isGroup()) {
	Group * g = (Group * ) m;
	foreach(PortTuple * pt, g->getInPortTuples()) {
	out << QString::fromStdString(SimulaitonWriter::writeLink(pt->getOutPort()));
	}
	foreach(PortTuple * pt, g->getOutPortTuples()) {
	out << QString::fromStdString(SimulaitonWriter::writeLink(pt->getOutPort()));
	}
	}
	}*/

	out << "\t</Links>\n";

	out << "</DynaMindCore>\n";
	//out << "</DynaMind>"<< "\n";

	//file.close();
	dest->close();
	Logger(Debug) << "Finished saving file";
}

