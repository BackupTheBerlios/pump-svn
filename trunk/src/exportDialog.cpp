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

#include "exportDialog.hh"
#include "mainWindow.hh"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include <assert.h>

/******************************************************************************/

PuMP_ExportWidget::PuMP_ExportWidget(QWidget *parent)
	: QWidget(parent), PuMP_SettingsInterface()
{
	watermarkGroupBox = new QGroupBox(this);
	watermarkGroupBox->setTitle("Watermark");
	outputGroupBox = new QGroupBox(this);
	outputGroupBox->setTitle("Output");

	watermarkCheckBox1 = new QCheckBox(watermarkGroupBox);
	watermarkCheckBox1->setText("Keep aspect ratio");
	watermarkCheckBox1->setChecked(true);
	watermarkCheckBox2 = new QCheckBox(watermarkGroupBox);
	watermarkCheckBox2->setText("None");
	watermarkCheckBox2->setChecked(false);
	connect(
		watermarkCheckBox2,
		SIGNAL(clicked(bool)),
		this,
		SLOT(on_watermarkCheckBox2_clicked(bool)));
	
	outputComboBoxMode = new QComboBox(outputGroupBox);
	outputComboBoxMode->insertItem(IgnoreAspect, "ignore aspect ratio");
	outputComboBoxMode->insertItem(KeepAspect, "keep aspect ratio");
	outputComboBoxMode->insertItem(
		KeepAspectByExpanding,
		"keep aspect ratio by expanding");
	outputComboBoxFormat = new QComboBox(outputGroupBox);
	outputComboBoxFormat->insertItems(0, PuMP_MainWindow::nameFilters);
	outputComboBoxQuality = new QComboBox(outputGroupBox);
	outputComboBoxQuality->insertItem(
		Unsmoothed,
		"without smoothing (poor quality but fast)");
	outputComboBoxQuality->insertItem(
		Smoothed,
		"with smoothing (better quality but slower)");
	
	gridLayout = new QGridLayout();
	hboxLayout = new QHBoxLayout();
	hboxLayout1 = new QHBoxLayout();
	hboxLayout2 = new QHBoxLayout();
	hboxLayout3 = new QHBoxLayout();
	hboxLayout4 = new QHBoxLayout();
	hboxLayout5 = new QHBoxLayout();
	hboxLayout6 = new QHBoxLayout();
	hboxLayout7 = new QHBoxLayout();
	hboxLayout8 = new QHBoxLayout();

	label = new QLabel(watermarkGroupBox);
	label->setMinimumSize(QSize(70, 0));
	label->setText("Preview:");
	label1 = new QLabel(watermarkGroupBox);
	label1->setMinimumSize(QSize(10, 0));
	label2 = new QLabel(watermarkGroupBox);
	label2->setMinimumSize(QSize(70, 0));
	label2->setText("File:");
	label3 = new QLabel(watermarkGroupBox);
	label3->setMaximumSize(QSize(70, 16777215));
	label3->setMinimumSize(QSize(70, 0));
	label3->setText("Size (Pixels):");
	label4 = new QLabel(watermarkGroupBox);
	label4->setMaximumSize(QSize(10, 16777215));
	label4->setAlignment(Qt::AlignCenter);
	label4->setText("X");
	label5 = new QLabel(watermarkGroupBox);
	label5->setMaximumSize(QSize(70, 16777215));
	label5->setText("Transparency (%):");
	label6 = new QLabel(watermarkGroupBox);
	label6->setMaximumSize(QSize(70, 16777215));
	label6->setText("Position:");
	label7 = new QLabel(outputGroupBox);
	label7->setMinimumSize(QSize(70, 0));
	label7->setText("Export to:");
	label8 = new QLabel(outputGroupBox);
	label8->setMaximumSize(QSize(70, 16777215));
	label8->setText("Size (Pixels):");
	label9 = new QLabel(outputGroupBox);
	label9->setMaximumSize(QSize(10, 16777215));
	label9->setAlignment(Qt::AlignCenter);
	label9->setText("X");
	label10 = new QLabel(outputGroupBox);
	label10->setMaximumSize(QSize(70, 16777215));
	label10->setText("Resize-Mode:");
	label11 = new QLabel(outputGroupBox);
	label11->setMaximumSize(QSize(70, 16777215));
	label11->setText("Quality:");
	label12 = new QLabel(outputGroupBox);
	label12->setMaximumSize(QSize(70, 16777215));
	label12->setText("Format:");

	watermarkLineEdit = new QLineEdit(watermarkGroupBox);
	watermarkLineEdit->setReadOnly(true);
	outputLineEdit = new QLineEdit(outputGroupBox);

	outputPushButton = new QPushButton(outputGroupBox);
	outputPushButton->setMaximumSize(QSize(30, 16777215));
	outputPushButton->setText(QString());
	outputPushButton->setIcon(QIcon(":/folder16.png"));
	connect(
		outputPushButton,
		SIGNAL(clicked(bool)),
		this,
		SLOT(on_outputPushButton_clicked(bool)));
	watermarkPushButton = new QPushButton(watermarkGroupBox);
	watermarkPushButton->setMaximumSize(QSize(30, 16777215));
	watermarkPushButton->setText(QString());
	watermarkPushButton->setIcon(QIcon(":/folder16.png"));
	connect(
		watermarkPushButton,
		SIGNAL(clicked(bool)),
		this,
		SLOT(on_watermarkPushButton_clicked(bool)));
	watermarkPushButtonDefaultSize = new QPushButton(watermarkGroupBox);
	watermarkPushButtonDefaultSize->setMaximumSize(QSize(30, 16777215));
	watermarkPushButtonDefaultSize->setText("1:1");
	connect(
		watermarkPushButtonDefaultSize,
		SIGNAL(clicked(bool)),
		this,
		SLOT(on_watermarkPushButtonDefaultSize_clicked(bool)));
	watermarkPushButtonDefault = new QPushButton(watermarkGroupBox);
	watermarkPushButtonDefault->setMaximumSize(QSize(30, 16777215));
	watermarkPushButtonDefault->setIcon(QIcon(":/godefault.png"));
	connect(
		watermarkPushButtonDefault,
		SIGNAL(clicked(bool)),
		this,
		SLOT(on_watermarkPushButtonDefault_clicked(bool)));

	watermarkRadioButtonC = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonC->setText("centered");
	watermarkRadioButtonTR = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonTR->setText("top right");
	watermarkRadioButtonTL = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonTL->setText("top left");
	watermarkRadioButtonTC = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonTC->setText("top center");
	watermarkRadioButtonBR = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonBR->setText("bottom right");
	watermarkRadioButtonBL = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonBL->setText("bottom left");
	watermarkRadioButtonBC = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonBC->setText("bottom center");

	watermarkSpinBoxW = new QSpinBox(watermarkGroupBox);
	watermarkSpinBoxW->setMaximum(100000);
	watermarkSpinBoxW->setMinimum(1);
	watermarkSpinBoxW_value = watermarkSpinBoxW->value();
	connect(
		watermarkSpinBoxW,
		SIGNAL(editingFinished()),
		this,
		SLOT(on_watermarkSpinBoxW_editingFinished()));
	watermarkSpinBoxH = new QSpinBox(watermarkGroupBox);
	watermarkSpinBoxH->setMaximum(100000);
	watermarkSpinBoxH->setMinimum(1);
	watermarkSpinBoxH_value = watermarkSpinBoxH->value();
	connect(
		watermarkSpinBoxH,
		SIGNAL(editingFinished()),
		this,
		SLOT(on_watermarkSpinBoxH_editingFinished()));
	watermarkSpinBoxT = new QSpinBox(watermarkGroupBox);
	outputSpinBoxW = new QSpinBox(outputGroupBox);
	outputSpinBoxW->setMaximum(100000);
	outputSpinBoxW->setMinimum(1);
	outputSpinBoxH = new QSpinBox(outputGroupBox);
	outputSpinBoxH->setMaximum(100000);
	outputSpinBoxH->setMinimum(1);
	
	vboxLayout = new QVBoxLayout(this);
	vboxLayout1 = new QVBoxLayout(watermarkGroupBox);
	vboxLayout2 = new QVBoxLayout(outputGroupBox);

	gridLayout->addWidget(watermarkRadioButtonC, 1, 2, 1, 1);
	gridLayout->addWidget(watermarkRadioButtonBC, 2, 2, 1, 1);
	gridLayout->addWidget(watermarkRadioButtonTR, 0, 3, 1, 1);
	gridLayout->addWidget(watermarkRadioButtonTL, 0, 1, 1, 1);
	gridLayout->addWidget(label6, 0, 0, 1, 1);
	gridLayout->addWidget(watermarkRadioButtonTC, 0, 2, 1, 1);
	gridLayout->addWidget(watermarkRadioButtonBL, 2, 1, 1, 1);
	gridLayout->addWidget(watermarkRadioButtonBR, 2, 3, 1, 1);

	hboxLayout->addWidget(label);
	hboxLayout->addWidget(label1);
	hboxLayout->insertStretch(-1);
	hboxLayout1->addWidget(label2);
	hboxLayout1->addWidget(watermarkLineEdit);
	hboxLayout1->addWidget(watermarkPushButtonDefault);
	hboxLayout1->addWidget(watermarkPushButton);
	hboxLayout2->addWidget(label3);
	hboxLayout2->addWidget(watermarkSpinBoxW);
	hboxLayout2->addWidget(label4);
	hboxLayout2->addWidget(watermarkSpinBoxH);
	hboxLayout2->addWidget(watermarkCheckBox1);
	hboxLayout2->addWidget(watermarkPushButtonDefaultSize);
	hboxLayout3->addWidget(label5);
	hboxLayout3->addWidget(watermarkSpinBoxT);
	hboxLayout4->addWidget(label7);
	hboxLayout4->addWidget(outputLineEdit);
	hboxLayout4->addWidget(outputPushButton);
	hboxLayout5->addWidget(label8);
	hboxLayout5->addWidget(outputSpinBoxW);
	hboxLayout5->addWidget(label9);
	hboxLayout5->addWidget(outputSpinBoxH);
	hboxLayout6->addWidget(label10);
	hboxLayout6->addWidget(outputComboBoxMode);
	hboxLayout7->addWidget(label12);
	hboxLayout7->addWidget(outputComboBoxFormat);
	hboxLayout8->addWidget(label11);
	hboxLayout8->addWidget(outputComboBoxQuality);
	
	vboxLayout1->addWidget(watermarkCheckBox2);
	vboxLayout1->addLayout(hboxLayout);
	vboxLayout1->addLayout(hboxLayout1);
	vboxLayout1->addLayout(hboxLayout2);
	vboxLayout1->addLayout(hboxLayout3);
	vboxLayout1->addLayout(gridLayout);
	
	vboxLayout2->addLayout(hboxLayout4);
	vboxLayout2->addLayout(hboxLayout5);
	vboxLayout2->addLayout(hboxLayout6);
	vboxLayout2->addLayout(hboxLayout7);
	vboxLayout2->addLayout(hboxLayout8);

	vboxLayout->addWidget(watermarkGroupBox);
	vboxLayout->addWidget(outputGroupBox);

	loadSettings();
}

