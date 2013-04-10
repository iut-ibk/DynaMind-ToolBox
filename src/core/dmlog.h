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
#ifndef DMLOG_H
#define DMLOG_H

#include <iostream>
#include <string>
#include <vector>
#include "dmcompilersettings.h"

using namespace std;

namespace DM {
//class Node;
class Simulation;
class Logger;
class LogSink;

enum LogLevel {
    Debug = 0,
    Standard = 1,
    Warning = 2,
    Error = 3
};

#define LOG_HEAD \
    if (level <= max) return *this;\
    out << logLevel() << ": " << "\n"
/**
 * @ingroup DynaMind-Core
 * @brief DynaMind Log
 *
 * The Logger is implemented as singleton. The Logger need to initialsed by calling the init function.
 * With the init function a LogSink is registered and the max LogLevel is defined.
 * Following LogLevels can be used:
 * - Debug
 * - Standard
 * - Warning
 * - Error
 * Error is the highest and Debug the lowest LogLevel. Only messages with the current LogLevel or higher are send to the sink.
 * After Log is initalised messages the logger can be used like DM::Logger(DM::Standard) << "Test"
 *
 * Sample code to redirect logging to cout
 * @code
 * //redirect out to cout
 * ostream *out = &cout;
 * //Create a new LogSink (cout) and use the Standard LogLevel
 * DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
 * DM::Logger(DM::Standard) << "Hello Logger";
 * @endcode
 *
 */
class DM_HELPER_DLL_EXPORT Log
{
public:
    /** @brief Initialise Logger
     *
     * If no instance of the logger exists. A new instance is created.
     * Log takes ownership of the instance
     */
    static void init(LogSink *sink, LogLevel max = Debug);
    static void addLogSink(LogSink *sink);
    /** @brief Delets sink and instance*/
    static void shutDown();
    /** @brief Returns the current Instance */
    static Log *getInstance();
    friend class Logger;
    virtual ~Log();

private:
    Log();

    static Log *instance;

    std::vector<LogSink*> *sinks;
    LogLevel max;
};
}
#endif // LOG_H
