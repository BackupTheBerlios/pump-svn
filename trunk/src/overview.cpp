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
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

#include "directoryView.hh"
#include "mainWindow.hh"
#include "overview.hh"

/*****************************************************************************/

/**
 * Constructor of class PuMP_OverviewModel.
 * @param	fontMetrics	Pointer on the font-metric of the parent-widget.
 */
PuMP_OverviewModel::PuMP_OverviewModel(
	QFontMetrics *fontMetrics,
	QObject *parent)
	: QAbstractListModel(parent)
{
	if(fontMetrics == NULL) this->fontMetrics = NULL;
	else this->fontMetrics = new QFontMetrics(*fontMetrics);
}

/**
 * Destructor of class PuMP_OverviewModel.
 */
PuMP_OverviewModel::~PuMP_OverviewModel()
{
	if(fontMetrics != NULL) delete fontMetrics;
}

/**
 * Function that adds an image with its properties to this model. Duplicates
 * will be handled.
 * @param	pixmap	The (scaled) image.
 * @param	name	The image's file-name.
 * @param	props	The image's property-string (size and dimensions).
 */
void PuMP_OverviewModel::addImage(
	const QPixmap &pixmap,
	const QString &name,
	const QString &props)
{
	if(getRowFromName(name) != -1) return;
	int row = pixmaps.size();
	
	beginInsertRows(QModelIndex(), row, row);
	pixmaps.insert(row, pixmap);
	names.insert(row, name);
	properties.insert(row, props);
	endInsertRows();
}

/**
 * Function that clears all entries in the model.
 */
void PuMP_OverviewModel::clear()
{
	QModelIndex dummy;
	removeRows(0, rowCount(dummy), dummy);
}

/**
 * Function that returns the data of a certain item in the model. Usually only
 * called from the view displaying the model.
 * @param	index	The index of the demanded item.
 * @param	role	The role defining which data will be returned.
 * @return	The queried data or an empty QVariant if none is available.
 */
QVariant PuMP_OverviewModel::data(
	const QModelIndex &index,
	int role) const
{
	if(!index.isValid()) return QVariant();
	
	if(role == Qt::DecorationRole) return QIcon(pixmaps.value(index.row()));
	else if(role == Qt::DisplayRole)
	{
		QString display = names.value(index.row());
		if(fontMetrics != NULL)
		{
			display = fontMetrics->elidedText(
				display,
				Qt::ElideRight,
				(int)((ITEM_STRETCH - 1) * THUMB_SIZE) - ICON_PADDING);
		}

		QString props = properties.value(index.row());
		if(!props.isEmpty()) display += "\n" + props;

		return display;
	}
	else if(role == Qt::SizeHintRole)
	{
		QSize size(
			(int)(ITEM_STRETCH * THUMB_SIZE),
			THUMB_SIZE + ITEM_SPACING);
		return size;
	}
//	else if(role == Qt::ToolTipRole)

	return QVariant();
}

/**
 * Function that returns flags defining the behaviour of an item of the model.
 * In our case the flags are always the same and indicate, that items are
 * selectible and enabled.
 * @param	index	The index of the demanded item.
 * @return	The flags defining the behaviour of the queried item.
 */
Qt::ItemFlags PuMP_OverviewModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index);
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/**
 * Function that returns the file-name of the given item.
 * @param	index	The index of the item, the name will be fetched.
 * @return	A QString containing the file-name for the index, an empty QString
 * 			else.
 */
QString PuMP_OverviewModel::getFileName(const QModelIndex &index) const
{
	if(!index.isValid()) return QString();
	return names.value(index.row());
}

/**
 * Function that returns the index or row of a certain item identified by its
 * file-name.
 * @param	name	The file-name of the queried item.
 * @return	The index of the demanded item, or -1 if it wasn't found.
 */
int PuMP_OverviewModel::getRowFromName(const QString &name) const
{
	return names.indexOf(name);
}

/**
 * Function that removes count items starting from index row.
 * @param	row		The index of the first item to remove.
 * @param	count	The number of items to remove.
 * @param	parent	Not needed.
 * @return	True on success, false otherwise. 
 */
