/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#include "simulaitonwriter.h"
#include <simulation.h>
#include <module.h>
#include <vector>
#include <QFile>
#include <QTextStream>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include <port.h>
#include <modulelink.h>
#include <group.h>
#include <porttuple.h>
#include <sstream>
using namespace boost;

namespace DM {
    SimulaitonWriter::SimulaitonWriter()
    {
    }
    std::string   SimulaitonWriter::writeLink(Port * p) {
        std::stringstream out;
        BOOST_FOREACH(ModuleLink *l, p->getLinks()) {
            out << "\t\t" << "<Link>" << "\n";
            out << "\t\t\t" << "<BackLink value = \"";
            out << l->isBackLink();
            out << "\"/>" << "\n";
            out << "\t\t\t" << "<InPort>" << "\n";
            out << "\t\t\t\t" << "<UUID value = \"";
            out << l->getInPort()->getModule()->getUuid();
            out << "\"/>" << "\n";
            out << "\t\t\t\t" << "<PortName value = \"";
            out << l->getInPort()->getLinkedDataName();
            out << "\"/>" << "\n";
            out << "\t\t\t\t" << "<PortType value = \"";
            out << l->getInPort()->isPortTuple();
            out << "\"/>" << "\n";
            out << "\t\t\t" << "</InPort>" << "\n";

            out << "\t\t\t" << "<OutPort>" << "\n";
            out << "\t\t\t\t" << "<UUID value = \"";
            out << l->getOutPort()->getModule()->getUuid();
            out << "\"/>" << "\n";
            out << "\t\t\t\t" << "<PortName value = \"";
            out << l->getOutPort()->getLinkedDataName();
            out << "\"/>" << "\n";
            out << "\t\t\t\t" << "<PortType value = \"";
            out << l->getOutPort()->isPortTuple();
            out << "\"/>" << "\n";
            out << "\t\t\t" << "</OutPort>" << "\n";
            out << "\t\t" << "</Link>" << "\n";
        }
        return out.str();
    }

    void SimulaitonWriter::writeSimulation(std::string filename, Simulation *sim) {
        std::vector<Module*> modules = sim->getModules();
        Logger(Debug) << "Save File";

        QFile file(QString::fromStdString(filename));
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << "<VIBe2>" << "\n";
        out << "\t"<<"<Info Version=\"0.3/\">" << "\n";
        out << "<VIBe2Core>" << "\n";
        out << "\t"<<"<Nodes>" << "\n";
        out  << "\t" << "\t"<<"<RootNode>" << "\n";
        out << "\t" << "\t"<< "\t" << "<UUID value=\""
                << QString::fromStdString(sim->getRootGroup()->getUuid()) << "\"/>" << "\n";
        out  << "\t" << "\t"<<"</RootNode>" << "\n";
         Logger(Debug) << "Number of Modules " << modules.size();
        BOOST_FOREACH(Module * m, modules) {
            Logger(Debug) << m->getClassName() << m->getUuid();
            out  << "\t" << "\t"<<"<Node>" << "\n";

            out << "\t" << "\t"<< "\t" << "<ClassName value=\""
                    << QString::fromStdString(m->getClassName()) << "\"/>" << "\n";

            out << "\t" << "\t"<< "\t" << "<UUID value=\""
                    << QString::fromStdString(m->getUuid()) << "\"/>" << "\n";

            out << "\t" << "\t"<< "\t" << "<Name value=\""
                    << QString::fromStdString(m->getName()) << "\"/>" << "\n";
            out << "\t" << "\t"<< "\t" << "<GroupUUID value=\""
                    << QString::fromStdString(m->getGroup()->getUuid()) << "\"/>" << "\n";



            unordered_map<std::string, int> parameterList = m->getParameterList();
            for (unordered_map<std::string, int>::iterator it = parameterList.begin(); it != parameterList.end(); ++it) {
                Logger(Debug) << it->first;
            }

            for (unordered_map<std::string, int>::iterator it = parameterList.begin(); it != parameterList.end(); ++it) {
                if (it->second < VIBe2::USER_DEFINED_INPUT) {
                    out <<  "\t\t\t<parameter name=\"" << QString::fromStdString(it->first) <<"\"" << ">"
                            << "\n" "\t\t\t\t<![CDATA["
                            <<  QString::fromStdString(m->getParameterAsString(it->first))
                            << "]]>" << "\n"
                            << "\t\t\t</parameter>\n";
                }
            }

            out  << "\t" << "\t"<<"</Node>" << "\n";

        }
        out << "\t"<<"</Nodes>" << "\n";

        out << "\t" << "<Links>" << "\n";


        BOOST_FOREACH(Module * m, modules) {
            BOOST_FOREACH(Port * p, m->getOutPorts()) {
                out << QString::fromStdString(SimulaitonWriter::writeLink(p));
            }
            if (m->isGroup()) {
                Group * g = (Group * ) m;
                BOOST_FOREACH(PortTuple * pt, g->getInPortTuples()) {
                  out << QString::fromStdString(SimulaitonWriter::writeLink(pt->getOutPort()));
                }
                BOOST_FOREACH(PortTuple * pt, g->getOutPortTuples()) {
                  out << QString::fromStdString(SimulaitonWriter::writeLink(pt->getOutPort()));
                }
            }
        }

        out << "\t" << "</Links>" << "\n";

        out << "</VIBe2Core>" << "\n";
        //out << "</VIBe2>"<< "\n";

        file.close();




    }
}
