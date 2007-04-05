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
#include "mainWindow.hh"
#include "overview.hh"

/*****************************************************************************/

/** init static action-pointers */
QAction *PuMP_ImageView::closeAllAction = NULL;
QAction *PuMP_ImageView::closeOthersAction = NULL;

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

	connect(
		PuMP_MainWindow::addAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_addAction_triggered()));
	connect(
		PuMP_MainWindow::closeAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeAction_triggered()));
	PuMP_ImageView::closeAllAction = new QAction("Close all tabs", this);
	connect(
		PuMP_ImageView::closeAllAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeAllAction_triggered()));
	PuMP_ImageView::closeOthersAction = new QAction("Close other tabs", this);
	connect(
		PuMP_ImageView::closeOthersAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeOthersAction_triggered()));
	connect(
		PuMP_MainWindow::mirrorHAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_mirrorHAction()));
	connect(
		PuMP_MainWindow::mirrorVAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_mirrorVAction()));
	connect(
		PuMP_MainWindow::rotateCWAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_rotateCWAction()));
	connect(
		PuMP_MainWindow::rotateCCWAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_rotateCCWAction()));
	connect(
		PuMP_MainWindow::sizeOriginalAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_sizeOriginalAction()));
	connect(
		PuMP_MainWindow::sizeFittedAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_sizeFittedAction()));
	connect(
		PuMP_MainWindow::zoomInAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_zoomInAction()));
	connect(
		PuMP_MainWindow::zoomOutAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_zoomOutAction()));
}

/**
 * Destructor of class PuMP_ImageView that clears all data strucutures and
 * removes all tabs.
 */
PuMP_ImageView::~PuMP_ImageView()
{
	delete PuMP_ImageView::closeAllAction;
	delete PuMP_ImageView::closeOthersAction;
	
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
	
	PuMP_MainWindow::closeAction->setEnabled(true);
	PuMP_ImageView::closeAllAction->setEnabled(true);
	PuMP_ImageView::closeOthersAction->setEnabled(true);
	if(cw == (QWidget *) overview)
		PuMP_MainWindow::closeAction->setEnabled(false);

	QMenu menu(this);
	menu.addAction(PuMP_MainWindow::closeAction);
	menu.addSeparator();
	menu.addAction(PuMP_ImageView::closeAllAction);
	menu.addAction(PuMP_ImageView::closeOthersAction);
	menu.exec(e->globalPos());
}

/**
 * Function that zooms the given display's image to the given step.
 * @param	view	The display to zoom.
 * @param	step	The zoom-step.
 */
void PuMP_ImageView::zoom(PuMP_DisplayView *view, int step)
{
	if(view == NULL ||
		step > MAX_ZOOM_STEPS ||
		step < 0 ||
		view->display.zoom == step) return;
	
	QImage temp = view->display.image.mirrored(
		view->display.mirroredHorizontal,
		view->display.mirroredVertical);

	QMatrix matrix;
	matrix.rotate(view->display.rotation);
	temp = temp.transformed(matrix, Qt::SmoothTransformation);

	view->display.zoom = step;
	double factor = view->display.zoom - DEFAULT_ZOOM;
	if(factor < 0) factor /= (int)(MAX_ZOOM_STEPS / 2) + 1;
	
	PuMP_MainWindow::zoomInAction->setEnabled(true);
	PuMP_MainWindow::zoomOutAction->setEnabled(true);
	if(view->display.zoom == MAX_ZOOM_STEPS)
		PuMP_MainWindow::zoomInAction->setEnabled(false);
	if(view->display.zoom == 0)
		PuMP_MainWindow::zoomOutAction->setEnabled(false);
	
	matrix.reset();
	matrix.scale(1 + factor, 1 + factor);

	view->display.displayed = QPixmap::fromImage(
		temp.transformed(matrix,
		Qt::SmoothTransformation));
	view->display.scaled = false;
	view->display.adjustSize();
	view->display.update();
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
		PuMP_MainWindow::addAction->setEnabled(true);
		PuMP_MainWindow::closeAction->setEnabled(false);
		PuMP_ImageView::closeAllAction->setEnabled(enable);
		PuMP_ImageView::closeOthersAction->setEnabled(enable);
		PuMP_MainWindow::mirrorHAction->setEnabled(false);
		PuMP_MainWindow::mirrorVAction->setEnabled(false);
		PuMP_MainWindow::rotateCWAction->setEnabled(false);
		PuMP_MainWindow::rotateCCWAction->setEnabled(false);
		PuMP_MainWindow::sizeOriginalAction->setEnabled(false);
		PuMP_MainWindow::sizeFittedAction->setEnabled(false);
		PuMP_MainWindow::zoomInAction->setEnabled(false);
		PuMP_MainWindow::zoomOutAction->setEnabled(false);
	}
	else
	{
		bool enable = (tabs.size() != 1);
		PuMP_DisplayView *view = (PuMP_DisplayView *) cw;
		
		PuMP_MainWindow::addAction->setEnabled(true);
		PuMP_MainWindow::closeAction->setEnabled(true);
		PuMP_ImageView::closeAllAction->setEnabled(true);
		PuMP_ImageView::closeOthersAction->setEnabled(enable);
		PuMP_MainWindow::mirrorHAction->setEnabled(true);
		PuMP_MainWindow::mirrorVAction->setEnabled(true);
		PuMP_MainWindow::rotateCWAction->setEnabled(true);
		PuMP_MainWindow::rotateCCWAction->setEnabled(true);
		PuMP_MainWindow::sizeOriginalAction->setEnabled(view->display.scaled);
		PuMP_MainWindow::sizeFittedAction->setEnabled(!view->display.scaled);
		PuMP_MainWindow::zoomInAction->setEnabled(
			(view->display.zoom < MAX_ZOOM_STEPS));
		PuMP_MainWindow::zoomOutAction->setEnabled(
			(view->display.zoom > 0));
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

	PuMP_MainWindow::sizeOriginalAction->setEnabled(false);
	PuMP_MainWindow::sizeFittedAction->setEnabled(true);

	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;

	QImage temp = view->display.image.mirrored(
		view->display.mirroredHorizontal,
		view->display.mirroredVertical);

	QMatrix matrix;
	matrix.rotate(view->display.rotation);
	temp = temp.transformed(matrix, Qt::SmoothTransformation);

	view->display.displayed = QPixmap::fromImage(temp);
	view->display.zoom = DEFAULT_ZOOM;
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

	PuMP_MainWindow::sizeOriginalAction->setEnabled(true);
	PuMP_MainWindow::sizeFittedAction->setEnabled(false);

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
	if(factor < 1) factor *= (int)(MAX_ZOOM_STEPS / 2) + 1;

	view->display.zoom = ((int) factor) + DEFAULT_ZOOM - 1;
	view->display.scaled = true;
	
	PuMP_MainWindow::zoomInAction->setEnabled(true);
	if(view->display.zoom == 0)
		PuMP_MainWindow::zoomOutAction->setEnabled(false);

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
	zoom(view, view->display.zoom + 1);
}

/**
 * Slot-function that is called to zoom out of the current image.
 */
void PuMP_ImageView::on_zoomOutAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_DisplayView *view = (PuMP_DisplayView *) cw;
	zoom(view, view->display.zoom - 1);
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
