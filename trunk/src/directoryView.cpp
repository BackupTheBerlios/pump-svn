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

#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>

/*****************************************************************************/

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
	openAction = new QAction("Open", this);
	connect(
		openAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_openAction_triggered()));
	openInNewTabAction = new QAction("Open in new tab", this);
	connect(
		openInNewTabAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_openInNewTabAction_triggered()));
	refreshAction = new QAction("Refresh", this);
	connect(
		refreshAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_refreshAction_triggered()));

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
	delete openAction;
	delete openInNewTabAction;
	delete refreshAction;
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
	
	openAction->setEnabled(enable);
	openInNewTabAction->setEnabled(enable);
	refreshAction->setEnabled(!enable);

	QMenu menu(this);
	menu.addAction(refreshAction);
	menu.addSeparator();
	menu.addAction(openAction);
	menu.addAction(openInNewTabAction);
	menu.exec(e->globalPos());
}

/**
 * Slot-function that is called when an entry is activated.
 * @param	index	The index of the corresponding entry. 
 */
void PuMP_DirectoryView::on_activated(const QModelIndex &index)
{
	QFileInfo info = model.fileInfo(index);
	if(!info.isDir()) emit viewerRequested(info, true);
}

/**
 * Slot-function that is called when an entry was clicked.
 * @param	index	The index of the corresponding entry.
 */
void PuMP_DirectoryView::on_clicked(const QModelIndex &index)
{
	setCurrentIndex(index);
	QFileInfo info = model.fileInfo(index);
	if(info.isDir()) emit overviewRequested(info);
}

/**
 * Slot-function that is called when the open-action was triggered.
 */
void PuMP_DirectoryView::on_openAction_triggered()
{
	QModelIndex index = currentIndex();
	if(!index.isValid()) return;
	
	QFileInfo info = model.fileInfo(index);
	if(!info.isDir()) emit viewerRequested(info, false);
}

/**
 * Slot-function that is called when the open-in-a-new-tab-action was triggered.
 */
void PuMP_DirectoryView::on_openInNewTabAction_triggered()
{
	QModelIndex index = currentIndex();
	if(!index.isValid()) return;
	
	QFileInfo info = model.fileInfo(index);
	if(!info.isDir()) emit viewerRequested(info, true);
}

/**
 * Slot-function that is called when the refresh-action was triggered.
 */
void PuMP_DirectoryView::on_refreshAction_triggered()
{
	QModelIndex index = currentIndex();
	if(index.isValid())
	{
		model.refresh(index);
		emit refreshRequested();
	}
}


/*****************************************************************************/
