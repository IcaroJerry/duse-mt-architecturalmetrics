QT += quick widgets script

include(../../duse-mt-plugin.pri)

SOURCES += jsarchitecturalmetricsplugin.cpp \
    jsarchitecturalmetricsconfig.cpp

HEADERS += jsarchitecturalmetricsplugin.h \
    jsarchitecturalmetricsconfig.h
FORMS += \
    jsarchitecturalmetrics.ui \
    jsarchitecturalmetricsconfig.ui

OTHER_FILES += jsarchitecturalmetrics.json

