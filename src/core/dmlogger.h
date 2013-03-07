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
#ifndef DMLOGGER_H
#define DMLOGGER_H

#include <dmlog.h>
#include <dmcompilersettings.h>
#include <QMutex>
#include <QMutexLocker>

using namespace std;

class QString;
namespace DM {
/**
 * @ingroup DynaMind-Core
 * @brief The Logger class is used to get messages from the core, modules, database.
 *
 * The Logger sends the messages to the sink defined in Log.
 **/
    class DM_HELPER_DLL_EXPORT Logger {
    public:
        /** @brief Creates a new Log entry.
         *
         * As default LogLevel the Logger uses Standard. (LogeLevels see Log)
         * Creates the log string with LogLevel and data
         */
        Logger(LogLevel level = Standard);
        /** @brief sends log string to the registed sind */
        virtual ~Logger();
        /** @brief Sends LogLevel */
        Logger &operator<< (LogLevel level);
        /** @brief Sends a char* to the sink*/
        Logger &operator<< (const char* s);
        /** @brief Sends an int to the sink*/
        Logger &operator<< (const int i);
        /** @brief Sends a size_t to the sink*/
        Logger &operator<< (const size_t i)
        /** @brief Sends a long to the sink*/;
        Logger &operator<< (const long i);
        /** @brief Sends a double to the sink*/
        Logger &operator<< (const double f);
        /** @brief Sends a float to the sink*/
        Logger &operator<< (const float f);
        /** @brief Sends a string to the sink*/
        Logger &operator<< (const string &i);
        /** @brief Sends a QString to the sink*/
        Logger &operator<< (const QString &s);
    private:
        string logLevel() const;
        string date() const;

        LogLevel level, max;
        QMutex mutex;
        string logstring;
        bool dirty;
    public:
        std::vector <LogSink*> sinks;
        //LogSink &sink;
    };
}

#endif // LOGGER_H
