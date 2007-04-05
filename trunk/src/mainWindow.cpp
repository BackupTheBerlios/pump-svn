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

/** init static action-pointers */
QAction *PuMP_MainWindow::aboutAction = NULL;
QAction *PuMP_MainWindow::aboutQtAction = NULL;
QAction *PuMP_MainWindow::backwardAction = NULL;
QAction *PuMP_MainWindow::closeAction = NULL;
QAction *PuMP_MainWindow::exitAction = NULL;
QAction *PuMP_MainWindow::forceExitAction = NULL;
QAction *PuMP_MainWindow::forwardAction = NULL;
QAction *PuMP_MainWindow::homeAction = NULL;
QAction *PuMP_MainWindow::mirrorHAction = NULL;
QAction *PuMP_MainWindow::mirrorVAction = NULL;
QAction *PuMP_MainWindow::nextAction = NULL;
QAction *PuMP_MainWindow::openInNewTabAction = NULL;
QAction *PuMP_MainWindow::parentAction = NULL;
QAction *PuMP_MainWindow::previousAction = NULL;
QAction *PuMP_MainWindow::refreshAction = NULL;
QAction *PuMP_MainWindow::rotateCWAction = NULL;
QAction *PuMP_MainWindow::rotateCCWAction = NULL;
QAction *PuMP_MainWindow::sizeOriginalAction = NULL;
QAction *PuMP_MainWindow::sizeFittedAction = NULL;
QAction *PuMP_MainWindow::stopAction = NULL;
QAction *PuMP_MainWindow::zoomInAction = NULL;
QAction *PuMP_MainWindow::zoomOutAction = NULL;

/** init static string-list */
QStringList PuMP_MainWindow::nameFilters;

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

	// setup global actions
	setupActions();
	
	// central-widget and main layout
	QWidget *cWidget = new QWidget(this);
	QHBoxLayout *hBoxLayout = new QHBoxLayout(cWidget);
	setCentralWidget(cWidget);
	
	// treeview for filesystem & imageview for images and overview
	directoryView = new PuMP_DirectoryView(cWidget);
	imageView = new PuMP_ImageView(cWidget);
	hBoxLayout->addWidget(directoryView);
	hBoxLayout->addWidget(imageView);

	// connect their signals
	connect(
		directoryView,
		SIGNAL(openImage(const QFileInfo &, bool)),
		imageView,
		SLOT(on_openImage(const QFileInfo &, bool)));
	connect(
		imageView,
		SIGNAL(updateStatusBar(int, const QString &)),
		this,
		SLOT(on_statusBarUpdate(int, const QString &)));
		
	// toolbar
	toolBar.setParent(this);
	toolBar.setMovable(false);
	toolBar.setAllowedAreas(Qt::TopToolBarArea);
	toolBar.setToolButtonStyle(Qt::ToolButtonIconOnly);
	toolBar.insertAction(NULL, PuMP_MainWindow::backwardAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::forwardAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::parentAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::homeAction);
	toolBar.addSeparator();
	toolBar.insertAction(NULL, PuMP_MainWindow::refreshAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::stopAction);
	toolBar.addSeparator();
	toolBar.insertAction(NULL, PuMP_MainWindow::openInNewTabAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::closeAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::previousAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::nextAction);	
	toolBar.addSeparator();
	toolBar.insertAction(NULL, PuMP_MainWindow::mirrorHAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::mirrorVAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::rotateCCWAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::rotateCWAction);
	toolBar.addSeparator();
	toolBar.insertAction(NULL, PuMP_MainWindow::sizeOriginalAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::sizeFittedAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::zoomInAction);
	toolBar.insertAction(NULL, PuMP_MainWindow::zoomOutAction);
	toolBar.addSeparator();
	addToolBar(&toolBar);

	// setup menubar
	QMenu *menu = NULL;
	menu = menuBar()->addMenu("&File");
	menu->insertAction(NULL, PuMP_MainWindow::openInNewTabAction);
	menu->insertAction(NULL, PuMP_MainWindow::closeAction);
	menu->addSeparator();
	menu->insertAction(NULL, PuMP_MainWindow::forceExitAction);
	menu->insertAction(NULL, PuMP_MainWindow::exitAction);
	menu = menuBar()->addMenu("&Edit");