bool PuMP_OverviewModel::removeRows(
	int row,
	int count,
	const QModelIndex &parent)
{
	if(parent.isValid()) return false;
	
	if(row >= pixmaps.size() || row + count <= 0) return false;

	int begin = qMax(0, row);
	int end = qMin(row + count - 1, pixmaps.size() - 1);

	beginRemoveRows(parent, begin, end);
	while(end >= begin)
	{
		pixmaps.removeAt(end);
		names.removeAt(end);
		properties.removeAt(end);
		end--;
	}
	endRemoveRows();
	return true;
}

/**
 * Function that returns the number of items stored in this model.
 * @param	parent	Node needed.
 * @return	The number of items stored in this model.
 */
int PuMP_OverviewModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return pixmaps.size();
}

/**
 * Function to set the used fontMetrics.
 * @param	fontMetrics	The font Metric of the parent-widget.	
 */
void PuMP_OverviewModel::setFontMetrics(const QFontMetrics &fontMetrics)
{
	if(this->fontMetrics == NULL)
		this->fontMetrics = new QFontMetrics(fontMetrics);
}

/*****************************************************************************/

/**
 * Constructor of class PuMP_OverviewLoader. The thread processes the given
 * images (represented by a QFileInfo-Object). It returns a scaled version of
 * the image along with its file-name and a property-string.
 * @param	parent	The parent-object of this thread.
 */
PuMP_OverviewLoader::PuMP_OverviewLoader(QObject *parent) : QThread(parent)
{
	killed = false;
	size.setWidth(THUMB_SIZE);
	size.setHeight(THUMB_SIZE);
}

/**
 * The overloaded main-function of this thread, which processes the given
 * image.
 */
void PuMP_OverviewLoader::run()
{
	killed = false;
	
	rProperties = "";
	rName = "";
	
	if(!info.isDir())
	{
		rSize = reader.size();
		rProperties += QString::number(rSize.width())
			+ "x"
			+ QString::number(rSize.height())
			+ "\n"
			+ QString::number(((double) info.size()) / 1024, 'f', 1)
			+ " KB";

		if(rSize.height() > size.height() || rSize.width() > size.width())
		{
			if(rSize.height() > rSize.width())
				size.setWidth(
					(THUMB_SIZE * 100 / rSize.height()) *
					rSize.width() / 100);
			else size.setHeight(
				(THUMB_SIZE * 100 / rSize.width()) *
				rSize.height() / 100);
			
			reader.setScaledSize(size);
		}
		
		if(!reader.canRead())
		{
			emit imageIsNull(info.fileName());
			return;
		}

		result = reader.read();
	}
	else result.load(":/folder64.png");
	
	rName = info.fileName();
	if(result.isNull())
	{
		emit imageIsNull(info.fileName());
		return;
	}
	
	if(result.width() < THUMB_SIZE || result.height() < THUMB_SIZE)
	{
		QImage master(THUMB_SIZE, THUMB_SIZE, QImage::Format_ARGB32);
		painter.begin(&master);
		painter.fillRect(0, 0, THUMB_SIZE, THUMB_SIZE, QBrush(Qt::white));
		painter.drawImage(
			QPoint((THUMB_SIZE - result.width()) / 2,
				(THUMB_SIZE - result.height()) / 2),
			result);
		painter.setPen(QPen(QBrush(Qt::lightGray), 1));
		painter.drawRect(0, 0, THUMB_SIZE - 1, THUMB_SIZE - 1);
		painter.end();

		result = master;
	}
	
	emit processedImage(rName, rProperties, result, killed);
}

/**
 * Function that can be called from the outside of this class to let the
 * thread process the given image.
 * @param	fileinfo	The QFileInfo-Object representing the image to
 * 						process. 
 */
void PuMP_OverviewLoader::processImage(const QFileInfo &fileInfo)
{
	info = fileInfo;
	reader.setFileName(info.filePath());
	reader.setScaledSize(QSize());

	size.setWidth(THUMB_SIZE);
	size.setHeight(THUMB_SIZE);

	start();
}

