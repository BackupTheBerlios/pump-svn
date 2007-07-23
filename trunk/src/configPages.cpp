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

#include "configPages.hh"

/******************************************************************************/

PuMP_BasicConfigWidget::PuMP_BasicConfigWidget(QWidget *parent)
	: QWidget(parent), PuMP_SettingsInterface()
{
	
}

PuMP_BasicConfigWidget::~PuMP_BasicConfigWidget()
{
	
}
		
void PuMP_BasicConfigWidget::loadSettings()
{
	
}

void PuMP_BasicConfigWidget::storeSettings()
{
	
}

/******************************************************************************/

PuMP_AboutWidget::PuMP_AboutWidget(QWidget *parent) : QWidget(parent)
{
	QLabel *leftLabel = new QLabel();
	leftLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	leftLabel->setPixmap(QPixmap(":/pump.png"));
	QLabel *rightLabel = new QLabel();
	rightLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	rightLabel->setWordWrap(true);
	rightLabel->setText("<h1>About PuMP</h1>" \
		"<p>This program uses Qt Open Source Edition version " \
		QT_VERSION_STR \
		".</p>" \
		"<p>Qt Open Source Edition is intended for the development of Open " \
		"Source applications. You need a commercial Qt license for " \
		"development of proprietary (closed source) applications.</p>" \
		"<p>Please see www.trolltech.com/company/model/ for an overview of " \
		"Qt licensing.</p>" \
		"<p>Qt is a C++ toolkit for cross-platform application " \
		"development.</p>" \
		"<p>Qt provides single-source portability across MS Windows, Mac OS " \
		"X, Linux, and all major commercial Unix variants. Qt is also " \
		"available for embedded devices as Qtopia Core.</p>" \
		"<p>Qt is a Trolltech product. See www.trolltech.com/qt/ for more " \
		"information.</p>");

	hboxLayout = new QHBoxLayout(this);
	hboxLayout->addWidget(leftLabel);
	hboxLayout->addWidget(rightLabel);
	
	setMaximumWidth(400);
}

PuMP_AboutWidget::~PuMP_AboutWidget()
{
	delete hboxLayout;
}

/******************************************************************************/

PuMP_AboutQtWidget::PuMP_AboutQtWidget(QWidget *parent) : QWidget(parent)
{
	QLabel *leftLabel = new QLabel();
	leftLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	leftLabel->setPixmap(QPixmap(":/qt-logo64.png"));
	QLabel *rightLabel = new QLabel();
	rightLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	rightLabel->setWordWrap(true);
	rightLabel->setText("<h1>About Qt</h1>" \
		"<p>This program uses Qt Open Source Edition version " \
		QT_VERSION_STR \
		".</p>" \
		"<p>Qt Open Source Edition is intended for the development of Open " \
		"Source applications. You need a commercial Qt license for " \
		"development of proprietary (closed source) applications.</p>" \
		"<p>Please see <a href=\"http://www.trolltech.com/company/model/\">" \
		"www.trolltech.com/company/model/</a> for an overview of " \
		"Qt licensing.</p>" \
		"<p>Qt is a C++ toolkit for cross-platform application " \
		"development.</p>" \
		"<p>Qt provides single-source portability across MS Windows, Mac OS " \
		"X, Linux, and all major commercial Unix variants. Qt is also " \
		"available for embedded devices as Qtopia Core.</p>" \
		"<p>Qt is a Trolltech product. See " \
		"<a href=\"http://www.trolltech.com/qt/\">www.trolltech.com/qt/</a> " \
		"for more information.</p>");

	hboxLayout = new QHBoxLayout(this);
	hboxLayout->addWidget(leftLabel);
	hboxLayout->addWidget(rightLabel);
	
	setMaximumWidth(400);
}

PuMP_AboutQtWidget::~PuMP_AboutQtWidget()
{
	delete hboxLayout;
}

/******************************************************************************/
