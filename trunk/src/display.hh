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

#ifndef DISPLAY_HH_
#define DISPLAY_HH_

#include <QFileInfo>
#include <QImage>
#include <QMatrix>
#include <QPixmap>
#include <QScrollArea>
#include <QThread>
#include <QWidget>

#define MAX_ZOOM_STEPS	8
#define DEFAULT_ZOOM	4

/*****************************************************************************/

class PuMP_DisplayLoader : public QThread
{
	Q_OBJECT
	
	protected:
		QFileInfo info;
		QImage image;

		void run();
	
	public:
		PuMP_DisplayLoader(QObject *parent = 0) : QThread(parent){};
		
		void load(const QFileInfo &info);
	
	signals:
		void imageIsNull(const QFileInfo &info);
		void imageLoaded(const QImage &image);
};

/*****************************************************************************/

class PuMP_Display : public QWidget
{
	Q_OBJECT
	
	protected:
		void mousePressEvent(QMouseEvent *event);
		void paintEvent(QPaintEvent *event);
	
	public:
		bool hasNext;
		bool hasPrevious;
		bool mirroredHorizontal;
		bool mirroredVertical;
		int rotation;
		bool scaled;
		bool sizeOriginal;
		int zoom;
		
		PuMP_DisplayLoader loader;

		QImage image;
		QMatrix matrix;
		QPixmap displayed;

		PuMP_Display(const QFileInfo &info = QFileInfo(), QWidget *parent = 0);
		QSize sizeHint() const;
	
	public slots:
		void on_loader_imageIsNull(const QFileInfo &info);
		void on_loader_imageLoaded(const QImage &image);
	
	signals:
		void loadingError();
};

/*****************************************************************************/

class PuMP_DisplayView : public QScrollArea
{
	Q_OBJECT

	protected:
		QFileInfo info;
		QPoint lastPos;

		void contextMenuEvent(QContextMenuEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);		
		void moveBy(int x, int y);

	public:
		PuMP_Display display;

		PuMP_DisplayView(const QFileInfo &info, QWidget *parent = 0);
		
		QString fileName() const;
		QString filePath() const;
		QFileInfo getSuccessor(bool previous = false) const;

		void setImage(const QFileInfo &info);

	public slots:
		void on_display_loadingError();
		
	signals:
		void loadingError(PuMP_DisplayView *view);	
};

/*****************************************************************************/

#endif /*DISPLAY_HH_*/