/**
 * Function to mark the current execution of this thread as killed.
 * Needed for a stop-function.
 */
void PuMP_OverviewLoader::setKilled()
{
	killed = true;
}

/**
 * Function returns whether the last execution of this thread was killed.
 * @return	True if the execution was killed, false otherwise.
 */
bool PuMP_OverviewLoader::wasKilled()
{
	return killed;
}

/*****************************************************************************/

/** init static actions */
QAction *PuMP_Overview::openAction = NULL;

/**
 * Constructor of class PuMP_Overview which is basically a QListView to
 * display the PuMP_OverviewModel.
 * @param	nameFilters	A string containing the supported image-formats the
 * 						application can handle.
 * @param	parent		The parent-widget of this object.
 */
PuMP_Overview::PuMP_Overview(QWidget *parent)
	: QListView(parent), PuMP_SettingsInterface()
{
	progress = 0;
	progressMax = 1;

	PuMP_Overview::openAction = new QAction("Open", this);
	connect(
		PuMP_Overview::openAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_openAction_triggered()));
	connect(
		PuMP_MainWindow::openInNewTabAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_openInNewTabAction_triggered()));
	connect(
		PuMP_MainWindow::refreshAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_refresh()));
	connect(
		PuMP_MainWindow::stopAction,
		SIGNAL(triggered()),
		this,
		SLOT(on_stop()));

	model.setParent(this);
	model.setFontMetrics(fontMetrics());
	loader.setParent(this);
	connect(&loader, SIGNAL(finished()), this, SLOT(on_loader_finished()));
	connect(
		&loader,
		SIGNAL(imageIsNull(const QString &)),
		this,
		SLOT(on_loader_imageIsNull(const QString &)));
	connect(
		&loader,
		SIGNAL(processedImage(
			const QString &,
			const QString &,
			const QImage &,
			bool)),
		this,
		SLOT(on_loader_processedImage(
			const QString &,
			const QString &,
			const QImage &,
			bool)));
	
	dir.setNameFilters(PuMP_MainWindow::nameFilters);
	
	setViewMode(QListView::ListMode);
	setFlow(QListView::LeftToRight);
	setFocusPolicy(Qt::StrongFocus);
	setIconSize(QSize(THUMB_SIZE, THUMB_SIZE));
	setLayoutMode(QListView::Batched);
	setMinimumWidth(450);
	setModel(&model);
	setMovement(QListView::Static);
	setResizeMode(QListView::Fixed);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setTextElideMode(Qt::ElideNone);
	setWordWrap(true);
	setWrapping(true);
	setUniformItemSizes(true);
	connect(
		this,
		SIGNAL(activated(const QModelIndex &)),
		this,
		SLOT(on_activated(const QModelIndex &)));
	
	loadSettings();
	PuMP_Overview::openAction->setData(dirFromSettings);
	PuMP_DirectoryView::openAction->setData(dirFromSettings);
	PuMP_Overview::openAction->trigger();
	PuMP_DirectoryView::openAction->trigger();
}

/**
 * Destructor of class PuMP_Overview that simply empties all lists, discards
 * the content of the model and stops the thread if running.
 */
PuMP_Overview::~PuMP_Overview()
{
	delete PuMP_Overview::openAction;

	storeSettings();
	on_stop();
	model.clear();
}

/**
 * Function that loads the formerly stored settings for this class.
 */
void PuMP_Overview::loadSettings()
{
	dirFromSettings = PuMP_MainWindow::settings->value(
		PUMP_OVERVIEW_DIR,
		QDir::homePath()).toString();
}

/**
 * Function that stores all settings for this class.
 */
void PuMP_Overview::storeSettings()
{
	PuMP_MainWindow::settings->setValue(PUMP_OVERVIEW_DIR, dir.path());
}

/**
 * Function that saves a selected image under another name.
 */
