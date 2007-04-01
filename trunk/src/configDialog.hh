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

#ifndef CONFIGDIALOG_HH_
#define CONFIGDIALOG_HH_

#include <QColor>
#include <QDialog>
#include <QFileInfo>
#include <QSettings>
#include <QWidget>
#include <QListWidget>
#include <QPalette>
#include <QPushButton>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

/* Structs to store config values */
enum exportFormat {
	ZIP_IMAGES,
	ZIP_IMAGES_WITH_PREVIEW,
	HTML_GALLERY,
	PLAIN_FOLDER,
	PLAIN_FOLDER_WITH_PREVIEW
};

typedef struct basicConfig_s {
	enum exportFormat exportFormat;	
	QString currentExportPath;
	
	/* was sonst:
	 * Zielgröße für Bilder, Previews?
	 * 
	 */
} basicConfig_t;

typedef struct advancedConfig_s {
	/* Image Parameters */
	QSize previewSize;
	QSize exportImageSize;
	
} advancedConfig_t;

typedef struct exportConfig_s {
	
} exportConfig_t;

typedef struct configValues_s {
	basicConfig_t bc;
	advancedConfig_t ac;
	exportConfig_t ec;
} configValues_t;




class configDialog : public QDialog
{
	Q_OBJECT
	
public:
	configDialog(QWidget *parent=0);
	virtual ~configDialog();
	
	/* Image parameters */
	void setWatermarkFile(QFileInfo &watermarkFile);
	QFileInfo getWatermarkFile(void);
	void setExportImageSize(QSize imageSize);
	QSize getExportImageSize(void);
	void setWatermarkSize(QSize watermarkSize);
	QSize getWatermarkSize(void);
	void setWatermarkSizeRelative(int percentOfSize=0);
	int getWatermarkSizeRelative(void);
	
	/* Export Parameters */ 
	void setExportFormat(enum exportFormat);
	enum exportFormat getExportFormat(void);
	

public slots:
	void save(void);
	void load(void);
	void changePage(QListWidgetItem *current, QListWidgetItem *previous);
	
private:
	QSettings *PuMPSettings;
	inline void initSettings(void);
	
	QListWidget *configWidget;
	QStackedWidget *pagesWidget;
	void createIcons(void);

};

#endif /*CONFIGDIALOG_HH_*/
