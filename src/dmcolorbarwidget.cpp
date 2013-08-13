/**
* @file
* @author  Chrisitan Urich <christian.urich@gmail.com>
* @version 1.0
* @section LICENSE
*
* This file is part of DynaMind
*
* Copyright (C) 2011-2012  Christian Urich, Markus Sengthaler

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

#include "dmcolorbarwidget.h"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#endif

using namespace DM;

ColorBarWidget::ColorBarWidget(QWidget *parent) :
QGLWidget(parent)
{
	texture = 0;
}

ColorBarWidget::ColorBarWidget(QWidget *parent, QGLWidget *shared) :
	QGLWidget(parent, shared)
{
	texture = 0;
}

void ColorBarWidget::initializeGL()
{
	glClearColor(0, 0.0, 0, 1.0);
}

void ColorBarWidget::paintGL()
{

	if (glIsTexture(texture))
	{
		glEnable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, texture);
		glColor3f(1.0, 1.0, 1.0);

	}
	else
	{
		glDisable(GL_TEXTURE_1D);
		glBindTexture(GL_TEXTURE_1D, 0);
		glColor3f(0,0,0);
	}

	glBegin(GL_QUADS);

	glTexCoord1f(0.0);
	glVertex2f(0, height());

	glTexCoord1f(1.0);
	glVertex2f(width(), height());

	glTexCoord1f(1.0);
	glVertex2f(width(), 0);

	glTexCoord1f(0.0);
	glVertex2f(0, 0);
	glEnd();

	glDisable(GL_TEXTURE_1D);
}

void ColorBarWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
}