void PuMP_Overview::save()
{
	QModelIndex index = currentIndex();
	if(!index.isValid()) return;

	QFileInfo info(dir.absoluteFilePath(model.getFileName(index)));
	if(!info.exists()) return;
	
	QImage toSave(dir.absoluteFilePath(model.getFileName(index)));
	if(toSave.isNull()) return;

	QString oldExt = info.completeSuffix();
	QString newExt = oldExt;
	if(!newExt.isEmpty()) newExt.prepend("*.");

	QString fpath = QFileDialog::getSaveFileName(
		this,
		QString("Save ") + info.fileName() + " as",
		QString(QDir::homePath()) + "/" + info.fileName(),
		PuMP_MainWindow::nameFilterString2,
		&newExt);
	if(fpath.isEmpty() || newExt.isEmpty()) return;
	
	newExt.remove(0, 2);
	if(fpath.endsWith(newExt)) newExt.clear();
	else if(fpath.endsWith(oldExt))
	{
		fpath.chop(oldExt.length());
		fpath += newExt;
	}
	else fpath += "." + newExt;
	
	qDebug() << fpath << oldExt << newExt;

	const char *ext = NULL;
	if(!newExt.isEmpty()) ext = newExt.toAscii().data();
	if(!toSave.save(fpath, ext))
	{
		QMessageBox::information(
			this,
			"Information",
			"Failed to save \"" + fpath + "\"");
		return;
	}
}

/**
 * Overloaded function for context-menu-events. It provides a custom menu for
 * directories and files.
 * @param	e The context-menu-event. 
 */
void PuMP_Overview::contextMenuEvent(QContextMenuEvent *e)
{
	bool show = true;
	QModelIndex index = currentIndex();
	if(!index.isValid()) show = false;
	
	QFileInfo info(dir.absoluteFilePath(model.getFileName(index)));
	if(!info.exists()) show = false;
	
	PuMP_MainWindow::saveAsAction->setEnabled(!info.isDir() && show);
	PuMP_MainWindow::exportAction->setEnabled(show);
	PuMP_Overview::openAction->setEnabled(show);
	PuMP_MainWindow::openInNewTabAction->setEnabled(!info.isDir() && show);
	PuMP_MainWindow::refreshAction->setEnabled(dir.exists());

	QMenu menu(this);
	menu.addAction(PuMP_Overview::openAction);
	menu.addAction(PuMP_MainWindow::openInNewTabAction);
	menu.addSeparator();
	menu.addAction(PuMP_MainWindow::saveAsAction);
	menu.addAction(PuMP_MainWindow::exportAction);
	menu.addSeparator();
	menu.addAction(PuMP_MainWindow::refreshAction);
	menu.addAction(PuMP_MainWindow::stopAction);
	menu.exec(e->globalPos());
}

/**
 * Slot-function that is called when the current index changes.
 * @param	current		The new current index.
 * @param	previous	The old index.
 */
void PuMP_Overview::currentChanged(
	const QModelIndex &current,
	const QModelIndex &previous)
{
	Q_UNUSED(previous);
	QFileInfo info(dir.absoluteFilePath(model.getFileName(current)));
	PuMP_MainWindow::exportAction->setEnabled(info.exists());
	PuMP_MainWindow::openInNewTabAction->setEnabled(
		info.exists() && !info.isDir());
}

/**
 * Slot-function that is called when an entry in this list-view was activated.
 * @param	index	The index of the activated item.
 * @param	newTab	Indicates whether the file should be opened in a new tab.
 */
void PuMP_Overview::on_activated(const QModelIndex &index, bool newTab)
{
	if(!index.isValid()) return;
	
	QFileInfo info(dir.absoluteFilePath(model.getFileName(index)));
	if(!info.exists()) return;
	
	if(info.isDir())
	{
		on_open(info);
		PuMP_DirectoryView::openAction->setData(info.filePath());
		PuMP_DirectoryView::openAction->trigger();
	}
	else emit openImage(info, newTab);
}

/**
 * Slot-function that is called when the thread processing the preview-images
 * returns from its main-funtion. If necessary, a the next execution will be
 * scheduled here.
 */
