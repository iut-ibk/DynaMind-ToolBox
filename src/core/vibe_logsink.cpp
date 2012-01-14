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
#include <vibe_logsink.h>

namespace DM {
    OStreamLogSink::OStreamLogSink(std::ostream &ostream) : out(ostream) {
        mutex = new QMutex(QMutex::Recursive);
    }

    OStreamLogSink::~OStreamLogSink()
    {
        delete mutex;
    }

    LogSink &OStreamLogSink::operator<<(const std::string &string) {
        QMutexLocker locker(mutex);
        out << string;
        return *this;
    }

    LogSink &OStreamLogSink::operator<<(const char *string) {
        QMutexLocker locker(mutex);
        out << string;
        return *this;
    }

    /*LogSink &OStreamLogSink::operator<<(int i) {
        QMutexLocker locker(mutex);
        out << i;
        return *this;
    }*/


    LogSink &OStreamLogSink::operator<<(double f) {
        QMutexLocker locker(mutex);
        out << f;
        return *this;
    }
    LogSink &OStreamLogSink::operator<<(LSEndl e) {
        QMutexLocker locker(mutex);
        out << std::endl;
        return *this;
    }
}
