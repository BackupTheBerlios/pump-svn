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

extern "C" {
	#include <stdio.h>
	#include <errno.h>
	#include "src/zlib/zlib.h"
	#include "src/zip/zip.h"
}

/******************************************************************************/
PuMP_ExportThread::PuMP_ExportThread(QObject *parent)
{
	killed=false;
}
	
	
PuMP_ExportThread::~PuMP_ExportThread()
{
	qDebug() << "~Pump_ExportThread()";
	stop();
}


void PuMP_ExportThread::run()
{
	killed=false;
	
	qDebug() << "Total files to be handled:" <<exp_files.size();
	for (QList<QFileInfo>::iterator i=exp_files.begin(); i!=exp_files.end(); ++i) {
		qDebug() << i->fileName();
		zip_append(*i, exp_dest);
	}	

}


void PuMP_ExportThread::stop()
{
	qDebug() << "PuMP_ExportThread::stop()";
	wait();
	setKilled();
}


void PuMP_ExportThread::doExportPix(
	QList<QFileInfo> &files,
	const QFileInfo &dest,
	const QFileInfo &watermark,
	const QSize &watermarkPos,
	const QSize &destSize,
	int options)
{
	QMessageBox ErrMsg;
	ErrMsg.setWindowTitle("PuMP Warning");
	ErrMsg.setIcon(QMessageBox::Critical);
	
	
	
	/* Sanity checks */
	if (files.size() == 0) {
		qDebug() << "Nothing to do";
		return;
	}
	/* All passed files readable? */
	for (QList<QFileInfo>::iterator i=files.begin(); i!=files.end(); ++i) {
		if(!i->exists()||!i->isReadable()) {
		ErrMsg.setText("File "+i->absoluteFilePath()+" not readable");
		ErrMsg.setDetailedText("You passed some unreadable files, please "\
		"check that all files exist and you got read permissions!<p>"\
		"Offending file: " + i->absoluteFilePath());
		ErrMsg.exec();
		qDebug() << "File does not exist or is not readable: " \
		<< i->absoluteFilePath();
		return;
		}
	}
	/* Does target exist yet or is not writeable? */
	if ( dest.exists() ) {
		ErrMsg.setText("Archive "+dest.absoluteFilePath()+" exists yet");
		ErrMsg.setDetailedText("The archive " + dest.absoluteFilePath() + \
		"exists, please remove or rename it and try again or choose another"\
		"filename for the target archive.");
		ErrMsg.exec();
		qDebug() << "Target" << dest.filePath() << dest.fileName() << "does "\
		"exist yet";
		return;
	}
	// FIXME: 	Seems to work for files only
	// 			We need to know whether we may write to the directory
	if ( !dest.isWritable() ) {
		ErrMsg.setText("Archive destination "+dest.absoluteFilePath()+" is not "\
		"writable");
		ErrMsg.setDetailedText("Choose another destination and/or make sure"\
		"that you are allowed to create files there");
		qDebug() << "Target" << dest.filePath() << dest.fileName() << "not "\
		"writeable";
		//return;
	}
	if (!watermark.isReadable() ) {
				
		QMessageBox msgBox;
		msgBox.setText("Watermark file "+watermark.absoluteFilePath()+ "not"\
		" readable<p>Do you want to continue without a watermark?");
		msgBox.setDetailedText("Make sure you chose the correct Image as your "\
		"watermark to be embedded in your Pictures");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		switch (msgBox.exec()) {
		case QMessageBox::Yes:
			qDebug()<<"Continuing without watermark";
			// unset corresponding fileinfo or set flag?
			break;
		case QMessageBox::No:
			qDebug()<<"Return from export... - perhaps jump to watermark config?";
			return;
			break;
		default:
			qDebug()<< "default case: Watermark not readable!";
			break;
		}
		
		//Pop up continue/go-back dialog
		
	}
	//How/Where to handle watermarksize given as %-age of resulting image?
	if ( watermarkPos.width() > destSize.width() ||
		 watermarkPos.height() > destSize.height() ) {
		ErrMsg.setText("Watermark too big");
		ErrMsg.setDetailedText("");
		qDebug() << "Watermark too big for resulting image size";
		return;
	}
		
	/* Accept Parameters */
	exp_files=files;
	exp_dest=dest;
	exp_watermark=watermark;
	exp_watermarkPos=watermarkPos;
	exp_destSize=destSize;
	exp_options=options;
	
	
	/* Start the thread */
	start();	
}


/**
 * Add a list of files to a zip archive
 * Basic sanity checks are performed by the caller (doExportPix)
 * Code adapted from minizip (included in zlib)
 * Really, really ... I mean REALLY ... ugly.
 * Needs a rewrite.
 */