PuMP_ExportWidget::~PuMP_ExportWidget()
{
	delete gridLayout;
	delete hboxLayout;
	delete hboxLayout1;
	delete hboxLayout2;
	delete hboxLayout3;
	delete hboxLayout4;
	delete hboxLayout5;
	delete hboxLayout6;
	delete hboxLayout7;
	delete hboxLayout8;
	delete vboxLayout;
	delete vboxLayout1;
	delete vboxLayout2;
}

void PuMP_ExportWidget::setPreview(const QString &path)
{
	QFileInfo info(path);
	if(info.exists())
	{
		watermark.load(info.filePath());
		if(!watermark.isNull())
		{
			outputGroupBox->setMaximumSize(outputGroupBox->size());
			watermarkScaled = watermark;
			watermarkLineEdit->setText(path);
			watermarkSpinBoxW->setValue(watermark.width());
			watermarkSpinBoxW_value = watermark.width();
			watermarkSpinBoxH->setValue(watermark.height());
			watermarkSpinBoxH_value = watermark.height();

			label1->setMinimumSize(watermark.size());
			label1->setMaximumSize(watermark.size());
			label1->setPixmap(watermark);
			label1->updateGeometry();
		}
		else
		{
			QMessageBox::warning(
				this,
				"Warning",
				"Couldn't load " + info.filePath() + "!");
			watermarkLineEdit->setText("default.png");
			watermark.load(":/default.png");
			watermarkScaled = watermark;
			watermarkSpinBoxW->setValue(watermark.width());
			watermarkSpinBoxW_value = watermark.width();
			watermarkSpinBoxH->setValue(watermark.height());
			watermarkSpinBoxH_value = watermark.height();

			label1->setMinimumSize(watermark.size());
			label1->setMaximumSize(watermark.size());
			label1->setPixmap(watermark);
			label1->updateGeometry();
			updateGeometry();
		}
	}
	else
	{
		watermarkLineEdit->setText("default.png");
		watermark.load(":/default.png");
		watermarkScaled = watermark;
		watermarkSpinBoxW->setValue(watermark.width());
		watermarkSpinBoxW_value = watermark.width();
		watermarkSpinBoxH->setValue(watermark.height());
		watermarkSpinBoxH_value = watermark.height();

		label1->setMinimumSize(watermark.size());
		label1->setMaximumSize(watermark.size());
		label1->setPixmap(watermark);
		label1->updateGeometry();
		updateGeometry();
	}
}

