# Platform independent project-file for "Publish My Pictures"
# note: You need qt4-qmake version 4.2 or higher to build this project-file!

# output directory
DESTDIR = ./

include(res/qmake.in)
include(src/qmake.in)
include(src/zip/qmake.in)
include(src/zlib/qmake.in)

# built application not lib
TEMPLATE = app

# use qt-lib
CONFIG += qt release

win32{ DEFINES += WINDOWS }
