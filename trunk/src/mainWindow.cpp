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

#include "configDialog.hh"
#include "directoryView.hh"
#include "imageView.hh"
#include "mainWindow.hh"

#include <assert.h>

#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QImageWriter>
#include <QList>
#include <QMenuBar>
#include <QStatusBar>
#include <QString>
#include <QToolBar>

/*****************************************************************************/

/**
 * Constructor of class PuMP_MainWindow that allocates all needed components,
 * sets the layout and other mainwindow-properties and makes all
 * slot-connections between the created components.
 * @param	parent	This widgets parent (usually NULL).
 * @param	flags	Qt-specific window-flags (usually 0).
 */
PuMP_MainWindow::PuMP_MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	// find out which image-formats are supported
	getSupportedImageFormats();
	
	// central-widget and main layout
	QWidget *cWidget = new QWidget(this);
	QHBoxLayout *hBoxLayout = new QHBoxLayout(cWidget);
	setCentralWidget(cWidget);
	
	// treeview for filesystem & imageview for images and overview
	directoryView = new PuMP_DirectoryView(nameFilters, cWidget);
	imageView = new PuMP_ImageView(nameFilters, cWidget);
	hBoxLayout->addWidget(directoryView);
	hBoxLayout->addWidget(imageView);

	// connect both
	connect(
		directoryView,
		SIGNAL(openDir(const QFileInfo &)),
		imageView,
		SLOT(on_openDir(const QFileInfo &)));
	connect(
		directoryView,
		SIGNAL(openImage(const QFileInfo &, bool)),
		imageView,
		SLOT(on_openImage(const QFileInfo &, bool)));
	connect(
		imageView,
		SIGNAL(dirOpened(const QFileInfo &)),
		directoryView,
		SLOT(on_dirOpened(const QFileInfo &)));	
	connect(
		imageView,
		SIGNAL(updateStatusBar(int, const QString &)),
		this,
		SLOT(on_statusBarUpdate(int, const QString &)));
		
	// setup global actions
	setupActions();
	
	// toolbar
	toolBar.setParent(this);
	toolBar.setMovable(false);
	toolBar.setAllowedAreas(Qt::TopToolBarArea);
	toolBar.setToolButtonStyle(Qt::ToolButtonIconOnly);
	toolBar.insertAction(NULL, homeAction);
	toolBar.insertAction(NULL, previousAction);
	toolBar.insertAction(NULL, nextAction);
	toolBar.addSeparator();
	toolBar.insertAction(NULL, refreshAction);
	toolBar.insertAction(NULL, stopAction);
	toolBar.addSeparator();
	toolBar.insertAction(NULL, addAction);
	toolBar.insertAction(NULL, closeAction);
	toolBar.addSeparator();
	toolBar.insertAction(NULL, mirrorHAction);
	toolBar.insertAction(NULL, mirrorVAction);
	toolBar.insertAction(NULL, rotateCCWAction);
	toolBar.insertAction(NULL, rotateCWAction);
	toolBar.addSeparator();
	toolBar.insertAction(NULL, sizeOriginalAction);
	toolBar.insertAction(NULL, sizeFittedAction);
	toolBar.insertAction(NULL, zoomInAction);
	toolBar.insertAction(NULL, zoomOutAction);
	toolBar.addSeparator();
	addToolBar(&toolBar);

	// setup menubar
	QMenu *menu = NULL;
	menu = menuBar()->addMenu("&File");
	menu->insertAction(NULL, addAction);
	menu->insertAction(NULL, closeAction);
	menu->addSeparator();
	menu->insertAction(NULL, forceExitAction);
	menu->insertAction(NULL, exitAction);
	menu = menuBar()->addMenu("&Edit");
