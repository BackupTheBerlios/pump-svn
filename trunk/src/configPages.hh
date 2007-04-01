#ifndef CONFIGPAGES_HH_
#define CONFIGPAGES_HH_

#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include <QDebug>

class BasicConfiguration : public QWidget
{	
Q_OBJECT
public:
	BasicConfiguration(QWidget *parent=0);
	
public slots:
	void setExportPath(void);
	void currentExportPath(void);
};


class ExportConfiguration : public QWidget
{
public:
	ExportConfiguration(QWidget *parent=0);
};


class AdvancedConfiguration : public QWidget
{
public:
	AdvancedConfiguration(QWidget *parent=0);
};


class AboutPage : public QWidget
{
public:
	AboutPage(QWidget *parent=0);	
};

#endif /*CONFIGPAGES_HH_*/

