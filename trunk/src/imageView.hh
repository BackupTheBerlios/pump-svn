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

#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QScrollArea>
#include <QThread>

#define MAX_ZOOM_STEPS	8
#define DEFAULT_ZOOM	4

/*****************************************************************************/

class PuMP_Display : public QWidget
{
	Q_OBJECT
	
	protected:
		void mousePressEvent(QMouseEvent *event);
		void paintEvent(QPaintEvent *event);
	
	public:
		QPixmap image;

		PuMP_Display(QWidget *parent = 0);
		QSize sizeHint() const;
	
};

/*****************************************************************************/

class PuMP_ImageProcessor : public QThread
{
	Q_OBJECT
	
	protected:
		int mode;
		QImage image;

		void run();
	
	public:
		QFileInfo info;

		bool hasNext;
		bool hasPrevious;
		bool mirroredHorizontal;
		bool mirroredVertical;
		int rotation;
		bool scaled;
		int zoom;

		PuMP_ImageProcessor(QObject *parent = 0);
		
		QFileInfo getSuccessor(bool previous = false) const;
		void process(int mode, const QFileInfo &info = QFileInfo());
	
	signals:
		void error(const QString &file);
		void imageProcessed(const QImage &result);
};

/*****************************************************************************/

class PuMP_ImageView : public QScrollArea
{
	Q_OBJECT

	protected:
		QPoint lastPos;

		void contextMenuEvent(QContextMenuEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);		
		void moveBy(int x, int y);

	public:
		static int None;
		static int LoadImage;
		static int LoadNextImage;
		static int LoadPreviousImage;
		static int MirrorHorizontally;
		static int MirrorVertically;
		static int ResizeToOriginal;
		static int ResizeToFitted;
		static int RotateClockWise;
		static int RotateCounterClockWise;
		static int ZoomIn;
		static int ZoomOut;

		PuMP_Display display;
		PuMP_ImageProcessor processor;
		PuMP_ImageView(QWidget *parent = 0);
		
		QString fileName() const;
		QString filePath() const;
		QFileInfo getSuccessor(bool previous = false) const;
		void process(int mode, const QFileInfo &info = QFileInfo());
		void setActions(bool disableAll = false);

	public slots:
		void on_error(const QString &file);
		void on_imageProcessed(const QImage &result);
		
	signals:
		void error(PuMP_ImageView *view);	
};

/*****************************************************************************/

#endif /*IMAGEVIEW_HH_*/
