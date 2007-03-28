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

#ifndef EXPORT_HH_
#define EXPORT_HH_

#include <QDebug>
#include <QFileInfo>
#include <QList>
#include <QSize>
#include <QThread>

/******************************************************************************/

class PuMP_ExportThread : public QThread
{
	Q_OBJECT

	protected:
		void run();

	public:
		PuMP_ExportThread(QObject *parent = 0);
		~PuMP_ExportThread();
		
		void exportPix(
			QList<QFileInfo> &files,
			const QFileInfo &dest,
			const QFileInfo &watermark,
			const QSize &watermarkPos,
			const QSize &destSize,
			int options);

};

/******************************************************************************/

#endif /*EXPORT_HH_*/
