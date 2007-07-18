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

#ifndef EXPORTDIALOG_HH_
#define EXPORTDIALOG_HH_

#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVariant>
#include <QVBoxLayout>

#include "settings.hh"

/******************************************************************************/

typedef enum
{
	IgnoreAspect,
	KeepAspect,
	KeepAspectByExpanding
}
PuMP_ExportMode;

typedef enum
{
	Unsmoothed,
	Smoothed
}
PuMP_ExportQuality;

typedef enum
{
	Centered,
	TopRight,
	TopLeft,
	TopCentered,
	BottomRight,
	BottomLeft,
	BottomCentered
}
PuMP_ExportPosition;

/******************************************************************************/

#define PUMP_EXPORTWIDGET_USEWATERMARK	"PuMP_ExportWidget::useWatermark"
#define PUMP_EXPORTWIDGET_WATERMARK 	"PuMP_ExportWidget::watermark"
#define PUMP_EXPORTWIDGET_WATERMARKSIZE "PuMP_ExportWidget::watermarkSize"
#define PUMP_EXPORTWIDGET_TRANSPARENCY	"PuMP_ExportWidget::transparency"
#define PUMP_EXPORTWIDGET_POSITION		"PuMP_ExportWidget::position"
#define PUMP_EXPORTWIDGET_OUTPUTDIR 	"PuMP_ExportWidget::outputDir"
#define PUMP_EXPORTWIDGET_OUTPUTSIZE 	"PuMP_ExportWidget::outputSize"
#define PUMP_EXPORTWIDGET_OUTPUTQUALITY	"PuMP_ExportWidget::outputQuality"
#define PUMP_EXPORTWIDGET_OUTPUTFORMAT 	"PuMP_ExportWidget::outputFormat"
#define PUMP_EXPORTWIDGET_OUTPUTMODE 	"PuMP_ExportWidget::outputMode"

/******************************************************************************/

class PuMP_ExportWidget : public QWidget, public PuMP_SettingsInterface 
{
	Q_OBJECT

	protected:
		int watermarkSpinBoxW_value;
		int watermarkSpinBoxH_value;

		QCheckBox *watermarkCheckBox1;
		QCheckBox *watermarkCheckBox2;
		QComboBox *outputComboBoxMode;
		QComboBox *outputComboBoxFormat;
		QComboBox *outputComboBoxQuality;
		QGridLayout *gridLayout;
		QGroupBox *watermarkGroupBox;
		QGroupBox *outputGroupBox;
		QHBoxLayout *hboxLayout;
		QHBoxLayout *hboxLayout1;
		QHBoxLayout *hboxLayout2;
		QHBoxLayout *hboxLayout3;
		QHBoxLayout *hboxLayout4;
		QHBoxLayout *hboxLayout5;
		QHBoxLayout *hboxLayout6;
		QHBoxLayout *hboxLayout7;
		QHBoxLayout *hboxLayout8;
		QLabel *label;
		QLabel *label1;
		QLabel *label2;
		QLabel *label3;
		QLabel *label4;
		QLabel *label5;
		QLabel *label6;
		QLabel *label7;
		QLabel *label8;
		QLabel *label9;
		QLabel *label10;
		QLabel *label11;
		QLabel *label12;
		QLineEdit *watermarkLineEdit;
		QLineEdit *outputLineEdit;
		QPixmap watermark;
		QPixmap watermarkScaled;
		QPushButton *outputPushButton;
		QPushButton *watermarkPushButton;
		QPushButton *watermarkPushButtonDefault;
		QPushButton *watermarkPushButtonDefaultSize;
		QRadioButton *watermarkRadioButtonC;
		QRadioButton *watermarkRadioButtonBC;
		QRadioButton *watermarkRadioButtonTR;
		QRadioButton *watermarkRadioButtonTL;
		QRadioButton *watermarkRadioButtonTC;
		QRadioButton *watermarkRadioButtonBL;
		QRadioButton *watermarkRadioButtonBR;
		QSpinBox *watermarkSpinBoxW;
		QSpinBox *watermarkSpinBoxH;
		QSpinBox *watermarkSpinBoxT;
		QSpinBox *outputSpinBoxW;
		QSpinBox *outputSpinBoxH;
		QVBoxLayout *vboxLayout;
		QVBoxLayout *vboxLayout1;
		QVBoxLayout *vboxLayout2;

		void setPreview(const QString &path = QString());
		void setPos(PuMP_ExportPosition position);
		PuMP_ExportPosition getPos();
	
	public:
		PuMP_ExportWidget(QWidget *parent = 0);
		~PuMP_ExportWidget();

		void getConfig(
			QPixmap &wmark,
			int &transparency,
			PuMP_ExportPosition &pos,
			QFileInfo &outputDir,
			QSize &scaleSize,
			PuMP_ExportQuality &quality,
			QString &format,
			PuMP_ExportMode &mode);

		void loadSettings();
		void storeSettings();

	public slots:
		void on_outputPushButton_clicked(bool checked);
		void on_watermarkCheckBox2_clicked(bool checked);
		void on_watermarkPushButton_clicked(bool checked);
		void on_watermarkPushButtonDefault_clicked(bool checked);
		void on_watermarkPushButtonDefaultSize_clicked(bool checked);
		void on_watermarkSpinBoxW_editingFinished();
		void on_watermarkSpinBoxH_editingFinished();
};

/******************************************************************************/

#define PUMP_EXPORTDIALOG_POS	"PuMP_ExportDialog::pos"
#define PUMP_EXPORTDIALOG_SIZE	"PuMP_ExportDialog::size"	

/******************************************************************************/

class PuMP_ExportDialog : public QDialog, public PuMP_SettingsInterface
{
	Q_OBJECT

	protected:
//		QList<QFileInfo> files;
//		QFileInfo dest;
//		QFileInfo watermark;
//		QSize watermarkPos;
//		QSize destSize;
//		int options;

		QVBoxLayout *vboxLayout;

		PuMP_ExportWidget *exportWidget;
		QDialogButtonBox *buttonBox;
		
		void accept();
	
	public:
		PuMP_ExportDialog(QWidget *parent = 0);
		~PuMP_ExportDialog();

		void loadSettings();
		void storeSettings();

};

/******************************************************************************/

#endif /*EXPORTDIALOG_HH_*/
