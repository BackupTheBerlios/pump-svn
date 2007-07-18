/*
 * Copyright 2007 Christoph Werle, Tobias Schlager
 * 
 * This file is part of "PuMP - Publish My Pictures".
 *
 * "Publish My Pictures" is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or any later version.
 *
 * "Publish My Pictures" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Publish My Pictures"; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifndef SETTINGS_HH_
#define SETTINGS_HH_

#include <QSettings>

/******************************************************************************/

#define NATIVE_SETTINGS_FORMAT	QSettings::NativeFormat
#define INI_SETTINGS_FORMAT		QSettings::IniFormat

/******************************************************************************/

class PuMP_SettingsInterface
{
	public:
		PuMP_SettingsInterface();
		virtual ~PuMP_SettingsInterface();
		
		virtual void loadSettings() = 0;
		virtual void storeSettings() = 0;

};

/******************************************************************************/

#endif /*SETTINGS_HH_*/
