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

/*****************************************************************************/

class PuMP_DirectoryView;
class PuMP_ImageView;

/*****************************************************************************/

class PuMP_MainWindow : public QMainWindow
{
	Q_OBJECT
	
	protected:
		PuMP_DirectoryView *directoryView;
		PuMP_ImageView *imageView;
		
		QAction *aboutAction;
		QAction *aboutQtAction;
		QAction *addAction;
		QAction *closeAction;
		QAction *exitAction;
		QAction *forceExitAction;
		QAction *homeAction;
		QAction *mirrorHAction;
		QAction *mirrorVAction;
		QAction *nextAction;
		QAction *previousAction;
		QAction *refreshAction;
		QAction *rotateCWAction;
		QAction *rotateCCWAction;
		QAction *sizeOriginalAction;
		QAction *sizeFittedAction;
		QAction *stopAction;
		QAction *zoomInAction;
		QAction *zoomOutAction;

		QLabel progressBarLabel;
		QProgressBar progressBar;
		QStringList nameFilters;
		QToolBar toolBar;
		
		void getSupportedImageFormats();
		void setupActions();

	public:
		PuMP_MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
		~PuMP_MainWindow();

	public slots:
		void on_about();
		void on_forceExit();
		void on_statusBarUpdate(int value, const QString &text);
};

/*****************************************************************************/

#endif /*MAINWINDOW_HH_*/