void PuMP_Overview::on_loader_finished()
{
	progress++;

	if(!current.isEmpty())
	{
		QFileInfo info = current.takeFirst();
		emit updateStatusBar(progress * 100 / progressMax, info.fileName());

		loader.processImage(info);
	}
	else
	{
		PuMP_MainWindow::refreshAction->setEnabled(true);
		PuMP_MainWindow::stopAction->setEnabled(false);
		emit updateStatusBar(100, QString());
	}
}

/**
 * Slot-function that is called when the picture in a thread couldn't get
 * loaded or scaled.
 * @param	info	The file that failed to load/scale.
 */
void PuMP_Overview::on_loader_imageIsNull(const QString &fileName)
{
	QMessageBox::information(
		this,
		"Information",
		"Cannot create thumbnail for entry \"" + fileName + "\"");
}

/**
 * Slot-function that is called when the thread finished the processing of an
 * image. The image will then be inserted into the model and so be displayed.
 * @param	name		The file-name of the image that was processed.
 * @param	properties	The property-string of the image.
 * @param	image		The scaled image itself.
 * @param	wasKilled	Flag indicating whether the thread was killed or not.
 */
void PuMP_Overview::on_loader_processedImage(
	const QString &name,
	const QString &properties,
	const QImage &image,
	bool wasKilled)
{
	if(!wasKilled)
	{
		model.addImage(QPixmap::fromImage(image), name, properties);
		update();
	}
}

/**
 * Function that creates an overview/preview of the given directory.
 * @param	info	The QFileInfo-object representing the directory to preview.
 */
void PuMP_Overview::on_open(const QFileInfo &info)
{
	if(!info.isDir() || !info.isAbsolute())
	{
		QMessageBox::information(
			this,
			"Information",
			"Cannot create overview for \"" + info.fileName() + "\"");
		return;
	}

	if(dir.path() != info.filePath())
	{
		on_stop();
		model.clear();

		dir.setPath(info.filePath());
		dir.refresh();
		current = dir.entryInfoList(
			QDir::Files | QDir::AllDirs |
			QDir::NoDotAndDotDot | QDir::Readable,
			QDir::Name | QDir::DirsFirst);
	
		progress = 0;
		progressMax = 1;
		
		if(!current.isEmpty())
		{
			PuMP_MainWindow::refreshAction->setEnabled(false);
			PuMP_MainWindow::stopAction->setEnabled(true);
			progressMax = current.size();
			QFileInfo first = current.first();
			emit updateStatusBar(
				progress * 100 / progressMax,
				first.fileName());
			
			if(!loader.isRunning())
			{
				first = current.takeFirst();
				loader.processImage(first);
			}
		}
	}
}

/**
 * Slot-function that is called when the user demanded to open a selected file
 * or directory.
 */
void PuMP_Overview::on_openAction_triggered()
{
	QVariant v = PuMP_Overview::openAction->data();
	if(v.isValid())
	{
		QFileInfo f(v.toString());
		PuMP_Overview::openAction->setData(QVariant());
		if(f.exists() && f.isDir()) on_open(f);
	}
	else
	{
		QModelIndex index = currentIndex();
		on_activated(index, false);
	}
}

/**
 * Slot-function that is called when the user demanded to open a selected
 * file in a new tab.
 */
void PuMP_Overview::on_openInNewTabAction_triggered()
{
	int i = 0;
	QList<QModelIndex> selected = selectedIndexes();
	for(i = 0; i < selected.size(); i++) on_activated(selected.at(i), true);
}

/**
 * Slot-function that refreshes and reloads the current directory.
 */
void PuMP_Overview::on_refresh()
{
	on_stop();
	if(dir.exists())
	{
		QFileInfo info(dir.path());
		dir.setPath("");
		
		on_open(info);
	}
}

/**
 * Function that stops a running calcultion of a preview (e.g. if the directory
 * is too large or the user is too impatient).
 */
void PuMP_Overview::on_stop()
{
	PuMP_MainWindow::stopAction->setEnabled(false);
	
	progress = 0;
	progressMax = 1;
	current.clear();
	emit updateStatusBar(100, QString());

	loader.setKilled();
}

/*****************************************************************************/
