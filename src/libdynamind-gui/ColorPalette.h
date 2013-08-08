/**
 * @file
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2013 Markus Sengthaler

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

#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QColor>
#include <QSettings>


/*#define COLOR_RASTERPORT  QColor(11,72,107);
#define COLOR_VECTORPORT  QColor(59,134,134);
#define COLOR_DOUBLEPORT  QColor(121,189,154);
#define COLOR_MODULESELECTED  QColor(168,219,168);
#define COLOR_MODULE  QColor(207,240,158);*/

/*
#define COLOR_RASTERPORT  QColor(105,210,231)
#define COLOR_VECTORPORT  QColor(243,134,48)
#define COLOR_DOUBLEPORT  Qt::gray
*/

/*#define COLOR_MODULE_SELECTED_BORDER Qt::yellow
#define COLOR_MODULE_FINISHED Qt::green
#define COLOR_MODULE_EXE_ERROR Qt::red
#define COLOR_MODULE_CHECK_ERROR QColor(255,150,0)
#define COLOR_MODULE  Qt::cyan
#define COLOR_DEBUG Qt::gray*/
//#define COLOR_DOUBLEPORT  QColor(250,105,0);

#define COLOR_TAB_BG QColor(255,255,255)	// QColor(239,235,226)



#define COLOR_MODULEBORDER  Qt::black

class ColorSettings
{
	static ColorSettings* instance;

	ColorSettings()
	{
		tabBg = ColorFromSettings("tab_bg_color", COLOR_TAB_BG);
	}

	static QColor ColorFromSettings(const char* name, const QColor& default_color)
	{
		QSettings settings;
		return settings.value(name, default_color).value<QColor>();
	}
	static void SaveSetting(const char* name, const QColor& color)
	{
		QSettings settings;
		settings.setValue(name, color);
	}
public:
	static ColorSettings* getInstance()
	{
		if(!instance)
			instance = new ColorSettings;
		return instance;
	}
	void Reload()
	{
		delete instance;
		instance = new ColorSettings();
	}
	void Save()
	{
		SaveSetting("tab_bg_color", COLOR_TAB_BG);
	}

	QColor tabBg;
};



#endif // COLORPALETTE_H
