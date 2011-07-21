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
#include "vibe_log.h"
#include <iostream>
#include <vibe_logger.h>
#include <vibe_logsink.h>
#include <ostream>
#include <assert.h>

using namespace std;

namespace vibens{
Log *Log::instance = 0;

Log *Log::getInstance() {
        if (!instance) {
                cerr << "log not initialed, call Log::init" << endl;
                assert(instance);
        }
        return instance;
}

void Log::init(LogSink *sink, LogLevel max) {
        if (!instance) {
                instance = new Log();
        }
        instance->sink = sink;
        instance->max = max;
}

void Log::shutDown() {
        instance->sink->close();
        delete instance->sink;
        instance->sink = 0;
        delete instance;
}

Log::Log(){}

Log::~Log(){}
}
