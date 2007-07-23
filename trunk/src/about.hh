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

#ifndef ABOUT_HH_
#define ABOUT_HH_

#include "configPages.hh"

#include <assert.h>
#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

/******************************************************************************/

typedef enum
{
	abPump,
	abQt
}
PuMP_AboutMessageType;

/******************************************************************************/

class PuMP_AboutMessage : public QDialog
{
	Q_OBJECT

	protected:
		PuMP_AboutWidget *abpWidget;
		PuMP_AboutQtWidget *abqWidget;
		
		QVBoxLayout *vboxLayout;
		QDialogButtonBox *button;

		PuMP_AboutMessage(
			QWidget *parent = 0,
			PuMP_AboutMessageType type = abPump)
			: QDialog(parent)
		{
			vboxLayout = new QVBoxLayout(this);
			abpWidget = NULL;
			abqWidget = NULL;
			
			if(type == abPump)
			{
				abpWidget = new PuMP_AboutWidget();
				vboxLayout->addWidget(abpWidget);
			}
			else if(type == abQt)
			{
				abqWidget = new PuMP_AboutQtWidget();
				vboxLayout->addWidget(abqWidget);
			}
			else assert(false);

			button = new QDialogButtonBox(QDialogButtonBox::Ok);
			vboxLayout->addWidget(button);
			connect(button, SIGNAL(accepted()), this, SLOT(close())); 
		};
	
	public:
		~PuMP_AboutMessage()
		{
			delete vboxLayout;
		};
	
		static void aboutPuMP(QWidget *parent = 0)
		{
			PuMP_AboutMessage *abm = new PuMP_AboutMessage(parent);
			abm->exec();
			
			delete abm;
		};

		static void aboutQt(QWidget *parent = 0)
		{
			PuMP_AboutMessage *abm = new PuMP_AboutMessage(parent, abQt);
			abm->exec();
			
			delete abm;
		};

};

/******************************************************************************/

#endif /*ABOUT_HH_*/
