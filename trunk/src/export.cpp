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

#include "export.hh"


/******************************************************************************/
PuMP_ExportThread::PuMP_ExportThread(QObject *parent)
{
	
}
	
/******************************************************************************/
PuMP_ExportThread::~PuMP_ExportThread()
{
	
}
/******************************************************************************/
void PuMP_ExportThread::run()
{
	
}
/******************************************************************************/
void PuMP_ExportThread::exportPix(
	QList<QFileInfo> &files,
	const QFileInfo &dest,
	const QFileInfo &watermark,
	const QSize &watermarkPos,
	const QSize &destSize,
	int options)
{
	qDebug() << "Total files to be handled:" <<files.size()<< endl;
	QList<QFileInfo>::iterator i;
	
	for (i=files.begin(); i!=files.end(); ++i) {
		qDebug() << i->fileName();
	}
	
	
	
}
/******************************************************************************/
