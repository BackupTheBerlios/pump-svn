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
#include <math.h>

#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>

#include "display.hh"
#include "imageView.hh"
#include "overview.hh"

/*****************************************************************************/

/**
 * Constructor of class PuMP_ImageView which mainly sets up the given
 * overview-tab and connects the needed slots
 * @param	nameFilters	A string containing the supported image-formats the
 * 						application can handle.
 * @param	parent		Pointer on this widgets parent-widget.
 */
PuMP_ImageView::PuMP_ImageView(QStringList &nameFilters, QWidget *parent)
	: QTabWidget(parent)
{
	overview = new PuMP_Overview(nameFilters, parent);
	connect(
		overview,
		SIGNAL(dirOpened(const QFileInfo &)),
		this,
		SLOT(on_overview_dirOpened(const QFileInfo &)));
	connect(
		overview,
		SIGNAL(openImage(const QFileInfo &, bool)),
		this,
		SLOT(on_openImage(const QFileInfo &, bool)));
	connect(
		overview,
		SIGNAL(updateStatusBar(int, const QString &)),
		this,
		SLOT(on_updateStatusBar(int, const QString &)));
	addTab(overview, "Overview");
	
	connect(
		this,
		SIGNAL(currentChanged(int)),
		this,
		SLOT(on_currentChanged(int)));

	addAction = NULL;
	closeAction = NULL;
	closeAllAction = NULL;
	closeOthersAction = NULL;
	mirrorHAction = NULL;
	mirrorVAction = NULL;
	rotateCWAction = NULL;
	rotateCCWAction = NULL;
	sizeOriginalAction = NULL;
	sizeFittedAction = NULL;
	zoomInAction = NULL;
	zoomOutAction = NULL;
}

/**
 * Destructor of class PuMP_ImageView that clears all data strucutures and
 * removes all tabs.
 */
PuMP_ImageView::~PuMP_ImageView()
{
	delete closeAllAction;
	delete closeOthersAction;
	
	removeTab(indexOf(overview));
	delete overview;
	
	QMap<QString, PuMP_DisplayView *>::iterator it;
	for(it = tabs.begin(); it != tabs.end(); it++)
	{
		removeTab(indexOf(it.value()));
		infos.remove(it.key());
		delete it.value();
	}
	tabs.clear();
}

/**
 * Overloaded function for context-menu-events. It provides a custom menu for
 * the overview and the other tabs.
 * @param	e The context-menu-event. 
 */
void PuMP_ImageView::contextMenuEvent(QContextMenuEvent *e)
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0) return;
	
	closeAction->setEnabled(true);
	closeAllAction->setEnabled(true);
	closeOthersAction->setEnabled(true);
	if(cw == (QWidget *) overview) closeAction->setEnabled(false);

	QMenu menu(this);
	menu.addAction(closeAction);
	menu.addSeparator();
	menu.addAction(closeAllAction);
	menu.addAction(closeOthersAction);
	menu.exec(e->globalPos());
}

/**
 * Function that connects the global actions for the imageview with its slots.
 * @param	addAction	The action that opens the current image in a new tab.
 * @param	closeAction	The action that closes the current tab.
 * @param	mirrorHAction	The action that mirrors the image horizontally.
 * @param	mirrorVAction	The action that mirrors the image vertically.
 * @param	refreshAction	The action to refresh the current directory.
 * @param	rotateCWAction	The action that rotates the image clockwise.
 * @param	rotateCCWAction	The action that rotates the image counter-clockwise.
 * @param	sizeOriginalAction	The action that sets the image to original size.
 * @param	sizeFittedAction	The action that fits the image to window-size.
 * @param	stopAction	The action that stops current processings.
 * @param	zoomInAction	The action that zooms into the image.
 * @param	zoomOutAction	The action that zooms out of the image.
 */
