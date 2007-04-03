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
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>

#include "display.hh"

/*****************************************************************************/

/**
 * The overloaded main-function of this thread, which loads the given image.
 * On success an imageLoaded-signal will be emitted, otherwise an imageIsNull-
 * signal will be emitted.
 */
void PuMP_DisplayLoader::run()
{
	image.load(info.filePath());
	if(image.isNull())
	{
		emit imageIsNull(info);
		return;
	}
	else emit imageLoaded(image);
}

/**
 * Public interface for functionality of this thread. If the thread currently
 * runs or the given file doesn't exist or is a directory an imageIsNull-signal
 * will be emitted. 
 * @param	info	The QFileInfo-Object representing the image to load.
 */	
void PuMP_DisplayLoader::load(const QFileInfo &info)
{
	this->info = info;
	if(isRunning() || !info.exists() || info.isDir()) emit imageIsNull(info);

	start();
}

/*****************************************************************************/

/**
 * Constructor of class PuMP_Display that sets this displays image and is
 * responsible for painting it. Can be OpenGL-supported if PuMP was compiled
 * with the appropriate flag.
 * @param	info	The QFileInfo-Object representing the image to show.
 * @param	parent	The parent widget of this view.
 */
PuMP_Display::PuMP_Display(const QFileInfo &info, QWidget *parent)
	: QWidget(parent)
{
	resize(200, 200);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	mirroredHorizontal = false;
	mirroredVertical = false;
	rotation = 0;
	scaled = false;
	sizeOriginal = true;
	zoom = 0;
	
	sizeOriginalAction = NULL;
	sizeFittedAction = NULL;

	loader.setParent(this);
	connect(
		&loader,
		SIGNAL(imageIsNull(const QFileInfo &)),
		this,
		SLOT(on_loader_imageIsNull(const QFileInfo &)));
	connect(
		&loader,
		SIGNAL(imageLoaded(const QImage &)),
		this,
		SLOT(on_loader_imageLoaded(const QImage &)));
	if(info.exists()) loader.load(info);
}

/**
 * The overloaded function that handles mouse-press-events for this widget.
 * @param	event	The mouse-event that occured.
 */
void PuMP_Display::mousePressEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::MidButton)
	{
		assert(sizeOriginalAction != NULL);
		assert(sizeFittedAction != NULL);

		if(!scaled) sizeFittedAction->trigger();
		else sizeOriginalAction->trigger();;

		adjustSize();
	}
	else event->ignore();
}

/**
 * Overloaded function that handles paint-events for this widget. It simply
 * paints the picture the image-variable contains.
 * @param	event	The paint-event that occured. 
 */
void PuMP_Display::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setClipRegion(event->region());
	painter.drawPixmap(event->rect(), displayed, event->rect());
}

/**
 * Function that connects the global actions for the display with its slots.
 * @param	sizeOriginalAction	The action that sets the image to original size.
 * @param	sizeFittedAction	The action that fits the image to window-size.
 */
void PuMP_Display::setupActions(
	QAction *sizeOriginalAction,
	QAction *sizeFittedAction)
{
	assert(sizeOriginalAction != NULL);
	assert(sizeFittedAction != NULL);

	this->sizeOriginalAction = sizeOriginalAction;
	this->sizeFittedAction = sizeFittedAction;
}

/**
 * The overloaded function that is called from other widgets to obtain a
 * suitable size-value for this widget.
 */
QSize PuMP_Display::sizeHint() const
{
	QSize dsize = size();
	if(!displayed.isNull()) dsize = displayed.size();
	 
	return dsize;
}

/**
 * Slot-function that is called when the thread couldn't load the demanded
 * image.
 * @param	info	The QFileInfo-Object representing the image tried
 * 					to load.
 */
void PuMP_Display::on_loader_imageIsNull(const QFileInfo &info)
{
	Q_UNUSED(info);
	emit loadingError();
}

/**
 * Slot-function that simply copies the thread-loaded image.
 * @param	image	Reference to the loaded image.
 */
void PuMP_Display::on_loader_imageLoaded(const QImage &image)
{
	this->image = image;
	displayed = QPixmap::fromImage(image);

	resize(displayed.size());
	repaint();
}

/*****************************************************************************/

/**
 * Consructor of class PuMP_DisplayView that creates a display (with or without
 * opengl-support).
 * @param	info	The QFileInfo-Object representing the image to show.
 * @param	parent	The parent widget of this view.
 */
PuMP_DisplayView::PuMP_DisplayView(const QFileInfo &info, QWidget *parent)
	: QScrollArea(parent)
{
	lastPos.setX(0);
	lastPos.setY(0);

	display.setParent(this);
	connect(
		&display,
		SIGNAL(loadingError()),
		this,
		SLOT(on_display_loadingError()));
	setImage(info);

	mirrorHAction = NULL;
	mirrorVAction = NULL;
	rotateCWAction = NULL;
	rotateCCWAction = NULL;
	sizeOriginalAction = NULL;
	sizeFittedAction = NULL;
	zoomInAction = NULL;
	zoomOutAction = NULL;

	horizontalScrollBar()->setMinimum(0);
	verticalScrollBar()->setMinimum(0);
	
	setAlignment(Qt::AlignCenter);
	setBackgroundRole(QPalette::Dark);
	setCursor(Qt::OpenHandCursor);
	setMouseTracking(false);
	setWidget(&display);
}

