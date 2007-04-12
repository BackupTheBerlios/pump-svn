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
#include <QDir>
#include <QList>
#include <QMatrix>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>

#include "imageView.hh"
#include "mainWindow.hh"
#include "tabView.hh"

/*****************************************************************************/

/**
 * Constructor of class PuMP_Display that sets this displays image and is
 * responsible for painting it. Can be OpenGL-supported if PuMP was compiled
 * with the appropriate flag.
 * @param	parent	The parent widget of this view.
 */
PuMP_Display::PuMP_Display(QWidget *parent)
	: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

/**
 * The overloaded function that handles mouse-press-events for this widget.
 * @param	event	The mouse-event that occured.
 */
void PuMP_Display::mousePressEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::MidButton)
	{
		if(PuMP_MainWindow::sizeOriginalAction->isEnabled())
			PuMP_MainWindow::sizeOriginalAction->trigger();
		else PuMP_MainWindow::sizeFittedAction->trigger();

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
	if(image.isNull()) resize(QSize(1, 1));
	else
	{	
		QPainter painter(this);
		painter.setClipRegion(event->region());
		painter.drawPixmap(event->rect(), image, event->rect());
	}
}

/**
 * The overloaded function that is called from other widgets to obtain a
 * suitable size-value for this widget.
 */
QSize PuMP_Display::sizeHint() const
{
	QSize dsize = size();
	if(!image.isNull()) dsize = image.size();

	return dsize;
}

/*****************************************************************************/

/**
 * Constructor of class PuMP_ImageProcessor that basically sets up the image
 * variables.
 * @param	parent	The parent of this class.
 */
PuMP_ImageProcessor::PuMP_ImageProcessor(QObject *parent) : QThread(parent)
{
	hasNext = false;
	hasPrevious = false;
	mirroredHorizontal = false;
	mirroredVertical = false;
	rotation = 0;
	scaled = false;
	zoom = DEFAULT_ZOOM;
	mode = PuMP_ImageView::None;	
}

/**
 * Function that returns a file-info-object pointing to the current images
 * successor in its directory.
 * @param	previous	Flag indicating if the successor should be the next
 * 						or the previous entry.
 * @return	A file-info object pointing on the demanded successor, an empty
 * 			file-info-object, if current has none.
 */
QFileInfo PuMP_ImageProcessor::getSuccessor(bool previous) const
{
	QFileInfo succ;
	if(previous && !hasPrevious) return succ;
	if(!previous && !hasNext) return succ;
	if(!info.exists()) return succ;

	QList<QFileInfo> list = info.dir().entryInfoList(
		PuMP_MainWindow::nameFilters, QDir::Files);
	int index = list.indexOf(info);
	if(previous && index == 0) return succ;
	if(!previous && index == (list.size() - 1)) return succ;
	
	if(previous) index--;
	else index++;
	
	succ = list.at(index);
	return succ;
}

/**
 * The overloaded main-function of this thread, which processed the (given)
 * image. On success an imageProcessed-signal will be emitted,
 * otherwise an error-signal will be emitted.
 */