void PuMP_ExportWidget::setPos(PuMP_ExportPosition position)
{
	switch(position)
	{
		case Centered:
		{
			watermarkRadioButtonC->setChecked(true);
			break;
		}
		case TopRight:
		{
			watermarkRadioButtonTR->setChecked(true);
			break;
		}
		case TopLeft:
		{
			watermarkRadioButtonTL->setChecked(true);
			break;
		}
		case TopCentered:
		{
			watermarkRadioButtonTC->setChecked(true);
			break;
		}
		case BottomLeft:
		{
			watermarkRadioButtonBR->setChecked(true);
			break;
		}
		case BottomRight:
		{
			watermarkRadioButtonBL->setChecked(true);
			break;
		}
		case BottomCentered:
		{
			watermarkRadioButtonBC->setChecked(true);
			break;
		}
	}
}

PuMP_ExportPosition PuMP_ExportWidget::getPos()
{
	if(watermarkRadioButtonC->isChecked()) return Centered;
	if(watermarkRadioButtonTR->isChecked()) return TopRight;
	if(watermarkRadioButtonTL->isChecked()) return TopLeft;
	if(watermarkRadioButtonTC->isChecked()) return TopCentered;
	if(watermarkRadioButtonBR->isChecked()) return BottomRight;
	if(watermarkRadioButtonBL->isChecked()) return BottomLeft;
	if(watermarkRadioButtonBC->isChecked()) return BottomCentered;
	assert(false);
}

