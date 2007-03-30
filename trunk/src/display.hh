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
#include <QPixmap>
#include <QScrollArea>
#include <QThread>
#include <QWidget>

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
		bool scaled;

		QImage image;
		QPixmap displayed;

		PuMP_DisplayLoader loader;

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
		PuMP_Display display;

		QFileInfo info;
		QPoint lastPos;

		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);		
		void moveBy(int x, int y);

	public:
		PuMP_DisplayView(const QFileInfo &info, QWidget *parent = 0);

		QString fileName() const;
		QString filePath() const;

		void mirror(bool horizontal = false);
		void rotate(bool clockwise = true);
		void setImage(const QFileInfo &info);

		void zoomIn();
		void zoomOut();
	
	public slots:
		void on_display_loadingError();

	signals:
		void loadingError(PuMP_DisplayView *view);	
};

/*****************************************************************************/

#endif /*DISPLAY_HH_*/
