#-------------------------------------------------
#
# Project created by QtCreator 2013-10-25T17:38:51
#
#-------------------------------------------------

QT += widgets quick modeling modelingwidgets script uml duse

include(../../duse-mt-plugin.pri)

SOURCES += designspaceexplorerplugin.cpp \
           newdusedesigndialog.cpp

HEADERS += designspaceexplorerplugin.h \
           newdusedesigndialog.h

FORMS += newdusedesigndialog.ui

RESOURCES += qml/designspaceexplorer.qrc

OTHER_FILES += designspaceexplorer.json
