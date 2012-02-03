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
#ifndef LOGGER_H
#define LOGGER_H

#include <vibe_log.h>
#include <compilersettings.h>
#include <QMutex>
#include <QMutexLocker>

//using namespace boost;
using namespace std;

class QString;
namespace DM {
    //class Variable;

    class DM_HELPER_DLL_EXPORT Logger {
    public:
        Logger(LogLevel level = Standard);
        virtual ~Logger();
        Logger &operator<< (LogLevel level);
        //Logger &operator<< (const Variable *var);
        Logger &operator<< (const char* s);
        Logger &operator<< (const int i);
        Logger &operator<< (const size_t i);
        Logger &operator<< (const long i);
        Logger &operator<< (const double f);
        Logger &operator<< (const float f);
        Logger &operator<< (const string &i);
        Logger &operator<< (const QString &s);
    private:
        string logLevel() const;
        string date() const;

        LogSink &sink;
        LogLevel level, max;
        QMutex mutex;
        string logstring;
        bool dirty;
    };
}

#endif // LOGGER_H
