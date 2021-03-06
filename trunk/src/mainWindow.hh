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

#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_

#include <QAction>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QStringList>
#include <QToolBar>

#include "settings.hh"

/******************************************************************************/

class PuMP_DirectoryView;
class PuMP_TabView;

/******************************************************************************/

#define PUMP_MAINWINDOW_SIZE	"PuMP_MainWindow::size"
#define PUMP_MAINWINDOW_POS		"PuMP_MainWindow::pos"

/******************************************************************************/

class PuMP_MainWindow : public QMainWindow, public PuMP_SettingsInterface
{
	Q_OBJECT
	
	protected:
		PuMP_DirectoryView *directoryView;
		PuMP_TabView *tabView;
		
		QLabel progressBarLabel;
		QProgressBar progressBar;
		QToolBar toolBar;
		
		void getSupportedImageFormats();
		void setupActions();

	public:
		static QAction *aboutAction;
		static QAction *aboutQtAction;
		static QAction *backwardAction;
		static QAction *closeAction;
		static QAction *exitAction;
		static QAction *exportAction;
		static QAction *forceExitAction;
		static QAction *forwardAction;
		static QAction *homeAction;
		static QAction *mirrorHAction;
		static QAction *mirrorVAction;
		static QAction *nextAction;
		static QAction *openInNewTabAction;
		static QAction *parentAction;
		static QAction *previousAction;
		static QAction *refreshAction;
		static QAction *rotateCWAction;
		static QAction *rotateCCWAction;
		static QAction *saveAction;
		static QAction *saveAsAction;
		static QAction *sizeOriginalAction;
		static QAction *sizeFittedAction;
		static QAction *stopAction;
		static QAction *zoomInAction;
		static QAction *zoomOutAction;
		
		static QSettings *settings;
		
		static QStringList nameFilters;
		static QString nameFilterString1;
		static QString nameFilterString2;

		PuMP_MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
		~PuMP_MainWindow();

		void loadSettings();
		void storeSettings();

	public slots:
		void on_about();
		void on_aboutQt();
		void on_exportAction();
		void on_forceExit();
		void on_statusBarUpdate(int value, const QString &text);
};

/******************************************************************************/

#endif /*MAINWINDOW_HH_*/
