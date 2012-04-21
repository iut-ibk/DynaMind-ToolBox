/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
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

#include <ostream>
#include <QtTest/QtTest>
#include "testsimulation.h"
#include <dmmodule.h>
#include <dmsimulation.h>
#include <dmlog.h>
#include "testmodules.h"

void TestModules::setParameterInModule() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Load Native Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    QVERIFY(m != 0);
    m->setParameterValue("DoubleValue", "0.1");
    double val = m->getParameter<double>("DoubleValue");
    QVERIFY(val == 0.1);
}

QTEST_MAIN( TestModules )
#include "testmodules.moc"
