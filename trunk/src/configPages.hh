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

#ifndef CONFIGPAGES_HH_
#define CONFIGPAGES_HH_

#include "settings.hh"

#include <QHBoxLayout>
#include <QLabel>

/******************************************************************************/

class PuMP_BasicConfigWidget : public QWidget, public PuMP_SettingsInterface
{
	Q_OBJECT

	public:
		PuMP_BasicConfigWidget(QWidget *parent = 0);
		~PuMP_BasicConfigWidget();
		
		void loadSettings();
		void storeSettings();
	
};

/******************************************************************************/

class PuMP_AboutWidget : public QWidget
{
	Q_OBJECT

	protected:
		QHBoxLayout *hboxLayout;
		QLabel *leftLabel;
		QLabel *rightLabel;

	public:
		PuMP_AboutWidget(QWidget *parent = 0);
		~PuMP_AboutWidget();
	
};

/******************************************************************************/

class PuMP_AboutQtWidget : public QWidget
{
	Q_OBJECT

	protected:
		QHBoxLayout *hboxLayout;
		QLabel *leftLabel;
		QLabel *rightLabel;

	public:
		PuMP_AboutQtWidget(QWidget *parent = 0);
		~PuMP_AboutQtWidget();

};

/******************************************************************************/

#endif /*CONFIGPAGES_HH_*/
