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




#define COLOR_TAB_BG QColor(255,255,255)	// QColor(239,235,226)

#define COLOR_MOD_UNTOUCHED QColor(244,141,2)
#define COLOR_MOD_EXECUTING QColor(142,174,255)
#define COLOR_MOD_SPILTTER QColor(204,204,0)
#define COLOR_MOD_EXECUTION_OK QColor(191,213,154)
#define COLOR_MOD_EXECUTION_ERROR QColor(244,141,2)
#define COLOR_MOD_CHECK_OK QColor(200,200,200)
#define COLOR_MOD_BG_COLOR QColor(255,255,255)
#define COLOR_MOD_CHECK_ERROR QColor(244,141,2)
#define COLOR_MODLE_TEXT_COLOR QColor(0,0,0)
#define COLOR_MODULE_COLOR QColor(244,141,2)
#define COLOR_MODULE_COLOR_ERROR QColor(255,0,0)
#define COLOR_MODULEBORDER QColor(0,0,0)

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