void PuMP_ExportWidget::getConfig(
	QPixmap &wmark,
	int &transparency,
	PuMP_ExportPosition &pos,
	QFileInfo &outputDir,
	QSize &scaleSize,
	PuMP_ExportQuality &quality,
	QString &format,
	PuMP_ExportMode &mode)
{
	wmark = watermarkScaled;
	transparency = watermarkSpinBoxT->value();
	pos = getPos();

	outputDir.setFile(outputLineEdit->text());
	if(!outputDir.exists())
	{
		outputLineEdit->setText(QDir::homePath());
		outputDir.setFile(QDir::homePath());
	}
	
	scaleSize = QSize(outputSpinBoxW->value(), outputSpinBoxH->value());
	quality = (PuMP_ExportQuality) outputComboBoxQuality->currentIndex();
	format = outputComboBoxFormat->currentText();
	mode = (PuMP_ExportMode) outputComboBoxMode->currentIndex();
}

void PuMP_ExportWidget::loadSettings()
{
	watermarkCheckBox2->setChecked(!PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_USEWATERMARK,
		false).toBool());

	setPreview(PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_WATERMARK,
		"").toString());

//	PuMP_MainWindow::settings->setValue(
//		PUMP_EXPORTWIDGET_WATERMARKSIZE,
//		QSize(watermarkSpinBoxW->value(), watermarkSpinBoxH->value()));

	watermarkSpinBoxT->setValue(PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_TRANSPARENCY,
		50).toInt());

	setPos((PuMP_ExportPosition) PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_POSITION,
		BottomRight).toInt());

	QFileInfo f(PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_OUTPUTDIR,
		QDir::homePath()).toString());
	if(f.exists()) outputLineEdit->setText(f.filePath());
	else outputLineEdit->setText(QDir::homePath());

	QSize oSize(PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_OUTPUTSIZE,
		QSize(640, 480)).toSize());
	outputSpinBoxW->setValue(oSize.width());
	outputSpinBoxH->setValue(oSize.height());

	outputComboBoxQuality->setCurrentIndex(PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_OUTPUTQUALITY,
		Smoothed).toInt());

	int index = outputComboBoxFormat->findText(QString("*.jpg"));
	if(index == -1) index = outputComboBoxFormat->findText(QString("*.jpeg"));
	if(index == -1) index = outputComboBoxFormat->findText(QString("*.png"));
	outputComboBoxFormat->setCurrentIndex(PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_OUTPUTFORMAT,
		index).toInt());

	outputComboBoxMode->setCurrentIndex(PuMP_MainWindow::settings->value(
		PUMP_EXPORTWIDGET_OUTPUTMODE, KeepAspectByExpanding).toInt());
}

