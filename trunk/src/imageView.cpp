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
		SIGNAL(viewerRequested(const QFileInfo &, bool)),
		this,
		SLOT(display(const QFileInfo &, bool)));
	connect(
		overview,
		SIGNAL(updateStatusBar(int, const QString &)),
		this,
		SLOT(on_updateStatusBar(int, const QString &)));
	addTab(overview, "Overview");
	
	closeAction = new QAction("Close tab", this);
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
}

/**
 * Destructor of class PuMP_ImageView that clears all data strucutures and
 * removes all tabs.
 */
PuMP_ImageView::~PuMP_ImageView()
{
	delete closeAction;
	delete closeAllAction;
	delete closeOthersAction;
	
	delete overview;
	
	QMap<QString, PuMP_DisplayView *>::iterator it;
	for(it = tabs.begin(); it != tabs.end(); it++)
	{
		removeTab(indexOf(it.value()));
		infos.remove(it.key());
		delete it.value();
	}
	tabs.clear();
	
	removeTab(indexOf(overview));
}

/**
 * Slot-function that displays the given image in the current-tab or in a
 * new tab.
 * @param	info	The image to load.
 * @param	newTab	Flag indicating if the image will be opened in a new tab.
 */
void PuMP_ImageView::display(const QFileInfo &info, bool newTab)
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

		addTab(view, info.fileName());
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
		
		setCurrentWidget(view);
	}
}

/**
 * Slot-function that is called to request an overview.
 * @param	info	File-info-object representing the folder to overview.
 */
void PuMP_ImageView::createOverview(const QFileInfo &info)
{
	overview->create(info);
}

/**
 * Slot-function that is called to mirror the current image.
 * @param	horizontal	Flag whether the image is mirrored horizontal or not.
 */
void PuMP_ImageView::mirror(bool horizontal)
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	((PuMP_DisplayView *) cw)->mirror(horizontal);
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
 * Slot-function that is called to rotate the current image by 90 degrees.
 * @param	clockwise	Flag indicating the direction of the rotation.
 */
void PuMP_ImageView::rotate(bool clockwise)
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	((PuMP_DisplayView *) cw)->rotate(clockwise);
}

/**
 * Slot-function that is called to stop the request of an overview.
 */
void PuMP_ImageView::stopCreateOverview()
{
	overview->stop();
}

/**
 * Slot-function that is called to zoom into the current image.
 */
void PuMP_ImageView::zoomIn()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	((PuMP_DisplayView *) cw)->zoomIn();
}

/**
 * Slot-function that is called to zoom out of the current image.
 */
void PuMP_ImageView::zoomOut()
{
	QWidget *cw = currentWidget();
	if(cw == NULL || tabs.size() == 0 || cw == overview) return;
	
	((PuMP_DisplayView *) cw)->zoomOut();
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
