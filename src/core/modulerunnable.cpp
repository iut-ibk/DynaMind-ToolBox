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
#include "modulerunnable.h"

#include <QThreadPool>
#include <module.h>
#include <group.h>
#include <time.h>
#include <vibe_logger.h>

ModuleRunnable::ModuleRunnable(vibens::Module * m)
{
    this-> m = m;
    this->setAutoDelete(true);
}

void ModuleRunnable::run() {
    clock_t start, finish;
    vibens::Logger(vibens::Standard) << "Start\t" << m->getName()<< " " << m->getUuid() << " Counter " << m->getInternalCounter();
    start = clock();

    m->updateParameter();
    m->run();

    m->setParameter();
    finish = clock();
    vibens::Logger(vibens::Standard) << "Success\t" << m->getName()<< " " << m->getUuid() << " Counter " << m->getInternalCounter()  <<  "\t time " <<  ( double (finish - start)/CLOCKS_PER_SEC );
    vibens::Group * g = m->getGroup();
    if (g!=0 && !m->isGroup())
        g->finishedModule(this->m);
}