/**
 * Overloaded function for context-menu-events. It provides a custom menu for
 * the this image.
 * @param	e The context-menu-event. 
 */
void PuMP_DisplayView::contextMenuEvent(QContextMenuEvent *event)
{
	if(!display.displayed.isNull())
	{
		assert(mirrorHAction != NULL);
		assert(mirrorVAction != NULL);
		assert(rotateCWAction != NULL);
		assert(rotateCCWAction != NULL);
		assert(sizeOriginalAction != NULL);
		assert(sizeFittedAction != NULL);
		assert(zoomInAction != NULL);
		assert(zoomOutAction != NULL);
	
		QMenu menu(this);
		menu.addAction(mirrorHAction);
		menu.addAction(mirrorVAction);
		menu.addAction(rotateCWAction);
		menu.addAction(rotateCCWAction);
		menu.addSeparator();
		menu.addAction(sizeOriginalAction);
		menu.addAction(sizeFittedAction);
		menu.addAction(zoomInAction);
		menu.addAction(zoomOutAction);
		menu.exec(event->globalPos());
	}
	else event->ignore();
}

/**
 * The overloaded function that handles mouse-move-events for this widget.
 * @param	event	The mouse-event that occured.
 */
void PuMP_DisplayView::mouseMoveEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::LeftButton)
	{
		if(horizontalScrollBar()->maximum() != 0 &&
			verticalScrollBar()->maximum() != 0)
		{
			setUpdatesEnabled(false);
			setCursor(Qt::ClosedHandCursor);
			
			int x = lastPos.x() - event->x();
			int y = lastPos.y() - event->y();
			
			moveBy(x, y);
			lastPos = event->pos();
			setUpdatesEnabled(true);
		}
	}
	else event->ignore();
}

/**
 * The overloaded function that handles mouse-press-events for this widget.
 * @param	event	The mouse-event that occured.
 */
void PuMP_DisplayView::mousePressEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::LeftButton) lastPos = event->pos();
	else event->ignore();
}

/**
 * The overloaded function that handles mouse-release-events for this widget.
 * @param	event	The mouse-event that occured.
 */
void PuMP_DisplayView::mouseReleaseEvent(QMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);
	event->ignore();
}	

/**
 * Function that moves the scroll-areas child-widget by the given coordinates.
 * @param	x	The horizontal movement.
 * @param	y	The vertical movement.
 */
void PuMP_DisplayView::moveBy(int x, int y)
{
	QSize s = widget()->size();
	double pX = s.width() / horizontalScrollBar()->maximum();
	double pY = s.height() / verticalScrollBar()->maximum();
	int valX = horizontalScrollBar()->value();
	int valY = verticalScrollBar()->value();

	horizontalScrollBar()->setValue((int)(valX + x * pX));
	verticalScrollBar()->setValue((int)(valY + y * pY));
}

/**
 * Function that returns the file-name associated with the view's image.
 * @return	The file-name associated with the view's image.
 */
QString PuMP_DisplayView::fileName() const
{
	return info.fileName();
}

/**
 * Function that returns the file-path associated with the view's image.
 * @return	The file-path associated with the view's image.
 */
QString PuMP_DisplayView::filePath() const
{
	return info.filePath();
}

/**
 * Function to set the image displayed by this view.
 * @param	info	The QFileInfo-Object representing the image to show.
 */
void PuMP_DisplayView::setImage(const QFileInfo &info)
{
	this->info = info;
	display.loader.load(this->info);
}

/**
 * Function that connects the global actions for the display with its slots.
 * @param	mirrorHAction	The action that mirrors the image horizontally.
 * @param	mirrorVAction	The action that mirrors the image vertically.
 * @param	rotateCWAction	The action that rotates the image clockwise.
 * @param	rotateCCWAction	The action that rotates the image counter-clockwise.
 * @param	sizeOriginalAction	The action that sets the image to original size.
 * @param	sizeFittedAction	The action that fits the image to window-size.
 * @param	zoomInAction	The action that zooms into the image.
 * @param	zoomOutAction	The action that zooms out of the image.
 */
void PuMP_DisplayView::setupActions(
	QAction *mirrorHAction,
	QAction *mirrorVAction,
	QAction *rotateCWAction,
	QAction *rotateCCWAction,
	QAction *sizeOriginalAction,
	QAction *sizeFittedAction,
	QAction *zoomInAction,
	QAction *zoomOutAction)
{
	assert(mirrorHAction != NULL);
	assert(mirrorVAction != NULL);
	assert(rotateCWAction != NULL);
	assert(rotateCCWAction != NULL);
	assert(sizeOriginalAction != NULL);
	assert(sizeFittedAction != NULL);
	assert(zoomInAction != NULL);
	assert(zoomOutAction != NULL);

	this->mirrorHAction = mirrorHAction;
	this->mirrorVAction = mirrorVAction;
	this->rotateCWAction = rotateCWAction;
	this->rotateCCWAction = rotateCCWAction;
	this->sizeOriginalAction = sizeOriginalAction;
	this->sizeFittedAction = sizeFittedAction;
	this->zoomInAction = zoomInAction;
	this->zoomOutAction = zoomOutAction;

	display.setupActions(sizeOriginalAction, sizeFittedAction);
}

/**
 * Slot-function that is called when the display couldn't load the demanded
 * image.
 */
void PuMP_DisplayView::on_display_loadingError()
{
	emit loadingError(this);
}

/*****************************************************************************/
