#include "configPages.hh"

BasicConfiguration::BasicConfiguration(QWidget *parent)
	:QWidget(parent)
{
	qDebug() << "BasicConfiguration";
	QLabel *ExportLabel = new QLabel(tr("Exportformat"));
	QComboBox *exportCombo = new QComboBox;
	exportCombo->addItem(tr("Scaled Images without watermark"));
	exportCombo->addItem(tr("Scaled Images with watermark"));
	exportCombo->addItem(tr("Basic HTML gallery"));
	exportCombo->addItem(tr("Advanced HTML gallery"));
	exportCombo->addItem(tr("Big/Small as .zip archive"));
	QHBoxLayout *basicExportLayout = new QHBoxLayout;
	basicExportLayout->addWidget(ExportLabel);
	basicExportLayout->addSpacing(10);
	basicExportLayout->addWidget(exportCombo);
	
	QLabel *DirLabel = new QLabel(tr("Current Directory"));
	
	QHBoxLayout *DirStatusLayout = new QHBoxLayout;
	DirStatusLayout->addWidget(DirLabel);
	DirStatusLayout->addSpacing(10);

	
	QPushButton *exportDirectoryButton =
		new QPushButton(tr("Select export directory..."));
	connect(exportDirectoryButton, SIGNAL(clicked()), this, SLOT(setExportPath()));
		
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addItem(basicExportLayout);
	mainLayout->addItem(DirStatusLayout);
	mainLayout->addWidget(exportDirectoryButton);
	
	setLayout(mainLayout);
	
}

void BasicConfiguration::setExportPath()
{
//	QFileDialog exportPathDialog(this);
//	exportPathDialog.setFileMode(QFileDialog::DirectoryOnly);
//	exportPathDialog.setViewMode(QFileDialog::Detail);
//	QStringList fileNames;
//	if (exportPathDialog.exec())
//		fileNames = exportPathDialog.selectedFiles();
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
					"/home",
					QFileDialog::ShowDirsOnly
					|QFileDialog::DontResolveSymlinks
			   );

	qDebug() << "Selected directory: " << dir;
}

void BasicConfiguration::currentExportPath()
{
	/* read from config, otherwise $HOME */
	qDebug() << "BasicConfigration::currentExportPath()";
	
};

ExportConfiguration::ExportConfiguration(QWidget *parent)
	:QWidget(parent)
{
	qDebug() << "ExportConfiguration";	
}

AdvancedConfiguration::AdvancedConfiguration(QWidget *parent)
	:QWidget(parent)
{
	qDebug() << "AdvancedConfiguration";
}

AboutPage::AboutPage(QWidget *parent)
	:QWidget(parent)
{
	qDebug() << "AboutPage()";
}