//	menu->insertAction(NULL, exportAction);
//	menu->insertAction(NULL, preferenceAction);
	menu = menuBar()->addMenu("&View");
	menu->insertAction(NULL, mirrorHAction);
	menu->insertAction(NULL, mirrorVAction);
	menu->insertAction(NULL, rotateCCWAction);
	menu->insertAction(NULL, rotateCWAction);
	menu->addSeparator();
	menu->insertAction(NULL, sizeOriginalAction);
	menu->insertAction(NULL, sizeFittedAction);
	menu->insertAction(NULL, zoomInAction);
	menu->insertAction(NULL, zoomOutAction);
	menu->addSeparator();
	menu->insertAction(NULL, refreshAction);
	menu->insertAction(NULL, stopAction);
	menu = menuBar()->addMenu("&Go");
	menu->insertAction(NULL, previousAction);
	menu->insertAction(NULL, nextAction);
	menu->insertAction(NULL, homeAction);
	menu = menuBar()->addMenu("&Help");
	menu->insertAction(NULL, aboutQtAction);
	menu->insertAction(NULL, aboutAction);

	// setup statusbar
	progressBar.setMinimum(0);
	progressBar.setMaximum(100);
	progressBarLabel.setMinimumWidth(100);
	QStatusBar *bar = statusBar();
	bar->addWidget(&progressBar);
	bar->addWidget(&progressBarLabel);
	QString text("");
	on_statusBarUpdate(100, text);
	
	// main window
	/*setWindowIcon(:/PuMP32.png);*/
	setWindowTitle("PuMP - Publish My Pictures");
	resize(QSize(640, 480));
	
//	// test
//	configDialog dialog(this);
//	dialog.exec();
}

/**
 * Simple destructor of class PuMP_MainWindow.
 */
PuMP_MainWindow::~PuMP_MainWindow()
{
	delete directoryView;
	delete imageView;

	delete aboutAction;
	delete aboutQtAction;
	delete addAction;
	delete closeAction;
	delete exitAction;
	delete forceExitAction;
	delete homeAction;
	delete mirrorHAction;
	delete mirrorVAction;
	delete nextAction;
	delete previousAction;
	delete refreshAction;
	delete rotateCWAction;
	delete rotateCCWAction;
	delete sizeOriginalAction;
	delete sizeFittedAction;
	delete stopAction;
	delete zoomInAction;
	delete zoomOutAction;
}

/**
 * Small helper-function that fills the nameFilter-variable with the supported
 * file types (png, bmp, ...).
 */
void PuMP_MainWindow::getSupportedImageFormats()
{
	QList<QByteArray> formats = QImageWriter::supportedImageFormats();
	QByteArray prefix("*.");
	QByteArray filter("");
	
	int index;
	for(index = 0; index < formats.size(); index++)
		nameFilters << QString(formats[index].prepend(prefix).data());
	
	qDebug() << "supported formats:";
	for(index = 0; index < nameFilters.size(); index++)
		qDebug() << nameFilters[index]; 
}

/**
 * Function that creates all global used actions and gives them to the
 * appropriate child-widgets (to connect to them).
 */
