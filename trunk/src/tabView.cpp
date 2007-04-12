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
#include <QMessageBox>

#include "imageView.hh"
#include "mainWindow.hh"
#include "overview.hh"
#include "tabView.hh"

/*****************************************************************************/

/** init static action-pointers */
QAction *PuMP_TabView::closeAllAction = NULL;
QAction *PuMP_TabView::closeOthersAction = NULL;

/**
 * Constructor of class PuMP_TabView which mainly sets up the given
 * overview-tab and connects the needed slots
 * @param	nameFilters	A string containing the supported image-formats the
 * 						application can handle.
 * @param	parent		Pointer on this widgets parent-widget.
 */
PuMP_TabView::PuMP_TabView(QWidget *parent)	: QTabWidget(parent)
{
	overview = new PuMP_Overview(parent);
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
		PuMP_MainWindow::closeAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeAction_triggered()));
	PuMP_TabView::closeAllAction = new QAction("Close all tabs", this);
	connect(
		PuMP_TabView::closeAllAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_closeAllAction_triggered()));
	PuMP_TabView::closeOthersAction = new QAction("Close other tabs", this);
	connect(
		PuMP_TabView::closeOthersAction,
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
		PuMP_MainWindow::nextAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_nextAction()));
	connect(
		PuMP_MainWindow::previousAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_previousAction()));
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
 * Destructor of class PuMP_TabView that clears all data strucutures and
 * removes all tabs.
 */
PuMP_TabView::~PuMP_TabView()
{
	delete PuMP_TabView::closeAllAction;
	delete PuMP_TabView::closeOthersAction;
	
	removeTab(indexOf(overview));
	delete overview;

	while(count() > 0) removeTab(0);

	QMap<QString, PuMP_ImageView *>::iterator it = tabs.begin();
	while(!tabs.isEmpty())
	{
		delete it.value();
		tabs.erase(it);
		it = tabs.begin();
	}

	infos.clear();
}

/**
 * Overloaded function for context-menu-events. It provides a custom menu for
 * the overview and the other tabs.
 * @param	e The context-menu-event. 
 */
void PuMP_TabView::contextMenuEvent(QContextMenuEvent *e)
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0) return;
	
	PuMP_MainWindow::closeAction->setEnabled(true);
	PuMP_TabView::closeAllAction->setEnabled(true);
	PuMP_TabView::closeOthersAction->setEnabled(true);
	if(cw == (QWidget *) overview)
		PuMP_MainWindow::closeAction->setEnabled(false);

	QMenu menu(this);
	menu.addAction(PuMP_MainWindow::closeAction);
	menu.addSeparator();
	menu.addAction(PuMP_TabView::closeAllAction);
	menu.addAction(PuMP_TabView::closeOthersAction);
	menu.exec(e->globalPos());
}

/**
 * Slot-function that is called when the user demands to close the current tab.
 * The overview can't be closed.
 */
void PuMP_TabView::on_closeAction_triggered()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0) return;

	QMap<QString, PuMP_ImageView *>::iterator it = tabs.begin();
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
 * @param	index The index of the new current tab.
 */
void PuMP_TabView::on_currentChanged(int index)
{
	QWidget *cw = widget(index);
	if(cw == NULL || tabs.size() == 0 || cw == overview)
	{
		bool enable = (tabs.size() != 0);
		PuMP_MainWindow::closeAction->setEnabled(false);
		PuMP_TabView::closeAllAction->setEnabled(enable);
		PuMP_TabView::closeOthersAction->setEnabled(enable);
		PuMP_MainWindow::mirrorHAction->setEnabled(false);
		PuMP_MainWindow::mirrorVAction->setEnabled(false);
		PuMP_MainWindow::nextAction->setEnabled(false);
		PuMP_MainWindow::previousAction->setEnabled(false);
		PuMP_MainWindow::rotateCWAction->setEnabled(false);
		PuMP_MainWindow::rotateCCWAction->setEnabled(false);
		PuMP_MainWindow::sizeOriginalAction->setEnabled(false);
		PuMP_MainWindow::sizeFittedAction->setEnabled(false);
		PuMP_MainWindow::zoomInAction->setEnabled(false);
		PuMP_MainWindow::zoomOutAction->setEnabled(false);
	}
	else
	{
		PuMP_ImageView *view = (PuMP_ImageView *) cw;
		PuMP_TabView::closeAllAction->setEnabled(true);
		PuMP_TabView::closeOthersAction->setEnabled((tabs.size() != 1));
		view->setActions();
	}
}

/**
 * Slot-function that is called when an image-view failed to load its image.
 * @param	view	A pointer to the image-view that failed.
 */
void PuMP_TabView::on_error(PuMP_ImageView *view)
{
	if(view == NULL) return;

	QMessageBox::information(
		this,
		"Information",
		"Failed to load \"" + view->fileName() + "\"");
	
	QMap<QString, PuMP_ImageView *>::iterator it = tabs.begin();
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
void PuMP_TabView::on_mirrorHAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::MirrorHorizontally);
}