void PuMP_ExportThread::zip_append(QFileInfo appendFile, QFileInfo archive) {
	qDebug()<< "zip_append";
	
	/* 
	 * Ugly - is there a better way?
	 * Either a qt-function like fread or a nicer way of converting to char
	 */
	QString app_str=appendFile.absoluteFilePath();
	QByteArray app_ba=app_str.toLatin1();
	const char *app_char=app_ba.data();
	
	QString archive_str=archive.absoluteFilePath();
	QByteArray archive_ba=archive_str.toLatin1();
	const char *archive_char=archive_ba.data();
	
	FILE *fin;
	zipFile zf;
	int errclose, err, size_read, overwrite;
	char buf[WRITEBUFFERSIZE];
	
	if (!archive.exists())
		overwrite=1; //Create
	else
		overwrite=2; //Append
	
#ifdef USEWIN32IOAPI
	zlib_filefunc_def ffunc;
	fill_win32_filefunc(&ffunc);
	zf = zipOpen2(archive_char, (overwrite==2)?2:0 , NULL ,&ffunc);
#else
	zf = zipOpen(archive_char, (overwrite==2)?2:0);
#endif

	if (zf == NULL) {
		qDebug() <<"error opening " << archive_char;
	} else {
		qDebug() << "creating " << archive_char;
	}
	
	
	/* Later on we should pass correct timestamps */
	unsigned long fake_time=0, crcOfFile=0;
	/* pass via parameter later on */
	int compress_level=4;
	

	
	
	err = zipOpenNewFileInZip3(zf, app_char, (zip_fileinfo *)&fake_time,
				NULL, 0, NULL, 0, NULL,
				(compress_level != 0) ? Z_DEFLATED : 0,
				compress_level, 0,
				-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
				/*password*/ NULL, 0 /*crcOfFile*/);
	
	if (err != ZIP_OK)
		qDebug() << "error opening zipfile" << archive_char;
	else {
		if( (fin = fopen(app_char, "rb")) == NULL ) {
			err = ZIP_ERRNO;
			qDebug() << "Could not open file " << app_char;
		}
	}
	
	if (err == ZIP_OK) {
		do {
			err = ZIP_OK;
			size_read = (int)fread(buf, 1, WRITEBUFFERSIZE, fin);
			if (size_read < WRITEBUFFERSIZE) {
				if (feof(fin)==0) {
					qDebug() << "Error reading " << app_char;
					err = ZIP_ERRNO;
				}
				
				if (size_read > 0) {
					err = zipWriteInFileInZip(zf, buf, size_read);
					if (err<0) {
						qDebug() << " error writing to zipfile";
					}
				}
			}
		} while ((err == ZIP_OK) && (size_read>0));
	}
	
	if (fin)
		fclose(fin);
	
	if (err<0) {
		err = ZIP_ERRNO;
	} else {
		err = zipCloseFileInZip(zf);
		if (err != ZIP_OK) {
			qDebug() << "error closing file "<<archive_char;
		}
	}
	
	err=zipClose(zf,NULL);
	if (err != ZIP_OK)
		qDebug() << "Error closing archive";
}

/**
 * Add a list of files to a gz archive
 * 
 * NOT FINISHED YET
 */
 void PuMP_ExportThread::gz_append(QFileInfo appendFile, QFileInfo archive)
 {
 	int ret, flush, sum;
	z_stream strm;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	char strm_in[ZLIB_CHUNK];
	char strm_out[ZLIB_CHUNK];
	
	/* compression level:
	 * -1=default, compromise between speed and size: Z_DEFAULT_COMPRESSION
	 * 0=no compression ... 9=greater compression
	 */
	int level = Z_DEFAULT_COMPRESSION;
	
	/* 
	 * Ugly - is there a better way?
	 * Either a qt-function like fread or a nicer way of converting to char
	 */
	QString app_str=appendFile.absoluteFilePath();
	QByteArray app_ba=app_str.toLatin1();
	const char *app_char=app_ba.data();
	
	QString archive_str=archive.absoluteFilePath();
	QByteArray archive_ba=archive_str.toLatin1();
	const char *archive_char=archive_ba.data();
	
	qDebug() << "I theoretically should append "<<app_char<<"to archive "<<archive_char;
	
	/* initialize zlib state */
	if ( (ret=deflateInit(&strm, level)) != Z_OK ) {
		qDebug() << "zlib: deflate_Init failed, returned: "<<ret;
		return;
	}
		
	/* open files */
	FILE *fout = fopen(archive_char, "w+");
	FILE *fin = fopen(app_char, "r");
	
	if ( fout==NULL || fin==NULL )
		qDebug() << "fout or fin were NULL";
	
	#if 0	
	strm.avail_in = fread(strm_in, 1, ZLIB_CHUNK, fin);
	sum+=strm.avail_in;
	
	if (ferror(fin)) {
			(void)deflateEnd(&strm);
			qDebug() <<"Z_ERRNO: " << Z_ERRNO;
			return;
		}
		
		flush = feof(fin) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = strm_in;
		
		/*
		 * DEFLATE
		 */
		 do {
		 	strm.avail_out = ZLIB_CHUNK;
		 	strm.next_out = strm_out;
		 	
		 	ret=deflate(&strm, flush);
		 	assert(ret != Z_STREAM_ERROR);
		 	
		 	have = CHUNK - strm.avail_out;
		 	if (fwrite(out, 1, have, fout) != have || ferror(fout)) {
		 		(void)deflateEnd(&strm);
		 		qDebug() << "done.";
		 		return Z_ERRNO;
		 	} while (strm.avail_out == 0);
		 	assert (strm.avail_in == 0);
		 	
		 } while (flush != Z_FINISH);
		 
	}
	
	qDebug() << "bytes read: " << sum;
	#endif
	
	(void)deflateEnd(&strm);
 }

/**
 * Function to mark the current execution of this thread as killed.
 * Needed for a stop-function.
 */
void PuMP_ExportThread::setKilled()
{
	killed = true;
}

/**
 * Function returns whether the last execution of this thread was killed.
 * @return	True if the execution was killed, false otherwise.
 */
bool PuMP_ExportThread::wasKilled()
{
	return killed;
}