void PuMP_ImageView::setupActions(
	QAction *addAction,
	QAction *closeAction,
	QAction *mirrorHAction,
	QAction *mirrorVAction,
	QAction *refreshAction,
	QAction *rotateCWAction,
	QAction *rotateCCWAction,
	QAction *sizeOriginalAction,
	QAction *sizeFittedAction,
	QAction *stopAction,
	QAction *zoomInAction,
	QAction *zoomOutAction)
{
	assert(addAction != NULL);
	assert(closeAction != NULL);

	this->addAction = addAction;
	connect(
		addAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_addAction_triggered()));
	this->closeAction = closeAction;
	connect(
		closeAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeAction_triggered()));
	closeAllAction = new QAction("Close all tabs", this);
	connect(
		closeAllAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeAllAction_triggered()));
	closeOthersAction = new QAction("Close other tabs", this);
	connect(
		closeOthersAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeOthersAction_triggered()));
	this->mirrorHAction = mirrorHAction;
	connect(
		mirrorHAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_mirrorHAction()));
	this->mirrorVAction = mirrorVAction;
	connect(
		mirrorVAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_mirrorVAction()));
	this->rotateCWAction = rotateCWAction;
	connect(
		rotateCWAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_rotateCWAction()));
	this->rotateCCWAction = rotateCCWAction;
	connect(
		rotateCCWAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_rotateCCWAction()));
	this->sizeOriginalAction = sizeOriginalAction;
	connect(
		sizeOriginalAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_sizeOriginalAction()));
	this->sizeFittedAction = sizeFittedAction;
	connect(
		sizeFittedAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_sizeFittedAction()));
	this->zoomInAction = zoomInAction;
	connect(
		zoomInAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_zoomInAction()));
	this->zoomOutAction = zoomOutAction;
	connect(
		zoomOutAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_zoomOutAction()));

	assert(overview != NULL);	
	overview->setupActions(refreshAction, stopAction);
}

/**
 * Slot-function that is called, when a new tab should be opened.
 */
void PuMP_ImageView::on_addAction_triggered()
{
	
}

/**
 * Slot-function that is called when the user demands to close the current tab.
 * The overview can't be closed.
 */
void PuMP_ImageView::on_closeAction_triggered()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0) return;

	QMap<QString, PuMP_DisplayView *>::iterator it = tabs.begin();
	while(it != tabs.end())
	{
		if((QWidget *) it.value() == cw)
		{
			removeTab(indexOf(it.value()));
			delete it.value();
			infos.remove(it.key());
			tabs.erase(it);
			break;
		}
		else it++;
	}
}

/**
 * Slot-function that is called, when the current tabs changed. It sets up the
 * actions states for the new current tab.
 */
void PuMP_ImageView::on_currentChanged(int index)
{
	QWidget *cw = widget(index);
	if(cw == NULL || tabs.size() == 0 || cw == overview)
	{
		bool enable = (tabs.size() != 0);
		addAction->setEnabled(true);
		closeAction->setEnabled(false);
		closeAllAction->setEnabled(enable);
		closeOthersAction->setEnabled(enable);
		mirrorHAction->setEnabled(false);
		mirrorVAction->setEnabled(false);
		rotateCWAction->setEnabled(false);
		rotateCCWAction->setEnabled(false);
		sizeOriginalAction->setEnabled(false);
		sizeFittedAction->setEnabled(false);
		zoomInAction->setEnabled(false);
		zoomOutAction->setEnabled(false);
	}
	else
	{
		bool enable = (tabs.size() != 1);
		PuMP_DisplayView *view = (PuMP_DisplayView *) cw;
		
		addAction->setEnabled(true);
		closeAction->setEnabled(true);
		closeAllAction->setEnabled(true);
		closeOthersAction->setEnabled(enable);
		mirrorHAction->setEnabled(true);
		mirrorVAction->setEnabled(true);
		rotateCWAction->setEnabled(true);
		rotateCCWAction->setEnabled(true);
		sizeOriginalAction->setEnabled(view->display.scaled);
		sizeFittedAction->setEnabled(!view->display.scaled);
		zoomInAction->setEnabled((view->display.zoom != MAX_ZOOM_STEPS));
		zoomOutAction->setEnabled(
			(view->display.zoom != (-1) * MAX_ZOOM_STEPS));
	}
}

/**
 * Slot-function that is called when an display-view failed to load its image.
 * @param	view	A pointer to the display-view that failed.
 */
void PuMP_ImageView::on_displayView_loadingError(PuMP_DisplayView *view)
{
	if(view == NULL) return;

	QMessageBox::information(
		this,
		"Information",
		"Failed to load \"" + view->fileName() + "\"");
	
	QMap<QString, PuMP_DisplayView *>::iterator it = tabs.begin();
	while(it != tabs.end())
	{
		if(it.value() == view)
		{
			removeTab(indexOf(it.value()));
			delete it.value();
			infos.remove(it.key());
			tabs.erase(it);
			break;
		}
		else it++;
	}
}

