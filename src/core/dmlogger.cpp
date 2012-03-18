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
#include <dmlogger.h>
#include <QDateTime>
#include <QString>
#include <dmlogsink.h>

namespace DM {
    Logger::Logger(LogLevel level)
        : sink(*Log::getInstance()->sink), level(level) {
        dirty = false;
        this->max = Log::getInstance()->max;
        logstring="";
        if (level >= max) {
            logstring += logLevel() + " " + date() + "|";
            dirty = true;
        }
    }

    Logger::~Logger() {
        sink << logstring;
        if (dirty)
            sink << LSEndl();
    }

    Logger &Logger::operator <<(LogLevel new_level) {
        level = new_level;
        logstring += "\n" + logLevel() + " " + date() + "|";
        dirty = true;
        return *this;
    }



    Logger &Logger::operator<< (const char* s) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + QString(s).toStdString();
        dirty = true;
        return *this;
    }

    Logger &Logger::operator<< (const int i) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + QString::number(i).toStdString();
        dirty = true;
        return *this;
    }

    Logger &Logger::operator<< (const size_t i) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + QString::number(i).toStdString();
        dirty = true;
        return *this;
    }

    Logger &Logger::operator<< (const long i) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + QString::number(i).toStdString();
        dirty = true;
        return *this;
    }

    Logger &Logger::operator<< (const double f) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + QString::number(f).toStdString();
        dirty = true;
        return *this;
    }

    Logger &Logger::operator<< (const float f) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + QString::number(f).toStdString();
        dirty = true;
        return *this;
    }

    Logger &Logger::operator<< (const string &s) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + s;
        dirty = true;
        return *this;
    }

    Logger &Logger::operator<< (const QString &s) {
        QMutexLocker locker(&mutex);
        if (level < max) {
            return *this;
        }
        logstring +=  " " + s.toStdString();
        dirty = true;
        return *this;
    }


    string Logger::logLevel() const {
        switch (level) {
        case Debug:
            return "DEBUG\t";
        case Warning:
            return "WARN\t";
        case Standard:
            return "STANDARD\t";
        case Error:
            return "ERROR\t";
        }
        return "UNKNOWN\t";
    }

    string Logger::date() const {
        return QDateTime::currentDateTime().toString().toStdString();
    }

}
