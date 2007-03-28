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

#ifndef OVERVIEW_HH_
#define OVERVIEW_HH_

#include <QAbstractListModel>
#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QList>
#include <QListView>
#include <QPixmap>
#include <QStringList>
#include <QThread>

#define MAX_VISIBLE_NAME_LENGTH	15
#define THUMBNAIL_SIZE			64
#define GRID_MARGIN_X			10
#define GRID_MARGIN_Y			60

/*****************************************************************************/

class PuMP_OverviewModel : public QAbstractListModel
{
	Q_OBJECT

	protected:
		QList<QPixmap> pixmaps;
		QList<QString> names;
		QList<QString> properties;
	
	public:
		PuMP_OverviewModel(QObject *parent = 0) : QAbstractListModel(parent){}

		void addImage(
			const QPixmap &pixmap,
			const QString &name,
			const QString &props);

		QVariant data(
			const QModelIndex &index,
			int role = Qt::DisplayRole) const;

		Qt::ItemFlags flags(const QModelIndex &index) const;
		QString getFileName(const QModelIndex &index) const;
		int getRowFromName(const QString &name) const;
		bool removeRows(int row, int count, const QModelIndex &parent);
		int rowCount(const QModelIndex &parent) const;
};

/*****************************************************************************/

class PuMP_OverviewLoader : public QThread
{
	Q_OBJECT
	
	protected:
		bool killed;
		QFileInfo info;
		QImageReader reader;
		QSize size;
		
		void run();
	
	public:
		QImage result;
		QSize rSize;
		QString rName;
		QString rProperties;

		PuMP_OverviewLoader(QObject *parent = 0);
		
		void processImage(const QFileInfo &fileInfo);
		void setKilled();
		bool wasKilled();
	
	signals:
		void imageIsNull(const QFileInfo &info);
		void processedImage(
			const QString &name,
			const QString &properties,
			const QImage &image);
};

/*****************************************************************************/

class PuMP_Overview : public QListView
{
	Q_OBJECT
	
	protected:
		int progress;
		int progressMax;

		PuMP_OverviewModel model;
		PuMP_OverviewLoader loader;

		QAction *openAction;
		QAction *openDirAction;
		QAction *openInNewTabAction;
		QDir dir;
		QList<QFileInfo> current;
		
		void contextMenuEvent(QContextMenuEvent *e);
	
	public:
		PuMP_Overview(QStringList &nameFilters, QWidget *parent = 0);
		~PuMP_Overview();

		void create(const QFileInfo &info);
		void stop();
	
	protected slots:
		void on_activated(const QModelIndex &index);
		void on_openAction_triggered();
		void on_openDirAction_triggered();
		void on_openInNewTabAction_triggered();
		
	public slots:
		void on_loader_finished();
		void on_loader_imageIsNull(const QFileInfo &info);
		void on_loader_processedImage(
			const QString &name,
			const QString &properties,
			const QImage &image);
	
	signals:
		void updateStatusBar(int value, const QString &text);
		void viewerRequested(const QFileInfo &info, bool newPage);

};

/*****************************************************************************/

#endif /*OVERVIEW_HH_*/
