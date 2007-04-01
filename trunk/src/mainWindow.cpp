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

#include "directoryView.hh"
#include "imageView.hh"
#include "mainWindow.hh"

#include <QByteArray>
#include <QDebug>
#include <QHBoxLayout>
#include <QImageWriter>
#include <QList>
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
		SIGNAL(overviewRequested(const QFileInfo &)),
		imageView,
		SLOT(createOverview(const QFileInfo &)));
	connect(
		directoryView,
		SIGNAL(refreshRequested()),
		imageView,
		SLOT(refreshOverview()));
	connect(
		directoryView,
		SIGNAL(viewerRequested(const QFileInfo &, bool)),
		imageView,
		SLOT(display(const QFileInfo &, bool)));	
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

	QAction *action = toolBar.addAction(
		QIcon(":/reload.png"),
		"Reload directory");
	action->setToolTip("Reload the current directory.");

	action = toolBar.addAction(QIcon(":/add.png"), "Add new tab");
	action->setToolTip("Add a new tab.");
	
	action = toolBar.addAction(QIcon(":/remove.png"), "Remove tab");
	action->setToolTip("Remove the current tab.");
	
	toolBar.addSeparator();

	action = toolBar.addAction(
		QIcon(":/hmirror.png"),
		"Mirror image horizontally");
	action->setToolTip("Mirror the current image horizontally.");

	action = toolBar.addAction(
		QIcon(":/vmirror.png"),
		"Mirror image vertically");
	action->setToolTip("Mirror the current image vertically.");

	action = toolBar.addAction(
		QIcon(":/rotate_ccw.png"),
		"Rotate image counter-clockwise");
	action->setToolTip("Rotate the current image counter-clockwise.");

	action = toolBar.addAction(
		QIcon(":/rotate_cw.png"),
		"Rotate image clockwise");
	action->setToolTip("Rotate the current image clockwise.");

	toolBar.addSeparator();

	action = toolBar.addAction(
		QIcon(":/viewmag1.png"),
		"Display image in original size");
	action->setToolTip("Display the current image in original size.");

	action = toolBar.addAction(
		QIcon(":/viewmagfit.png"),
		"Display image fitted to window");
	action->setToolTip("Display the current image fitted to window.");

	action = toolBar.addAction(QIcon(":/viewmag+.png"), "Zoom in");
	action->setToolTip("Zoom in current image.");

	action = toolBar.addAction(QIcon(":/viewmag-.png"), "Zoom out");
	action->setToolTip("Zoom out current image.");

	toolBar.addSeparator();

	addToolBar(&toolBar);

	// setup menubar

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
}

/**
 * Simple destructor of class PuMP_MainWindow.
 */
PuMP_MainWindow::~PuMP_MainWindow()
{
	delete directoryView;
	delete imageView;
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
