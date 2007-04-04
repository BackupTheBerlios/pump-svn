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

#ifndef IMAGEVIEW_HH_
#define IMAGEVIEW_HH_

#include <QAction>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QTabWidget>

/*****************************************************************************/

class PuMP_Overview;
class PuMP_DisplayView;

/*****************************************************************************/

class PuMP_ImageView : public QTabWidget
{
	Q_OBJECT

	protected:
		PuMP_Overview *overview;

		QMap<QString, QFileInfo> infos;
		QMap<QString, PuMP_DisplayView *> tabs;
		
		void contextMenuEvent(QContextMenuEvent *e);
	
	public:
		static QAction *closeAllAction;
		static QAction *closeOthersAction;

		PuMP_ImageView(QStringList &nameFilters, QWidget *parent = 0);
		~PuMP_ImageView();
	
	public slots:
		void on_addAction_triggered();
		void on_closeAction_triggered();
		void on_currentChanged(int index); 
		void on_displayView_loadingError(PuMP_DisplayView *view = NULL);
		void on_mirrorHAction();
		void on_mirrorVAction();
		void on_openImage(const QFileInfo &info, bool newTab);
		void on_rotateCWAction();
		void on_rotateCCWAction();
		void on_sizeOriginalAction();
		void on_sizeFittedAction();
		void on_zoomInAction();
		void on_zoomOutAction();
		void on_updateStatusBar(int value, const QString &text);
	
	protected slots:
		void on_closeAllAction_triggered();
		void on_closeOthersAction_triggered();
	
	signals:
		void updateStatusBar(int value, const QString &text);
};

/*****************************************************************************/

#endif /*IMAGEVIEW_HH_*/
