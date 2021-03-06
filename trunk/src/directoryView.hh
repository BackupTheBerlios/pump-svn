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

#ifndef DIRECTORYVIEW_HH_
#define DIRECTORYVIEW_HH_

#include <QAction>
#include <QDirModel>
#include <QFileInfo>
#include <QList>
#include <QTreeView>

/*****************************************************************************/

class PuMP_DirectoryView : public QTreeView
{
	Q_OBJECT

	protected:
		int historyCurrent;

		QDirModel model;
		QList<QFileInfo> history;
		
		void appendToHistory(const QFileInfo &info);
		void contextMenuEvent(QContextMenuEvent *e);
	
	public:
		static QAction *openAction;
		static QAction *openInNewTabAction;

		PuMP_DirectoryView(QWidget *parent = 0);
		~PuMP_DirectoryView();
	
	protected slots:
		void on_activated(const QModelIndex &index, bool newTab = true);
		void on_clicked(const QModelIndex &index);
		void on_collapsedOrExpanded(const QModelIndex &index);
	
	public slots:
		void on_backwardAction_triggered();
		void on_forwardAction_triggered();
		void on_homeAction_triggered();
		void on_openAction_triggered();
		void on_openInNewTabAction_triggered();
		void on_parentAction_triggered();
		void on_refreshAction_triggered();
	
	signals:
		void openImage(const QFileInfo &info, bool newPage);
};

/*****************************************************************************/

#endif /*DIRECTORYVIEW_HH_*/