void PuMP_ExportWidget::storeSettings()
{
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_USEWATERMARK,
		!watermarkCheckBox2->isChecked());
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_WATERMARK,
		watermarkLineEdit->text());
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_WATERMARKSIZE,
		QSize(watermarkSpinBoxW->value(), watermarkSpinBoxH->value()));
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_TRANSPARENCY,
		watermarkSpinBoxT->value());
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_POSITION,
		getPos());
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_OUTPUTDIR,
		outputLineEdit->text());
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_OUTPUTSIZE,
		QSize(outputSpinBoxW->value(), outputSpinBoxH->value()));
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_OUTPUTQUALITY,
		outputComboBoxQuality->currentIndex());
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_OUTPUTFORMAT,
		outputComboBoxFormat->currentIndex());
	PuMP_MainWindow::settings->setValue(
		PUMP_EXPORTWIDGET_OUTPUTMODE,
		outputComboBoxMode->currentIndex());
}

void PuMP_ExportWidget::on_outputPushButton_clicked(bool checked)
{
	Q_UNUSED(checked);
	QString file = QFileDialog::getExistingDirectory(
		this,
		"Select output directory",
		QDir::homePath());
	if(!file.isEmpty()) outputLineEdit->setText(file);
}

void PuMP_ExportWidget::on_watermarkCheckBox2_clicked(bool checked)
{
	label->setEnabled(!checked);
	label1->setEnabled(!checked);
	label2->setEnabled(!checked);
	label3->setEnabled(!checked);
	label4->setEnabled(!checked);
	label5->setEnabled(!checked);
	label6->setEnabled(!checked);
	watermarkRadioButtonC->setEnabled(!checked);
	watermarkRadioButtonBC->setEnabled(!checked);
	watermarkRadioButtonTR->setEnabled(!checked);
	watermarkRadioButtonTL->setEnabled(!checked);
	watermarkRadioButtonTC->setEnabled(!checked);
	watermarkRadioButtonBL->setEnabled(!checked);
	watermarkRadioButtonBR->setEnabled(!checked);
	watermarkLineEdit->setEnabled(!checked);
	watermarkPushButton->setEnabled(!checked);
	watermarkSpinBoxW->setEnabled(!checked);
	watermarkSpinBoxH->setEnabled(!checked);
	watermarkCheckBox1->setEnabled(!checked);
	watermarkPushButtonDefault->setEnabled(!checked);
	watermarkPushButtonDefaultSize->setEnabled(!checked);
	watermarkSpinBoxT->setEnabled(!checked);
	watermarkCheckBox2->setEnabled(true);
}

void PuMP_ExportWidget::on_watermarkPushButton_clicked(bool checked)
{
	Q_UNUSED(checked);
	QString file = QFileDialog::getOpenFileName(
		this,
		"Select watermark",
		QDir::homePath(),
		PuMP_MainWindow::nameFilterString1);
	if(!file.isEmpty()) setPreview(file);
}

void PuMP_ExportWidget::on_watermarkPushButtonDefault_clicked(bool checked)
{
	Q_UNUSED(checked);
	setPreview();
}

