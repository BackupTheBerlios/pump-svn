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

#include "settings.hh"
#include "mainWindow.hh"

#include <assert.h>

/******************************************************************************/

PuMP_SettingsInterface::PuMP_SettingsInterface()
{
	if(PuMP_MainWindow::settings == NULL)
	{
		PuMP_MainWindow::settings = new QSettings(
			NATIVE_SETTINGS_FORMAT,
			QSettings::UserScope,
			"PublishMyPictures",
			"PuMP");
	}
}

PuMP_SettingsInterface::~PuMP_SettingsInterface()
{
	assert(PuMP_MainWindow::settings != NULL);
	PuMP_MainWindow::settings->sync();
}

/******************************************************************************/
