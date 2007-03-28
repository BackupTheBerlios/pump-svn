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

		QAction *closeAction;
		QAction *closeAllAction;
		QAction *closeOthersAction;
		
		QMap<QString, QFileInfo> infos;
		QMap<QString, PuMP_DisplayView *> tabs;
		
		void contextMenuEvent(QContextMenuEvent *e);
	
	public:
		PuMP_ImageView(QStringList &nameFilters, QWidget *parent = 0);
		~PuMP_ImageView();
	
	public slots:
		void display(const QFileInfo &info, bool newTab);
		void createOverview(const QFileInfo &info);
		void mirror(bool horizontal);

		void on_displayView_loadingError(PuMP_DisplayView *view = NULL);
		void on_updateStatusBar(int value, const QString &text);

		void rotate(bool clockwise);
		void stopCreateOverview();
		void zoomIn();
		void zoomOut();
	
	protected slots:
		void on_closeAction_triggered();
		void on_closeAllAction_triggered();
		void on_closeOthersAction_triggered();
	
	signals:
		void updateStatusBar(int value, const QString &text);
};

/*****************************************************************************/

#endif /*IMAGEVIEW_HH_*/
