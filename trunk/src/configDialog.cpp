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

/******************************************************************************/

PuMP_ConfigDialog::PuMP_ConfigDialog(QWidget *parent)
	: QDialog(parent), PuMP_SettingsInterface()
{
	configWidget = new QListWidget(this);
	configWidget->setViewMode(QListView::IconMode);
	configWidget->setFlow(QListView::TopToBottom);
	configWidget->setIconSize(QSize(64, 64));
	configWidget->setGridSize(QSize(120, 100));
	configWidget->setMovement(QListView::Static);
	configWidget->setResizeMode(QListView::Fixed);
	configWidget->setTextElideMode(Qt::ElideNone);
	configWidget->setWrapping(false);
	configWidget->setUniformItemSizes(true);
	configWidget->setMinimumWidth(130);
	configWidget->setMaximumWidth(130);

	pagesWidget = new QStackedWidget(this);
	basicWidget = new PuMP_BasicConfigWidget(pagesWidget);
	exportWidget = new PuMP_ExportWidget(pagesWidget);
	aboutPuMP = new PuMP_AboutWidget(pagesWidget);
	aboutQt = new PuMP_AboutQtWidget(pagesWidget);

	pagesWidget->addWidget(basicWidget);
	pagesWidget->addWidget(exportWidget);
	pagesWidget->addWidget(aboutPuMP);
	pagesWidget->addWidget(aboutQt);

	okButton = new QPushButton("Ok", this);
	applyButton = new QPushButton("Apply", this);
	cancelButton = new QPushButton("Cancel", this);
	resetButton = new QPushButton("Reset", this);
	
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	createIcons();

	QHBoxLayout *hboxLayout1 = new QHBoxLayout();
	hboxLayout1->addWidget(configWidget);
	hboxLayout1->addWidget(pagesWidget, 1);
	QHBoxLayout *hboxLayout2 = new QHBoxLayout();
	hboxLayout2->addStretch(1);
	hboxLayout2->addWidget(resetButton);
	hboxLayout2->addWidget(applyButton);
	hboxLayout2->addWidget(okButton);
	hboxLayout2->addWidget(cancelButton);
	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	vboxLayout->addLayout(hboxLayout1);
	vboxLayout->addLayout(hboxLayout2);
	
	setWindowTitle(tr("PuMP configuration"));
	resize(300, 400);
}

PuMP_ConfigDialog::~PuMP_ConfigDialog()
{
	delete basConfig;
	delete expConfig;
	delete advConfig;
	delete about;
	delete basicWidget;
	delete exportWidget;
	delete aboutPuMP;
	delete aboutQt;
}

void PuMP_ConfigDialog::createIcons()
{
	basConfig = new QListWidgetItem(configWidget);
	basConfig->setIcon(QIcon(":/configure128.png"));
	basConfig->setText("Basic Configuration");
	basConfig->setTextAlignment(Qt::AlignHCenter);
	basConfig->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	
	expConfig = new QListWidgetItem(configWidget);
	expConfig->setIcon(QIcon(":/save_all128.png"));
	expConfig->setText("Export Options");
	expConfig->setTextAlignment(Qt::AlignHCenter);
	expConfig->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	
	advConfig = new QListWidgetItem(configWidget);
	advConfig->setIcon(QIcon(":/pump128.png"));
	advConfig->setText("About PuMP");
	advConfig->setTextAlignment(Qt::AlignHCenter);
	advConfig->setFlags(0);
	
	about = new QListWidgetItem(configWidget);
	about->setIcon(QIcon(":/qt-logo128.png"));
	about->setText("About Qt");
	about->setTextAlignment(Qt::AlignHCenter);
	about->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	
	connect(
		configWidget,
		SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
		this,
		SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

void PuMP_ConfigDialog::loadSettings()
{
	
}

void PuMP_ConfigDialog::storeSettings()
{
	
}

void PuMP_ConfigDialog::changePage(
	QListWidgetItem *current,
	QListWidgetItem *previous)
{
	if(!current) current = previous;
	pagesWidget->setCurrentIndex(configWidget->row(current));
}

/******************************************************************************/
