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
#include <QString>
#include <QThread>
#include <QDataStream>
#include <QMessageBox>
#include <QDialog>
#include <QPushButton>


#define ZLIB_CHUNK 65536 // increase for tuning

/******************************************************************************/

class PuMP_ExportThread : public QThread
{
	Q_OBJECT

	private:
		QList<QFileInfo> exp_files;
		QFileInfo exp_dest;
		QFileInfo exp_watermark;
		QSize exp_watermarkPos;
		QSize exp_destSize;
		int exp_options;
		
		void zip_append(QFileInfo appendFile, QFileInfo archive); 
		void gz_append(QFileInfo appendFile, QFileInfo archive);
		
		/*void exportPix(
			QList<QFileInfo> &files,
			const QFileInfo &dest,
			const QFileInfo &watermark,
			const QSize &watermarkPos,
			const QSize &destSize,
			int options
		);*/

	protected:
		void run();
		bool killed;
		
	public:
		PuMP_ExportThread(QObject *parent = 0);
		~PuMP_ExportThread();
		
	
		
		void doExportPix(QList<QFileInfo> &files,
			const QFileInfo &dest,
			const QFileInfo &watermark,
			const QSize &watermarkPos,
			const QSize &destSize,
			int options
		);
			
		void setKilled();
		bool wasKilled();
	
	public slots:
		void stop();

};

/******************************************************************************/

#endif /*EXPORT_HH_*/