/**
 * Slot-function that is called to mirror the current image vertically.
 */
void PuMP_TabView::on_mirrorVAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::MirrorVertically);
}

/**
 * Slot-function that is called when the go-to-next-image-action was triggered.
 */
void PuMP_TabView::on_nextAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	QFileInfo info = view->getSuccessor();
	on_openImage(info, false);
}

/**
 * Slot-function that is called when the go-to-previous-image-action
 * was triggered.
 */
void PuMP_TabView::on_previousAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	QFileInfo info = view->getSuccessor(true);
	on_openImage(info, false);
}

/**
 * Slot-function that displays the given image in the current-tab or in a
 * new tab.
 * @param	info	The image to load.
 * @param	newTab	Flag indicating if the image will be opened in a new tab.
 */
void PuMP_TabView::on_openImage(const QFileInfo &info, bool newTab)
{
	if(info.isDir() || !info.exists())
	{
		QMessageBox::information(
			this,
			"Information",
			"Cannot show \"" + info.fileName() + "\"");
		return;
	}
	
	QMap<QString, PuMP_ImageView *>::iterator it;
	it = tabs.find(info.filePath());
	if(it != tabs.end())
	{
		setCurrentWidget(it.value());
		return;
	}

	if(tabs.size() == 0 || newTab)
	{
		PuMP_ImageView *view = new PuMP_ImageView(this);
		connect(
			view,
			SIGNAL(error(PuMP_ImageView *)),
			this,
			SLOT(on_error(PuMP_ImageView *)));
		view->process(PuMP_ImageView::LoadImage, info);

		tabs.insert(info.filePath(), view);
		infos.insert(info.filePath(), info);

		int index = addTab(view, info.fileName());
		setTabToolTip(index, info.filePath());
		setCurrentIndex(index);
	}
	else
	{
		int index = currentIndex();
		if(index == 0) index = 1;

		PuMP_ImageView *view = (PuMP_ImageView *) widget(index);
		if(view == NULL)
		{
			QMessageBox::information(
				this,
				"Information",
				"Cannot show \"" + info.fileName() + "\"");
			return;
		}
		
		QMap<QString, QFileInfo>::iterator it1 = infos.find(view->filePath());
		if(it1 != infos.end()) infos.erase(it1);
		else qDebug() << "error couldn't find" << view->filePath();
		infos.insert(info.filePath(), info);
		
		QMap<QString, PuMP_ImageView *>::iterator it2;
		it2 = tabs.find(view->filePath());
		if(it2 != tabs.end()) tabs.erase(it2);
		else qDebug() << "error couldn't find" << view->filePath();
		tabs.insert(info.filePath(), view);
		
		view->process(PuMP_ImageView::LoadImage, info);
		setTabText(index, info.fileName());
		setTabToolTip(index, info.filePath());
		setCurrentIndex(index);
	}
}

/**
 * Slot-function that is called to rotate the current image clockwise by
 * 90 degrees.
 */
void PuMP_TabView::on_rotateCWAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::RotateClockWise);
}

/**
 * Slot-function that is called to rotate the current image
 * counter-clockwise by 90 degrees.
 */
void PuMP_TabView::on_rotateCCWAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;

	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::RotateCounterClockWise);
}

/**
 * Slot-function that shows the current image in original size.
 */
void PuMP_TabView::on_sizeOriginalAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;

	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::ResizeToOriginal);
}

/**
 * Slot-function that shows the current image fitted to the displays size.
 */
void PuMP_TabView::on_sizeFittedAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;

	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::ResizeToFitted);
}

/**
 * Slot-function that is called to zoom into the current image.
 */
void PuMP_TabView::on_zoomInAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::ZoomIn);
}

/**
 * Slot-function that is called to zoom out of the current image.
 */
void PuMP_TabView::on_zoomOutAction()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	PuMP_ImageView *view = (PuMP_ImageView *) cw;
	view->process(PuMP_ImageView::ZoomOut);
}

/**
 * Slot-function that enables all components of this widget to send
 * messages to the main-window's status-bar.
 * @param	value	The value for the status-bar's progress-bar.
 * @param	text	The message displayed at the status-bar.
 */
void PuMP_TabView::on_updateStatusBar(int value, const QString &text)
{
	emit updateStatusBar(value, text);
}

/**
 * Slot-function that is called when the user demands to close all tabs.
 * The overview can't be closed.
 */
void PuMP_TabView::on_closeAllAction_triggered()
{
	QMap<QString, PuMP_ImageView *>::iterator it;
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
void PuMP_TabView::on_closeOthersAction_triggered()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0) return;

	QMap<QString, PuMP_ImageView *>::iterator it = tabs.begin();
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