//	menu->insertAction(NULL, PuMP_MainWindow::exportAction);
//	menu->insertAction(NULL, PuMP_MainWindow::preferenceAction);
	menu = menuBar()->addMenu("&View");
	menu->insertAction(NULL, PuMP_MainWindow::mirrorHAction);
	menu->insertAction(NULL, PuMP_MainWindow::mirrorVAction);
	menu->insertAction(NULL, PuMP_MainWindow::rotateCCWAction);
	menu->insertAction(NULL, PuMP_MainWindow::rotateCWAction);
	menu->addSeparator();
	menu->insertAction(NULL, PuMP_MainWindow::sizeOriginalAction);
	menu->insertAction(NULL, PuMP_MainWindow::sizeFittedAction);
	menu->insertAction(NULL, PuMP_MainWindow::zoomInAction);
	menu->insertAction(NULL, PuMP_MainWindow::zoomOutAction);
	menu->addSeparator();
	menu->insertAction(NULL, PuMP_MainWindow::refreshAction);
	menu->insertAction(NULL, PuMP_MainWindow::stopAction);
	menu = menuBar()->addMenu("&Go to");
	menu->insertAction(NULL, PuMP_MainWindow::backwardAction);
	menu->insertAction(NULL, PuMP_MainWindow::forwardAction);
	menu->insertAction(NULL, PuMP_MainWindow::parentAction);
	menu->insertAction(NULL, PuMP_MainWindow::homeAction);
	menu->addSeparator();
	menu->insertAction(NULL, PuMP_MainWindow::previousAction);
	menu->insertAction(NULL, PuMP_MainWindow::nextAction);	
	menu = menuBar()->addMenu("&Help");
	menu->insertAction(NULL, PuMP_MainWindow::aboutQtAction);
	menu->insertAction(NULL, PuMP_MainWindow::aboutAction);

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
	//resize(QSize(640, 480));
	adjustSize();
	
	PuMP_MainWindow::homeAction->trigger();	
}

/**
 * Simple destructor of class PuMP_MainWindow.
 */
PuMP_MainWindow::~PuMP_MainWindow()
{
	delete directoryView;
	delete imageView;

	delete PuMP_MainWindow::aboutAction;
	delete PuMP_MainWindow::aboutQtAction;
	delete PuMP_MainWindow::backwardAction;
	delete PuMP_MainWindow::closeAction;
	delete PuMP_MainWindow::exitAction;
	delete PuMP_MainWindow::forceExitAction;
	delete PuMP_MainWindow::forwardAction;
	delete PuMP_MainWindow::homeAction;
	delete PuMP_MainWindow::mirrorHAction;
	delete PuMP_MainWindow::mirrorVAction;
	delete PuMP_MainWindow::nextAction;
	delete PuMP_MainWindow::openInNewTabAction;
	delete PuMP_MainWindow::parentAction;
	delete PuMP_MainWindow::previousAction;
	delete PuMP_MainWindow::refreshAction;
	delete PuMP_MainWindow::rotateCWAction;
	delete PuMP_MainWindow::rotateCCWAction;
	delete PuMP_MainWindow::sizeOriginalAction;
	delete PuMP_MainWindow::sizeFittedAction;
	delete PuMP_MainWindow::stopAction;
	delete PuMP_MainWindow::zoomInAction;
	delete PuMP_MainWindow::zoomOutAction;
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
		PuMP_MainWindow::nameFilters <<
			QString(formats[index].prepend(prefix).data());
	
	qDebug() << "supported formats:";
	for(index = 0; index < PuMP_MainWindow::nameFilters.size(); index++)
		qDebug() << PuMP_MainWindow::nameFilters[index]; 
}

/**
 * Function that creates all global used actions and gives them to the
 * appropriate child-widgets (to connect to them).
 */
