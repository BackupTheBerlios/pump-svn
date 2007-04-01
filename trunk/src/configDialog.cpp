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

#include "configDialog.hh"
#include "configPages.hh"

configDialog::configDialog(QWidget *parent) : QDialog(parent)
{
	qDebug() << "configDialog();";
	
	initSettings();
	
	/* List for icons to choose a configuration page */
	configWidget = new QListWidget;
	
	//configWidget->setBackgroundRole(QPalette::Dark);
	configWidget->setViewMode(QListView::ListMode);
	configWidget->setIconSize(QSize(48,48));
	configWidget->setMovement(QListView::Static);
	configWidget->setUniformItemSizes(true);
	configWidget->setMaximumWidth(200);
	configWidget->setSpacing(12);
	

	
	/* Stacked actual configuration pages */
	pagesWidget = new QStackedWidget;
	pagesWidget->addWidget(new BasicConfiguration());
	pagesWidget->addWidget(new ExportConfiguration());
	pagesWidget->addWidget(new AdvancedConfiguration());
	pagesWidget->addWidget(new AboutPage());
	
	
	createIcons();
	configWidget->setCurrentRow(0);
	
	/* Save, reload and close buttons */
	QPushButton *closeConfig = new QPushButton(tr("Close"));
	closeConfig->setToolTip(tr("Close the configuration dialog without saving"));
	connect(closeConfig, SIGNAL(clicked()), this, SLOT(close()));
	QPushButton *saveConfig = new QPushButton(tr("Save"));
	saveConfig->setToolTip(tr("Save current configuration"));
	connect(saveConfig, SIGNAL(clicked()), this, SLOT(save()));
	QPushButton *restoreConfig = new QPushButton(tr("Reload"));
	restoreConfig->setToolTip(tr("Reload stored configuration"));
	connect(restoreConfig, SIGNAL(clicked()), this, SLOT(load()));
	
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(configWidget);
	horizontalLayout->addWidget(pagesWidget,1);
	
	QHBoxLayout *buttons = new QHBoxLayout;
	buttons->addStretch(1);
	buttons->addWidget(restoreConfig);
	buttons->addWidget(saveConfig);
	buttons->addWidget(closeConfig);
		
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addLayout(buttons);
	
	setWindowTitle(tr("PuMP configuration"));
	setLayout(mainLayout);
}

configDialog::~configDialog()
{
	
}

void inline configDialog::initSettings()
{
	PuMPSettings=new QSettings(QSettings::IniFormat, QSettings::UserScope,
		 "PuMP", "", NULL);
}

void configDialog::save() 
{
	qDebug() << "configDialog::save()";
	PuMPSettings->beginWriteArray("basicConfiguration");
	PuMPSettings->setValue("exportFormat", "toFolder");
	PuMPSettings->setValue("exportLocation", "/home");
	PuMPSettings->endArray();
	PuMPSettings->sync();
}

void configDialog::load() {
	qDebug() << "configDialog::load()";
	int size = PuMPSettings->beginReadArray("basicConfiguration");
	/* where to store values in memory, which class? */
	/* Which scheme to use */
	//for (int i=0; i<size; ++i) {
	//	PuMPSettings->setArrayIndex(i);
		qDebug() << "test" << PuMPSettings->value("exportFormat");
	//}
	PuMPSettings->endArray();
}
	

void configDialog::createIcons()
{
	QListWidgetItem *basicConfigIcon = new QListWidgetItem(configWidget);
	basicConfigIcon->setIcon(QIcon(":/basicConfiguration.png"));
	basicConfigIcon->setText(tr("Basic Configuration"));
	basicConfigIcon->setTextAlignment(Qt::AlignHCenter);
	basicConfigIcon->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	//basicConfigIcon->setBackgroundColor(QColor(5,5,200));
	
	QListWidgetItem *exportConfigIcon = new QListWidgetItem(configWidget);
	exportConfigIcon->setIcon(QIcon(":/export.png"));
	exportConfigIcon->setText(tr("Export Options"));
	exportConfigIcon->setTextAlignment(Qt::AlignHCenter);
	exportConfigIcon->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	
	QListWidgetItem *advancedConfigIcon = new QListWidgetItem(configWidget);
	advancedConfigIcon->setIcon(QIcon(":/advancedConfiguration.png"));
	advancedConfigIcon->setText(tr("Advanced Configuration"));
	advancedConfigIcon->setTextAlignment(Qt::AlignHCenter);
	advancedConfigIcon->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	//advancedConfigIcon->setBackgroundColor(QColor(5,5,20));
	
	QListWidgetItem *aboutPuMP = new QListWidgetItem(configWidget);
	aboutPuMP->setIcon(QIcon(":/about.png"));
	aboutPuMP->setText(tr("About PuMP"));
	aboutPuMP->setTextAlignment(Qt::AlignHCenter);
	aboutPuMP->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	
	connect(configWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

void configDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;
	
	pagesWidget->setCurrentIndex(configWidget->row(current));
}