/**
 * Slot-function that is called to mirror the current image horizontally.
 */
void PuMP_ImageView::on_mirrorHAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;
	view->display.displayed = QPixmap::fromImage(
		view->display.displayed.toImage().mirrored(true, false));
	view->display.mirroredHorizontal = !view->display.mirroredHorizontal;
	view->display.adjustSize(); 
	view->display.update();
}

/**
 * Slot-function that is called to mirror the current image vertically.
 */
void PuMP_ImageView::on_mirrorVAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;
	view->display.displayed = QPixmap::fromImage(
		view->display.displayed.toImage().mirrored());
	view->display.mirroredVertical = !view->display.mirroredVertical;
	view->display.adjustSize();
	view->display.update();
}

/**
 * Slot-function that displays the given image in the current-tab or in a
 * new tab.
 * @param	info	The image to load.
 * @param	newTab	Flag indicating if the image will be opened in a new tab.
 */
void PuMP_ImageView::on_openImage(const QFileInfo &info, bool newTab)
{
	if(info.isDir() || !info.exists())
	{
		QMessageBox::information(
			this,
			"Information",
			"Cannot show \"" + info.fileName() + "\"");
		return;
	}
	
	if(tabs.find(info.filePath()) != tabs.end()) return;

	if(tabs.size() == 0 || newTab)
	{
		PuMP_DisplayView *view = new PuMP_DisplayView(info, this);
		connect(
			view,
			SIGNAL(loadingError(PuMP_DisplayView *)),
			this,
			SLOT(on_displayView_loadingError(PuMP_DisplayView *)));
		view->setupActions(
			mirrorHAction,
			mirrorVAction,
			rotateCWAction,
			rotateCCWAction,
			sizeOriginalAction,
			sizeFittedAction,
			zoomInAction,
			zoomOutAction);

		tabs[info.filePath()] = view;
		infos[info.filePath()] = info;

		int index = addTab(view, info.fileName());
		setTabToolTip(index, info.filePath());

		setCurrentWidget(view);
	}
	else
	{
		PuMP_DisplayView *view = (PuMP_DisplayView *) widget(1);
		if(view == NULL)
		{
			QMessageBox::information(
				this,
				"Information",
				"Cannot show \"" + info.fileName() + "\"");
			return;
		}
		
		infos.remove(view->filePath());
		infos[info.filePath()] = info;
		
		tabs.remove(view->filePath());
		tabs[info.filePath()] = view;
		
		view->setImage(info);
		setTabText(1, info.fileName());
		setTabToolTip(1, info.filePath());
		
		setCurrentWidget(view);
	}
}

/**
 * Slot-function that is called to request an overview.
 * @param	info	File-info-object representing the folder to overview.
 */
void PuMP_ImageView::on_openDir(const QFileInfo &info)
{
	overview->on_open(info);
}

/**
 * Slot-function that hands out the overview's dir-opened-signal.
 * @param	info	The QFileInfo-Object pointing on the new selection.
 */
void PuMP_ImageView::on_overview_dirOpened(const QFileInfo &info)
{
	emit dirOpened(info);
}

/**
 * Slot-function that is called to rotate the current image clockwise by
 * 90 degrees.
 */
void PuMP_ImageView::on_rotateCWAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;

	QMatrix matrix;
	matrix.rotate(90);
	
	view->display.displayed = QPixmap::fromImage(
		view->display.displayed.toImage().transformed(
		matrix, Qt::SmoothTransformation));
	view->display.rotation = (view->display.rotation + 90) % 360;
	view->display.adjustSize();
	view->display.update();
}

/**
 * Slot-function that is called to rotate the current image
 * counter-clockwise by 90 degrees.
 */
void PuMP_ImageView::on_rotateCCWAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;

	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;

	QMatrix matrix;
	matrix.rotate(-90);
	
	view->display.displayed = QPixmap::fromImage(
		view->display.displayed.toImage().transformed(
		matrix, Qt::SmoothTransformation));
	view->display.rotation = (view->display.rotation - 90) % 360;
	view->display.adjustSize();
	view->display.update();
}

/**
 * Slot-function that shows the current image in original size.
 */
