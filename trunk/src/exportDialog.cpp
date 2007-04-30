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

/*****************************************************************************/

PuMP_ExportDialog::PuMP_ExportDialog(QWidget *parent) : QDialog(parent, 0)
{
	watermarkGroupBox = new QGroupBox(this);
	watermarkGroupBox->setTitle("Watermark");
	outputGroupBox = new QGroupBox(this);
	outputGroupBox->setTitle("Output");

	watermarkCheckBox = new QCheckBox(watermarkGroupBox);
	watermarkCheckBox->setText("Keep aspect ratio");
	watermarkCheckBox->setChecked(true);
	
	int index = 0;
	outputComboBoxMode = new QComboBox(outputGroupBox);
	outputComboBoxMode->insertItem(0, "keep aspect ratio by expanding");
	outputComboBoxMode->insertItem(0, "keep aspect ratio");
	outputComboBoxMode->insertItem(0, "ignore aspect ratio");
	outputComboBoxFormat = new QComboBox(outputGroupBox);
	outputComboBoxFormat->insertItems(0, PuMP_MainWindow::nameFilters);
	index = outputComboBoxFormat->findText(QString("*.jpg"));
	if(index == -1) index = outputComboBoxFormat->findText(QString("*.jpeg"));
	if(index == -1) index = outputComboBoxFormat->findText(QString("*.png"));
	outputComboBoxFormat->setCurrentIndex(index);
	outputComboBoxQuality = new QComboBox(outputGroupBox);
	outputComboBoxQuality->insertItem(
		0,
		"with smoothing (better quality but slower)");
	outputComboBoxQuality->insertItem(
		0,
		"without smoothing (poor quality but fast)");
	
	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(
		QDialogButtonBox::Cancel |
		QDialogButtonBox::NoButton |
		QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

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
	watermarkLineEdit->setText("/home/tobias/screens/maam_small.png");
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
	watermarkPushButtonDefault = new QPushButton(watermarkGroupBox);
	watermarkPushButtonDefault->setMaximumSize(QSize(30, 16777215));
	watermarkPushButtonDefault->setText("1:1");
	connect(
		watermarkPushButtonDefault,
		SIGNAL(clicked(bool)),
		this,
		SLOT(on_watermarkPushButtonDefault_clicked(bool)));

	watermarkRadioButtonC = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonC->setText("centered");
	watermarkRadioButtonBC = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonBC->setText("bottom center");
	watermarkRadioButtonTR = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonTR->setText("top right");
	watermarkRadioButtonTL = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonTL->setText("top left");
	watermarkRadioButtonTC = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonTC->setText("top center");
	watermarkRadioButtonBL = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonBL->setText("bottom left");
	watermarkRadioButtonBR = new QRadioButton(watermarkGroupBox);
	watermarkRadioButtonBR->setText("bottom right");
	watermarkRadioButtonBR->setChecked(true);

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
	watermarkSpinBoxT->setValue(50);
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
	hboxLayout1->addWidget(watermarkPushButton);
	hboxLayout2->addWidget(label3);
	hboxLayout2->addWidget(watermarkSpinBoxW);
	hboxLayout2->addWidget(label4);
	hboxLayout2->addWidget(watermarkSpinBoxH);
	hboxLayout2->addWidget(watermarkCheckBox);
	hboxLayout2->addWidget(watermarkPushButtonDefault);
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
	vboxLayout->addWidget(buttonBox);

	QSize size(420, 460);
	size = size.expandedTo(minimumSizeHint());
	resize(size);
	
	setPreview(watermarkLineEdit->text());
}

PuMP_ExportDialog::~PuMP_ExportDialog()
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

void PuMP_ExportDialog::setPreview(const QString &path)
{
	QFileInfo info(path);
	if(info.exists())
	{
		watermark.load(info.filePath());
		if(!watermark.isNull())
		{
			watermarkLineEdit->setText(path);
			watermarkSpinBoxW->setValue(watermark.width());
			watermarkSpinBoxW_value = watermark.width();
			watermarkSpinBoxH->setValue(watermark.height());
			watermarkSpinBoxH_value = watermark.height();

			label1->setMinimumSize(watermark.size());
			label1->setPixmap(watermark);
			label1->updateGeometry();
		}
	}
}

void PuMP_ExportDialog::on_outputPushButton_clicked(bool checked)
{
	Q_UNUSED(checked);
}

void PuMP_ExportDialog::on_watermarkPushButton_clicked(bool checked)
{
	Q_UNUSED(checked);
	QString file = QFileDialog::getOpenFileName(
		this,
		"Select watermark",
		QDir::homePath(),
		PuMP_MainWindow::nameFilterString2);
	if(!file.isEmpty()) setPreview(file);
}

void PuMP_ExportDialog::on_watermarkPushButtonDefault_clicked(bool checked)
{
	Q_UNUSED(checked);

	if(!watermark.isNull())
	{
		watermarkSpinBoxW->setValue(watermark.width());
		watermarkSpinBoxW_value = watermark.width();
		watermarkSpinBoxH->setValue(watermark.height());
		watermarkSpinBoxH_value = watermark.height();
		
		label1->setMinimumSize(watermark.size());
		label1->setPixmap(watermark);
		label1->updateGeometry();
	}
}

void PuMP_ExportDialog::on_watermarkSpinBoxW_editingFinished()
{
	if(watermarkCheckBox->isChecked())
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

void PuMP_ExportDialog::on_watermarkSpinBoxH_editingFinished()
{
	if(watermarkCheckBox->isChecked())
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

/*****************************************************************************/