void PuMP_ImageProcessor::run()
{
	bool newImage = false;

	if(mode == PuMP_ImageView::LoadImage)
	{
		image.load(info.filePath());
		newImage = true;
	}
	else if(mode == PuMP_ImageView::LoadNextImage)
	{
		QFileInfo next = getSuccessor();
		if(!next.exists())
		{
			emit error(next.filePath());
			return;
		}
		
		info = next;
		image.load(info.filePath());
		newImage = true;
	}
	else if(mode == PuMP_ImageView::LoadPreviousImage)
	{
		QFileInfo prev = getSuccessor(true);
		if(!prev.exists())
		{
			emit error(prev.filePath());
			return;
		}
		
		info = prev;
		image.load(info.filePath());
		newImage = true;
	}
	else if(mode == PuMP_ImageView::MirrorHorizontally)
	{
		mirroredHorizontal = !mirroredHorizontal;
	}
	else if(mode == PuMP_ImageView::MirrorVertically)
	{
		mirroredVertical = !mirroredVertical;
	}
	else if(mode == PuMP_ImageView::ResizeToOriginal)
	{
		zoom = DEFAULT_ZOOM;
		scaled = false;
	}
	else if(mode == PuMP_ImageView::ResizeToFitted)
	{
		scaled = true;
	}
	else if(mode == PuMP_ImageView::RotateClockWise)
	{
		rotation = (rotation + 90) % 360;
	}
	else if(mode == PuMP_ImageView::RotateCounterClockWise)
	{
		rotation = (rotation - 90) % 360;
	}
	else if(mode == PuMP_ImageView::ZoomIn)
	{
		if(zoom < MAX_ZOOM_STEPS) zoom++;
		scaled = false;
	}
	else if(mode == PuMP_ImageView::ZoomOut)
	{
		if(zoom > 0) zoom--;
		scaled = false;
	}
	else
	{
		qDebug() << "Unknown action demanded";
		emit error(info.filePath());
		return;
	}

	QImage result;
	if(newImage)
	{
		if(image.isNull())
		{
			emit error(info.filePath());
			return;
		}

		QList<QFileInfo> list = info.dir().entryInfoList(
			PuMP_MainWindow::nameFilters, QDir::Files);
		int index = list.indexOf(info);

		hasNext = (index < (list.size() - 1));
		hasPrevious = (index > 0);	
		mirroredHorizontal = false;
		mirroredVertical = false;
		scaled = false;
		zoom = DEFAULT_ZOOM;
		
		result = image;
	}
	else
	{
		QMatrix matrix;
		if(!scaled)
		{
			double factor = zoom - DEFAULT_ZOOM;
			if(factor < 0) factor /= (int)(MAX_ZOOM_STEPS / 2) + 1;
			
			matrix.rotate(rotation);
			matrix.scale(1 + factor, 1 + factor);
			result = image;
			result = result.transformed(matrix, Qt::SmoothTransformation);
			result = result.mirrored(mirroredHorizontal, mirroredVertical);
		}
		else
		{
			matrix.rotate(rotation);
			result = image;
			result = result.transformed(matrix, Qt::SmoothTransformation);
			result = result.mirrored(mirroredHorizontal, mirroredVertical);
			result = result.scaled(
				((PuMP_ImageView *) parent())->size(),
				Qt::KeepAspectRatio,
				Qt::SmoothTransformation);
			
			double factor = ((double) result.width()) / image.width();
			if(factor < 1) factor *= (int)(MAX_ZOOM_STEPS / 2) + 1;

			zoom = ((int) factor) + DEFAULT_ZOOM - 1;
		}
	}
	
	if(result.isNull()) emit error(info.filePath());
	else emit imageProcessed(result);
}

/**
 * Public interface for functionality of this thread. If the thread currently
 * runs the function will simply return. If the given file doesn't exist or
 * is a directory (in case of a load-action) an error-signal will be emitted.
 * @param	mode	The action to perform.
 * @param	info	The QFileInfo-Object representing the image to load.
 */	
void PuMP_ImageProcessor::process(int mode, const QFileInfo &info)
{
	if(isRunning() || mode == PuMP_ImageView::None) return;
	
	this->mode = mode;
	if(mode == PuMP_ImageView::LoadImage)
	{
		if(!info.exists() || info.isDir())
		{
			emit error(info.filePath());
			return;
		}
		
		this->info = info;
	}
	
	start();
}

/*****************************************************************************/

/** init static modex */
int PuMP_ImageView::None = 0; 
int PuMP_ImageView::LoadImage = 1;
int PuMP_ImageView::LoadNextImage = 2;
int PuMP_ImageView::LoadPreviousImage = 4;
int PuMP_ImageView::MirrorHorizontally = 8;
int PuMP_ImageView::MirrorVertically = 16;
int PuMP_ImageView::ResizeToOriginal = 32;
int PuMP_ImageView::ResizeToFitted = 64;
int PuMP_ImageView::RotateClockWise = 128;
int PuMP_ImageView::RotateCounterClockWise = 256;
int PuMP_ImageView::ZoomIn = 512;
int PuMP_ImageView::ZoomOut = 1024;

/**
 * Consructor of class PuMP_ImageView that creates a widget to display images.
 * @param	parent	The parent widget of this view.
 */
