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

#ifndef CONFIGDIALOG_HH_
#define CONFIGDIALOG_HH_

#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "settings.hh"
#include "configPages.hh"
#include "exportDialog.hh"

/******************************************************************************/

class PuMP_ConfigDialog : public QDialog, public PuMP_SettingsInterface
{
	Q_OBJECT

	protected:
		QHBoxLayout *hboxLayout1;
		QHBoxLayout *hboxLayout2;
		QVBoxLayout *vboxLayout;
		QListWidget *configWidget;
		QListWidgetItem *basConfig;
		QListWidgetItem *expConfig;
		QListWidgetItem *advConfig;
		QListWidgetItem *about;
		QPushButton *okButton;
		QPushButton *applyButton;
		QPushButton *cancelButton;
		QPushButton *resetButton;
		QStackedWidget *pagesWidget;
		
		PuMP_BasicConfigWidget *basicWidget;
		PuMP_ExportWidget *exportWidget;
		PuMP_AboutWidget *aboutPuMP;
		PuMP_AboutQtWidget *aboutQt;

		void createIcons();

	public:
		PuMP_ConfigDialog(QWidget *parent = 0);
		~PuMP_ConfigDialog();

		void loadSettings();
		void storeSettings();
	
	public slots:
		void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	
};

/******************************************************************************/

#endif /*CONFIGDIALOG_HH_*/
