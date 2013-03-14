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
#define BOOST_ALL_DYN_LINK
#include "dmcompilersettings.h"

#include <iostream>
#include <vector>
//#include <boost/python.hpp>
#include <boost/program_options.hpp>
#include "dmsimulation.h"
//#include "dmsimulationwrapper.h"
#include <stdio.h>
#include <stdlib.h>
//#include <simulation.h>
//#include <database.h>
//#include <datamanagement.h>
//#include <porttuple.h>
//#include <vibe_log.h>
//#include <vibe_logger.h>
//#include <vibe_logsink.h>
#include <QThreadPool>

#include <ostream>
#include <dmlog.h>

using namespace std;
//using namespace boost::python;
namespace po = boost::program_options;
//using namespace vibens;
typedef boost::error_info<struct tag_errno,int> errno_info;
typedef boost::error_info<struct errinfo_type_info_name_,std::string> errinfo_type_info_name;

int main(int argc, char *argv[], char *envp[]) {
    QThreadPool::globalInstance()->setMaxThreadCount(1);

    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "start";

    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("input-file", po::value<string>(), "set simulation file")
            ("repeat", po::value<int>(), "repeat simulation")
            ("python-modules", po::value<vector <string> >(), "set path to python modules")
            ;
    std::string simulationfile;
    std::vector<std::string> pythonModules;
    int repeat;

    try {

        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 1;
        }

        if (vm.count("input-file")) {
            simulationfile =  vm["input-file"].as<string>();
        }  else {
            std::cout << "Simulation file not set" << std::endl;
            return -1;
        }
        if (vm.count("python-modules")) {
            pythonModules =  vm["python-modules"].as<vector <string> >();
        }
        if (vm.count("repeat")) {
            repeat =  vm["repeat"].as<int> ();
        }

    }
    catch (po::unknown_option & e){
        std::cout << "unknown option" << std::endl;
        cout << desc << "\n";

    }
    //modified
	
    DM::Simulation s;
	std::cout << "loading modules" << std::endl;
	s.loadModulesFromDefaultLocation();
	std::cout << "loading simulation file" << std::endl;
    s.loadSimulation(simulationfile);
	std::cout << "starting simulation" << std::endl;
    s.run();
	std::cout << "finished simulation" << std::endl;
    QThreadPool::globalInstance()->waitForDone();

}