void PuMP_MainWindow::setupActions()
{
	PuMP_MainWindow::aboutAction = new QAction(/*QIcon(), */"About", this);
	PuMP_MainWindow::aboutAction->setToolTip("Show information about PuMP.");
	connect(
		PuMP_MainWindow::aboutAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_about()));
	
	PuMP_MainWindow::aboutQtAction = new QAction("About Qt", this);
	PuMP_MainWindow::aboutQtAction->setToolTip("Show information about Qt.");
	connect(
		PuMP_MainWindow::aboutQtAction,
		SIGNAL(triggered()),
		qApp,
		SLOT(aboutQt()));

	PuMP_MainWindow::backwardAction = new QAction(
		QIcon(":/back.png"),
		"Last directory",
		this);
	PuMP_MainWindow::backwardAction->setToolTip("Got to the last directory.");
	PuMP_MainWindow::backwardAction->setEnabled(false);
	
	PuMP_MainWindow::closeAction = new QAction(
		QIcon(":/tab_remove.png"),
		"Close tab",
		this);
	PuMP_MainWindow::closeAction->setToolTip("Close the current tab.");
	PuMP_MainWindow::closeAction->setEnabled(false);

	PuMP_MainWindow::exitAction = new QAction(
		QIcon(":/exit.png"),
		"Exit",
		this);
	PuMP_MainWindow::exitAction->setToolTip("Close and exit PuMP.");
	connect(
		PuMP_MainWindow::exitAction,
		SIGNAL(triggered()),
		this,
		SLOT(close()));
	
	PuMP_MainWindow::forceExitAction = new QAction("Quit", this);
	PuMP_MainWindow::forceExitAction->setToolTip("Exit PuMP ungracefully.");
	connect(
		PuMP_MainWindow::forceExitAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_forceExit()));

	PuMP_MainWindow::forwardAction = new QAction(
		QIcon(":/forward.png"),
		"Next directory",
		this);
	PuMP_MainWindow::forwardAction->setToolTip("Got to the next directory.");
	PuMP_MainWindow::forwardAction->setEnabled(false);

	PuMP_MainWindow::homeAction = new QAction(
		QIcon(":/gohome.png"),
		"Home",
		this);
	PuMP_MainWindow::homeAction->setToolTip("Got to the home directory");
	PuMP_MainWindow::homeAction->setData(QDir::homePath());

	PuMP_MainWindow::mirrorHAction = new QAction(
		QIcon(":/hmirror.png"),
		"Mirror image horizontally",
		this);
	PuMP_MainWindow::mirrorHAction->setToolTip("Mirror the current image " \
		"horizontally.");
	PuMP_MainWindow::mirrorHAction->setEnabled(false);

	PuMP_MainWindow::mirrorVAction = new QAction(
		QIcon(":/vmirror.png"),
		"Mirror image vertically",
		this);
	PuMP_MainWindow::mirrorVAction->setToolTip("Mirror the current image " \
		"vertically.");
	PuMP_MainWindow::mirrorVAction->setEnabled(false);

	PuMP_MainWindow::nextAction = new QAction(
		QIcon(":/next.png"),
		"Next Image",
		this);
	PuMP_MainWindow::nextAction->setToolTip("Got to the next image.");
	PuMP_MainWindow::nextAction->setEnabled(false);

	PuMP_MainWindow::openInNewTabAction = new QAction(
		QIcon(":/tab_new.png"),
		"Open in new tab",
		this);
	PuMP_MainWindow::openInNewTabAction->setToolTip(
		"Open selected image in a new tab.");
	PuMP_MainWindow::openInNewTabAction->setEnabled(false);
	
	PuMP_MainWindow::parentAction = new QAction(
		QIcon(":/up.png"),
		"Parent directory",
		this);
	PuMP_MainWindow::parentAction->setToolTip("Got to the parent directory.");
	PuMP_MainWindow::parentAction->setEnabled(false);

	PuMP_MainWindow::previousAction = new QAction(
		QIcon(":/previous.png"),
		"Previous image",
		this);
	PuMP_MainWindow::previousAction->setToolTip("Go to the previous image.");
	PuMP_MainWindow::previousAction->setEnabled(false);

	PuMP_MainWindow::refreshAction = new QAction(
		QIcon(":/reload.png"),
		"Refresh",
		this);
	PuMP_MainWindow::refreshAction->setToolTip("Refresh the current " \
		"directory.");

	PuMP_MainWindow::rotateCCWAction = new QAction(
		QIcon(":/rotate_ccw.png"),
		"Rotate image counter-clockwise",
		this);
	PuMP_MainWindow::rotateCCWAction->setToolTip("Rotate the current image " \
		"counter-clockwise.");
	PuMP_MainWindow::rotateCCWAction->setEnabled(false);

	PuMP_MainWindow::rotateCWAction = new QAction(
		QIcon(":/rotate_cw.png"),
		"Rotate image clockwise",
		this);
	PuMP_MainWindow::rotateCWAction->setToolTip("Rotate the current image " \
		"clockwise.");
	PuMP_MainWindow::rotateCWAction->setEnabled(false);

	PuMP_MainWindow::sizeOriginalAction = new QAction(
		QIcon(":/viewmag1.png"),
		"Display image in original size",
		this);
	PuMP_MainWindow::sizeOriginalAction->setToolTip("Display the current " \
		"image in original size.");
	PuMP_MainWindow::sizeOriginalAction->setEnabled(false);

	PuMP_MainWindow::sizeFittedAction = new QAction(
		QIcon(":/viewmagfit.png"),
		"Display image fitted to window",
		this);
	PuMP_MainWindow::sizeFittedAction->setToolTip("Display the current " \
		"image fitted to window.");
	PuMP_MainWindow::sizeFittedAction->setEnabled(false);

	PuMP_MainWindow::stopAction = new QAction(
		QIcon(":/stop.png"),
		"Stop refresh",
		this);
	PuMP_MainWindow::stopAction->setToolTip("Stop refreshing the current " \
		"directory.");
	PuMP_MainWindow::stopAction->setEnabled(false);

	PuMP_MainWindow::zoomInAction = new QAction(
		QIcon(":/viewmag+.png"),
		"Zoom in",
		this);
	PuMP_MainWindow::zoomInAction->setToolTip("Zoom in current image.");
	PuMP_MainWindow::zoomInAction->setEnabled(false);

	PuMP_MainWindow::zoomOutAction = new QAction(
		QIcon(":/viewmag-.png"),
		"Zoom out",
		this);
	PuMP_MainWindow::zoomOutAction->setToolTip("Zoom out current image.");
	PuMP_MainWindow::zoomOutAction->setEnabled(false);
}

/**
 * Slot-function that shows PuMP's about-dialog.
 */
void PuMP_MainWindow::on_about()
{
	
}

/**
 * Slot-function that forces PuMP to exit immediatelly.
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
