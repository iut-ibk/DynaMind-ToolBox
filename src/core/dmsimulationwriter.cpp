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
#include <dmport.h>
#include <dmmodulelink.h>
#include <dmgroup.h>
#include <dmporttuple.h>
#include <sstream>

using namespace DM;

SimulationWriter::SimulationWriter()
{
}
/*
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
	out << "\t<Info Version=\"0.3/\">\n";
	out << "<DynaMindCore>\n";
}

void writeModule(QTextStream &out, Module* m)
{
	Logger(Debug) << "saving module '" << m->getClassName() << "'";

	out  << "\t\t<Node>\n";
	out << "\t\t"<< "\t<ClassName value=\""
		<< QString::fromStdString(m->getClassName()) << "\"/>\n";
	out << "\t\t"<< "\t<UUID value=\""
		//<< QString::fromStdString(m->getUuid()) << "\"/>\n";
		<< QString::fromAscii((char*)m, sizeof(Module*)) << "\"/>\n";
	out << "\t\t"<< "\t<Name value=\""
		<< QString::fromStdString(m->getName()) << "\"/>\n";
	out << "\t\t"<< "\t<GroupUUID value=\""
		<< QString::fromAscii((char*)m->getOwner(), sizeof(Module*)) << "\"/>\n";
	out << "\t\t"<< "\t<DebugMode value=\""
		<< QString::number(0) << "\"/>\n";

	foreach(Module::Parameter* p, m->getParameters())
	{
		out <<  "\t\t\t<parameter name=\"" << QString::fromStdString(p->name) <<"\">"
			<< "\n" "\t\t\t\t<![CDATA["
			<<  QString::fromStdString(m->getParameterAsString(p->name))
			<< "]]>\n"
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
	QString src = QString::fromAscii((char*)l->src, sizeof(Module*));
	QString dest = QString::fromAscii((char*)l->dest, sizeof(Module*));

	out << "\t\t<Link>\n";
	out << "\t\t\t<BackLink value = \"0\"/>\n";
	out << "\t\t\t<InPort>\n";
	out << "\t\t\t\t<UUID value = \"" << dest << "\"/>\n";
	out << "\t\t\t\t<PortName value = \"" << QString::fromStdString(l->inPort) << "\"/>\n";
	out << "\t\t\t\t<PortType value = \"0\"/>\n";
	out << "\t\t\t</InPort>\n";

	out << "\t\t\t<OutPort>\n";
	out << "\t\t\t\t<UUID value = \"" << src << "\"/>\n";
	out << "\t\t\t\t<PortName value = \"" << QString::fromStdString(l->outPort) << "\"/>\n";
	out << "\t\t\t\t<PortType value = \"0\"/>\n";
	out << "\t\t\t</OutPort>\n";
	out << "\t\t</Link>\n";
}

void SimulationWriter::writeSimulation(std::string filename, Simulation *sim) 
{
	std::list<Module*> modules = sim->getModules();
	Logger(Debug) << "Saving File";

	QFile file(QString::fromStdString(filename));
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);

	writeHead(out);

	// dump groups and modules
	out << "\t<Nodes>\n";

	out << "\t\t<RootNode>\n";
	out << "\t\t\t<UUID value=\"0\"/>\n";
	out << "\t\t</RootNode>\n";


	Logger(Debug) << "Number of Modules " << modules.size();
	foreach(Module * m, modules) 
		writeModule(out, m);

	out << "\t</Nodes>\n";

	// dump links
	out << "\t<Links>\n";

	foreach(Simulation::Link* l, sim->getLinks())
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

	file.close();
	Logger(Debug) << "Finished saving file";
}