void PuMP_ExportWidget::on_watermarkPushButtonDefaultSize_clicked(bool checked)
{
	Q_UNUSED(checked);

	if(!watermark.isNull())
	{
		watermarkScaled = watermark;
		watermarkSpinBoxW->setValue(watermark.width());
		watermarkSpinBoxW_value = watermark.width();
		watermarkSpinBoxH->setValue(watermark.height());
		watermarkSpinBoxH_value = watermark.height();
		
		label1->setMinimumSize(watermark.size());
		label1->setPixmap(watermark);
		label1->updateGeometry();
		updateGeometry();
	}
}

void PuMP_ExportWidget::on_watermarkSpinBoxW_editingFinished()
{
	if(watermarkCheckBox1->isChecked())
	{
		watermarkSpinBoxH->setValue((int)
			(((double) watermarkSpinBoxW->value())
			/ watermarkSpinBoxW_value
			* watermarkSpinBoxH->value()));
	}
	watermarkSpinBoxW_value = watermarkSpinBoxW->value();
	watermarkSpinBoxH_value = watermarkSpinBoxH->value();
	
	if(!watermark.isNull())
	{
		watermarkScaled = watermark;
		watermarkScaled = watermarkScaled.scaled(
			watermarkSpinBoxW_value,
			watermarkSpinBoxH_value,
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);
		label1->setMinimumSize(watermarkScaled.size());
		label1->setPixmap(watermarkScaled);
		label1->updateGeometry();
	}
}

void PuMP_ExportWidget::on_watermarkSpinBoxH_editingFinished()
{
	if(watermarkCheckBox1->isChecked())
	{
		watermarkSpinBoxW->setValue((int)
			(((double) watermarkSpinBoxH->value())
			/ watermarkSpinBoxH_value
			* watermarkSpinBoxW->value()));
	}
	watermarkSpinBoxW_value = watermarkSpinBoxW->value();
	watermarkSpinBoxH_value = watermarkSpinBoxH->value();

	if(!watermark.isNull())
	{
		watermarkScaled = watermark;
		watermarkScaled = watermarkScaled.scaled(
			watermarkSpinBoxW_value,
			watermarkSpinBoxH_value,
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation);
		label1->setMinimumSize(watermarkScaled.size());
		label1->setPixmap(watermarkScaled);
		label1->updateGeometry();
	}
}

/******************************************************************************/

PuMP_ExportDialog::PuMP_ExportDialog(QWidget *parent)
	: QDialog(parent, 0), PuMP_SettingsInterface()
{
	vboxLayout = new QVBoxLayout(this);
	exportWidget = new PuMP_ExportWidget(this);
	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(
		QDialogButtonBox::Cancel |
		QDialogButtonBox::NoButton |
		QDialogButtonBox::Ok);

	vboxLayout->addWidget(exportWidget);
	vboxLayout->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	loadSettings();
}

PuMP_ExportDialog::~PuMP_ExportDialog()
{
	delete vboxLayout;
	delete exportWidget;
	delete buttonBox;
}

void PuMP_ExportDialog::loadSettings()
{
	move(PuMP_MainWindow::settings->value(
		PUMP_EXPORTDIALOG_POS,
		pos()).toPoint());
	resize(PuMP_MainWindow::settings->value(
		PUMP_EXPORTDIALOG_SIZE, size()).toSize());
}

void PuMP_ExportDialog::storeSettings()
{
	PuMP_MainWindow::settings->setValue(PUMP_EXPORTDIALOG_POS, pos());
	PuMP_MainWindow::settings->setValue(PUMP_EXPORTDIALOG_SIZE, size());
	exportWidget->storeSettings(); // perhaps don't want to save every export?
}

void PuMP_ExportDialog::accept()
{
	qDebug() << "accepted";
	storeSettings();
	
	QPixmap watermark;
	int transparency;
	PuMP_ExportPosition position;
	QFileInfo outputDir;
	QSize scaledSize;
	PuMP_ExportQuality quality;
	QString fileFormat;
	PuMP_ExportMode scalingMode;
	exportWidget->getConfig(
		watermark,
		transparency,
		position,
		outputDir,
		scaledSize,
		quality,
		fileFormat,
		scalingMode);
	
	// start new export-thread and go
	
	QDialog::accept();
}

/******************************************************************************/