void PuMP_MainWindow::setupActions()
{
	aboutAction = new QAction(/*QIcon(), */"About", this);
	aboutAction->setToolTip("Show information about PuMP.");
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(on_about()));
	
	aboutQtAction = new QAction(/*QIcon(), */"About Qt", this);
	aboutAction->setToolTip("Show information about Qt.");
	connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	addAction = new QAction(QIcon(":/tab_new.png"), "Add new tab", this);
	addAction->setToolTip("Add a new tab.");
	
	closeAction = new QAction(QIcon(":/tab_remove.png"), "Close tab", this);
	closeAction->setToolTip("Close the current tab.");
	closeAction->setEnabled(false);

	exitAction = new QAction("Exit", this);
	exitAction->setToolTip("Close and exit PuMP.");
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
	
	forceExitAction = new QAction("Quit", this);
	forceExitAction->setToolTip("Exit PuMP ungracefully.");
	connect(forceExitAction, SIGNAL(triggered()), this, SLOT(on_forceExit()));

	homeAction = new QAction(QIcon(":/gohome.png"), "Home", this);
	homeAction->setToolTip("Got to the home directory");

	mirrorHAction = new QAction(
		QIcon(":/hmirror.png"),
		"Mirror image horizontally",
		this);
	mirrorHAction->setToolTip("Mirror the current image horizontally.");
	mirrorHAction->setEnabled(false);

	mirrorVAction = new QAction(
		QIcon(":/vmirror.png"),
		"Mirror image vertically",
		this);
	mirrorVAction->setToolTip("Mirror the current image vertically.");
	mirrorVAction->setEnabled(false);

	nextAction = new QAction(QIcon(":/forward.png"), "Next directory", this);
	nextAction->setToolTip("Got to the next directory.");

	previousAction = new QAction(
		QIcon(":/back.png"),
		"Previous directory",
		this);
	previousAction->setToolTip("Go to the previous directory.");

	refreshAction = new QAction(QIcon(":/reload.png"), "Refresh", this);
	refreshAction->setToolTip("Refresh the current directory.");

	rotateCCWAction = new QAction(
		QIcon(":/rotate_ccw.png"),
		"Rotate image counter-clockwise",
		this);
	rotateCCWAction->setToolTip("Rotate the current image counter-clockwise.");
	rotateCCWAction->setEnabled(false);

	rotateCWAction = new QAction(
		QIcon(":/rotate_cw.png"),
		"Rotate image clockwise",
		this);
	rotateCWAction->setToolTip("Rotate the current image clockwise.");
	rotateCWAction->setEnabled(false);

	sizeOriginalAction = new QAction(
		QIcon(":/viewmag1.png"),
		"Display image in original size",
		this);
	sizeOriginalAction->setToolTip("Display the current image in original size.");
	sizeOriginalAction->setEnabled(false);

	sizeFittedAction = new QAction(
		QIcon(":/viewmagfit.png"),
		"Display image fitted to window",
		this);
	sizeFittedAction->setToolTip("Display the current image fitted to window.");
	sizeFittedAction->setEnabled(false);

	stopAction = new QAction(QIcon(":/stop.png"), "Stop refresh", this);
	stopAction->setToolTip("Stop refreshing the current directory.");
	stopAction->setEnabled(false);

	zoomInAction = new QAction(QIcon(":/viewmag+.png"), "Zoom in", this);
	zoomInAction->setToolTip("Zoom in current image.");
	zoomInAction->setEnabled(false);

	zoomOutAction = new QAction(QIcon(":/viewmag-.png"), "Zoom out", this);
	zoomOutAction->setToolTip("Zoom out current image.");
	zoomOutAction->setEnabled(false);
	
	assert(directoryView != NULL);
	assert(imageView != NULL);
	directoryView->setupActions(refreshAction, stopAction);
	imageView->setupActions(
		addAction,
		closeAction,
		mirrorHAction,
		mirrorVAction,
		refreshAction,
		rotateCWAction,
		rotateCCWAction,
		sizeOriginalAction,
		sizeFittedAction,
		stopAction,
		zoomInAction,
		zoomOutAction);
}

/**
 * Function that shows PuMP's about-dialog.
 */
void PuMP_MainWindow::on_about()
{
	
}

/**
 * Function that forces PuMP to exit immediatelly.
 */
void PuMP_MainWindow::on_forceExit()
{
	exit(0);
}

/**
 * Slot-function that enables other widgets to print status-messages on the
 * status-bar. Note: value=0 will make the components visible while value=100
 * will hide them again.
 * @param	value	The value for the progress-bar.
 * @param	text	The text to display.
 */
void PuMP_MainWindow::on_statusBarUpdate(int value, const QString &text)
{
	progressBarLabel.setText(text);
	progressBarLabel.repaint();
	progressBar.setValue(value);

	bool enabled = true;
	if(value == 0) enabled = true;
	if(value == 100) enabled = false;

	progressBarLabel.setVisible(enabled);
	progressBar.setVisible(enabled);
}

/*****************************************************************************/
