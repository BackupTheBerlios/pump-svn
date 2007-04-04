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

#include <assert.h>

#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>

#include "directoryView.hh"
#include "mainWindow.hh"
#include "overview.hh"

/*****************************************************************************/

/** init static action pointers */
QAction *PuMP_DirectoryView::openAction = NULL;
QAction *PuMP_DirectoryView::openInNewTabAction = NULL;

/**
 * Constructor of class PuMP_DirectoryView that mainly provides the
 * functionality of a directory-tree-view ccontaining its context-menus.
 * @param	nameFilters	A string-list containing the extensions to display.
 * @param	parent		The parent-widget to this view. 
 */
PuMP_DirectoryView::PuMP_DirectoryView(
	const QStringList &nameFilters,
	QWidget *parent)
	: QTreeView(parent)
{
	model.setFilter(
		QDir::AllDirs |
		QDir::Files |
		QDir::Drives |
		QDir::NoDotAndDotDot |
		QDir::Readable);
	model.setNameFilters(nameFilters);
	model.setParent(this);
	model.setSorting(QDir::Name | QDir::DirsFirst);
	
	setColumnWidth(0, 130);
	setMaximumWidth(200);
	setMinimumWidth(140);
	setModel(&model);

	PuMP_DirectoryView::openAction = new QAction("Open", this);
	connect(
		PuMP_DirectoryView::openAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_openAction_triggered()));
	PuMP_DirectoryView::openInNewTabAction = new QAction(
		QIcon(":/tab_new.png"),
		"Open in new tab",
		this);
	connect(
		PuMP_DirectoryView::openInNewTabAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_openInNewTabAction_triggered()));
	connect(
		PuMP_MainWindow::refreshAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_refreshAction_triggered()));

	connect(
		this,
		SIGNAL(activated(const QModelIndex &)),
		this,
		SLOT(on_activated(const QModelIndex &)));
	connect(
		this,
		SIGNAL(clicked(const QModelIndex &)),
		this,
		SLOT(on_clicked(const QModelIndex &)));
}

/**
 * Destructor of class PuMP_DirectoryView that just frees all objects.
 */
PuMP_DirectoryView::~PuMP_DirectoryView()
{
	delete PuMP_DirectoryView::openAction;
	delete PuMP_DirectoryView::openInNewTabAction;
}

/**
 * Overloaded function for context-menu-events. It provides a custom menu for
 * directories and files.
 * @param	e The context-menu-event. 
 */
void PuMP_DirectoryView::contextMenuEvent(QContextMenuEvent *e)
{
	QModelIndex index = currentIndex();
	if(!index.isValid()) e->ignore();
	
	bool enable = true;
	QFileInfo info = model.fileInfo(index);
	if(info.isDir()) enable = false;

	PuMP_DirectoryView::openAction->setEnabled(enable);
	PuMP_DirectoryView::openInNewTabAction->setEnabled(enable);
	PuMP_MainWindow::refreshAction->setEnabled(!enable);

	QMenu menu(this);
	menu.addAction(PuMP_DirectoryView::openAction);
	menu.addAction(PuMP_DirectoryView::openInNewTabAction);
	menu.addSeparator();
	menu.addAction(PuMP_MainWindow::refreshAction);
	menu.addAction(PuMP_MainWindow::stopAction);
	menu.exec(e->globalPos());
}

/**
 * Slot-function that is called when an entry is activated.
 * @param	index	The index of the corresponding entry.
 * @param	newTab	Indicates whether the file should be opened in a new tab.
 */
void PuMP_DirectoryView::on_activated(const QModelIndex &index, bool newTab)
{
	if(!index.isValid()) return;

	QFileInfo info = model.fileInfo(index);
	if(!info.isDir()) emit openImage(info, newTab);
}

/**
 * Slot-function that is called when an entry was clicked.
 * @param	index	The index of the corresponding entry.
 */
void PuMP_DirectoryView::on_clicked(const QModelIndex &index)
{
	setCurrentIndex(index);
	QFileInfo info = model.fileInfo(index);
	if(info.isDir())
	{
		PuMP_Overview::openAction->setData(info.filePath());
		PuMP_Overview::openAction->trigger();
	}
}

/**
 * Slot-function that is called when the open-action was triggered.
 */
void PuMP_DirectoryView::on_openAction_triggered()
{
	QVariant v = PuMP_DirectoryView::openAction->data();
	if(v.isValid())
	{
		QFileInfo info(v.toString());
		PuMP_DirectoryView::openAction->setData(QVariant());
		QModelIndex index = model.index(info.filePath());
		if(index.isValid() && info.exists() && info.isDir())
			setCurrentIndex(index);
	}
	else
	{
		QModelIndex index = currentIndex();
		on_activated(index, false);
	}
}

/**
 * Slot-function that is called when the open-in-a-new-tab-action was triggered.
 */
void PuMP_DirectoryView::on_openInNewTabAction_triggered()
{
	QModelIndex index = currentIndex();
	on_activated(index, true);
}

/**
 * Slot-function that is called when the refresh-action was triggered.
 */
void PuMP_DirectoryView::on_refreshAction_triggered()
{
	QModelIndex index = currentIndex();
	if(index.isValid())
	{
		QFileInfo info = model.fileInfo(index);
		if(info.isDir())
		{
			PuMP_Overview::openAction->setData(info.filePath());
			PuMP_Overview::openAction->trigger();
		}
	
		model.refresh(index);
	}
}


/*****************************************************************************/