void PuMP_ImageView::on_sizeOriginalAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;

	sizeOriginalAction->setEnabled(false);
	sizeFittedAction->setEnabled(true);

	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;

	QImage temp = view->display.image.mirrored(
		view->display.mirroredHorizontal,
		view->display.mirroredVertical);

	QMatrix matrix;
	matrix.rotate(view->display.rotation);
	temp = temp.transformed(matrix, Qt::SmoothTransformation);

	view->display.displayed = QPixmap::fromImage(temp);
	view->display.zoom = 0;
	view->display.scaled = false;
	view->display.adjustSize();
	view->display.update();
}

/**
 * Slot-function that shows the current image fitted to the displays size.
 */
void PuMP_ImageView::on_sizeFittedAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;

	sizeOriginalAction->setEnabled(true);
	sizeFittedAction->setEnabled(false);

	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;

	QImage temp = view->display.image.scaled(
		view->size(),
		Qt::KeepAspectRatio,
		Qt::SmoothTransformation);
					
	temp = temp.mirrored(
		view->display.mirroredHorizontal,
		view->display.mirroredVertical);

	QMatrix matrix;
	matrix.rotate(view->display.rotation);
	temp = temp.transformed(matrix, Qt::SmoothTransformation);
	
	double factor = ((double) temp.width()) / view->display.image.width();
	factor = (log(factor) / log(2));
	view->display.zoom = (int) factor;
	view->display.scaled = true;
	
	zoomInAction->setEnabled(true);
	if(view->display.zoom == (-1) * MAX_ZOOM_STEPS)
		zoomOutAction->setEnabled(false);

	view->display.displayed = QPixmap::fromImage(temp);
	view->display.adjustSize();
	view->display.update();
}

/**
 * Slot-function that is called to zoom into the current image.
 */
void PuMP_ImageView::on_zoomInAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;
	if(view->display.zoom == MAX_ZOOM_STEPS) return;
	
	view->display.zoom += 1;
	double factor = pow(2, view->display.zoom);
	
	QMatrix matrix;
	matrix.scale(factor, factor);

	zoomOutAction->setEnabled(true);
	if(view->display.zoom == MAX_ZOOM_STEPS)
		zoomInAction->setEnabled(false);
	
	view->display.displayed = QPixmap::fromImage(
		view->display.image.transformed(matrix,
		Qt::SmoothTransformation));
	view->display.adjustSize();
	view->display.update();
}

/**
 * Slot-function that is called to zoom out of the current image.
 */
void PuMP_ImageView::on_zoomOutAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;
	if(view->display.zoom == (-1) * MAX_ZOOM_STEPS) return;
	
	view->display.zoom -= 1;
	double factor = pow(2, view->display.zoom);
	
	QMatrix matrix;
	matrix.scale(factor, factor);

	zoomInAction->setEnabled(true);
	if(view->display.zoom == (-1) * MAX_ZOOM_STEPS)
		zoomOutAction->setEnabled(false);
	
	view->display.displayed = QPixmap::fromImage(
		view->display.image.transformed(matrix,
		Qt::SmoothTransformation));
	view->display.adjustSize();
	view->display.update();
}

/**
 * Slot-function that enables all components of this widget to send
 * messages to the main-window's status-bar.
 * @param	value	The value for the status-bar's progress-bar.
 * @param	text	The message displayed at the status-bar.
 */
void PuMP_ImageView::on_updateStatusBar(int value, const QString &text)
{
	emit updateStatusBar(value, text);
}

/**
 * Slot-function that is called when the user demands to close all tabs.
 * The overview can't be closed.
 */
void PuMP_ImageView::on_closeAllAction_triggered()
{
	QMap<QString, PuMP_DisplayView *>::iterator it;
	for(it = tabs.begin(); it != tabs.end(); it++)
	{
		removeTab(indexOf(it.value()));
		infos.remove(it.key());
		delete it.value();
	}
	tabs.clear();
}

/**
 * Slot-function that is called when the user demands to close all tabs
 * but the current tab (and of course the overview).
 */
void PuMP_ImageView::on_closeOthersAction_triggered()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0) return;

	QMap<QString, PuMP_DisplayView *>::iterator it = tabs.begin();
	while(it != tabs.end())
	{
		if((QWidget *) it.value() != cw)
		{
			removeTab(indexOf(it.value()));
			infos.remove(it.key());
			delete it.value();
			it = tabs.erase(it);
		}
		else it++;
	}
}

/*****************************************************************************/
