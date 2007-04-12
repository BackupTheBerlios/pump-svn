# Platform independent project-file for "Publish My Pictures"
# note: You need qt4-qmake version 4.2 or higher to build this project-file!

# output directory
DESTDIR = ./

# designer-gui-form
#FORMS = ui/

HEADERS = \
	src/directoryView.hh \
	src/imageView.hh \
	src/mainWindow.hh \
	src/overview.hh \
	src/tabView.hh \
	src/export.hh \
	src/configDialog.hh \
	src/configPages.hh \
	src/zlib/zlib.h
	
SOURCES = \
	src/directoryView.cpp \
	src/imageView.cpp \
	src/main.cpp \
	src/mainWindow.cpp \
	src/overview.cpp \
	src/tabView.cpp \
	src/export.cpp \
	src/configDialog.cpp \
	src/configPages.cpp

# built application not lib
TEMPLATE = app

# built in resource-files
RESOURCES = res/PuMP.qrc

# use qt-lib
CONFIG += qt release

# defines for win32 builds
#win32{ RC_FILE = res/PuMP.rc }

# compression libraries
LIBS += -Lsrc/zlib -lz
LIBS += -Lsrc/zip -lzip

INCLUDEPATH += \
    src/zlib \
    src/zip


