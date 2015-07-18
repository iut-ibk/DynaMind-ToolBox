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
#include <iostream>
#include "dmmainwindow.h"
#include <sstream>
#include <cstdio>
#include <QApplication>
#include <QSplashScreen>
#include <QThreadPool>
#include <dynamindmainwindow.h>
#ifdef VIEWER
#include <QGLFormat>
#endif
int main(int argc, char *argv[]) 
{
	QApplication app(argc, argv);
	Q_INIT_RESOURCE(icons);
	Q_INIT_RESOURCE(splash);
	QApplication::setWindowIcon(QIcon(":/ressources/dynamind-icon.png"));
	//QApplication::setGraphicsSystem("native");
	QCoreApplication::setOrganizationName("IUT");
	QCoreApplication::setApplicationName("DYNAMIND");

#ifdef VIEWER
	QGLFormat glf = QGLFormat::defaultFormat();
	glf.setSampleBuffers(true);
	glf.setSamples(4);
	QGLFormat::setDefaultFormat(glf);
#endif

	QPixmap pixmap(":/Splash/ressources/logo.png");
	QSplashScreen splash1;
	splash1.show();
	app.processEvents();
	QThreadPool::globalInstance()->setMaxThreadCount(0);

	DMMainWindow mw;
	mw.show();
	splash1.finish(&mw);

	return app.exec();
}