PuMP_ImageView::PuMP_ImageView(QWidget *parent)
	: QScrollArea(parent)
{
	lastPos.setX(0);
	lastPos.setY(0);

	display.setParent(this);
	processor.setParent(this);
	connect(
		&processor,
		SIGNAL(error(const QString &)),
		this,
		SLOT(on_error(const QString &)));
	connect(
		&processor,
		SIGNAL(imageProcessed(const QImage &)),
		this,
		SLOT(on_imageProcessed(const QImage &)));

	horizontalScrollBar()->setMinimum(0);
	verticalScrollBar()->setMinimum(0);
	
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
void PuMP_ImageView::contextMenuEvent(QContextMenuEvent *event)
{
	if(!display.image.isNull())
	{
		QMenu menu(this);
		menu.addAction(PuMP_MainWindow::mirrorHAction);
		menu.addAction(PuMP_MainWindow::mirrorVAction);
		menu.addAction(PuMP_MainWindow::rotateCWAction);
		menu.addAction(PuMP_MainWindow::rotateCCWAction);
		menu.addSeparator();
		menu.addAction(PuMP_MainWindow::sizeOriginalAction);
		menu.addAction(PuMP_MainWindow::sizeFittedAction);
		menu.addAction(PuMP_MainWindow::zoomInAction);
		menu.addAction(PuMP_MainWindow::zoomOutAction);
		menu.exec(event->globalPos());
	}
	else event->ignore();
}

/**
 * The overloaded function that handles mouse-move-events for this widget.
 * @param	event	The mouse-event that occured.
 */
void PuMP_ImageView::mouseMoveEvent(QMouseEvent *event)
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
void PuMP_ImageView::mousePressEvent(QMouseEvent *event)
{
	if(event->buttons() == Qt::LeftButton) lastPos = event->pos();
	else event->ignore();
}

/**
 * The overloaded function that handles mouse-release-events for this widget.
 * @param	event	The mouse-event that occured.
 */
void PuMP_ImageView::mouseReleaseEvent(QMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);
	event->ignore();
}	

/**
 * Function that moves the scroll-areas child-widget by the given coordinates.
 * @param	x	The horizontal movement.
 * @param	y	The vertical movement.
 */
void PuMP_ImageView::moveBy(int x, int y)
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
QString PuMP_ImageView::fileName() const
{
	return processor.info.fileName();
}

/**
 * Function that returns the file-path associated with the view's image.
 * @return	The file-path associated with the view's image.
 */
QString PuMP_ImageView::filePath() const
{
	return processor.info.filePath();
}

/**
 * This is a wrapper for PuMP_ImageProcessor::getSuccessor().
 * @param	previous	Flag indicating if the successor should be the next
 * 						or the previous entry.
 * @return	A file-info object pointing on the demanded successor, an empty
 * 			file-info-object, if current has none.
 */
QFileInfo PuMP_ImageView::getSuccessor(bool previous) const
{
	return processor.getSuccessor(previous);
}

/**
 * Function that commands the processor-thread to do the demanded action.
 * @param	mode	The action to process.
 * @param	info	The image to load (if action is load).
 */
void PuMP_ImageView::process(int mode, const QFileInfo &info)
{
	setActions(true);
	processor.process(mode, info);
}

/**
 * Function that enables/disables all actions related to the image according
 * to the images properties.
 * @param	disableAll	Flag to disable all actions. 
 */
void PuMP_ImageView::setActions(bool disableAll)
{
	PuMP_MainWindow::closeAction->setEnabled(!disableAll);
	PuMP_MainWindow::mirrorHAction->setEnabled(!disableAll);
	PuMP_MainWindow::mirrorVAction->setEnabled(!disableAll);
	PuMP_MainWindow::nextAction->setEnabled(!disableAll && processor.hasNext);
	PuMP_MainWindow::previousAction->setEnabled(
		!disableAll && processor.hasPrevious);
	PuMP_MainWindow::rotateCWAction->setEnabled(!disableAll);
	PuMP_MainWindow::rotateCCWAction->setEnabled(!disableAll);
	PuMP_MainWindow::sizeOriginalAction->setEnabled(
		!disableAll && processor.scaled);
	PuMP_MainWindow::sizeFittedAction->setEnabled(
		!disableAll && !processor.scaled);
	PuMP_MainWindow::zoomInAction->setEnabled(
		!disableAll && (processor.zoom < MAX_ZOOM_STEPS));
	PuMP_MainWindow::zoomOutAction->setEnabled(
		!disableAll && (processor.zoom > 0));
}

/**
 * Slot-function that is called when the image couldn't be processed.
 * @param	file	The path of the image that failed.
 */
void PuMP_ImageView::on_error(const QString &file)
{
	qDebug() << "Error processing" << file;
	emit error(this);
}

/**
 * Slot-function that is called when the image was processed.
 * @param	image	The processed image..
 */
void PuMP_ImageView::on_imageProcessed(const QImage &result)
{
	setActions(); // this is a bit buggy because the user could have changed the tab already
	display.image = QPixmap::fromImage(result);
	display.adjustSize();
	display.update();
}

/*****************************************************************************/
