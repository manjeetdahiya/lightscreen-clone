TEMPLATE = app
TARGET = lightscreen
HEADERS += updater/updater.h \
    dialogs/areaselector.h \
    engines/screenshotengine.h \
    dialogs/optionsdialog.h \
    dialogs/aboutdialog.h \
    widgets/hotkeywidget.h \
    osspecific.h \
    lightscreenwindow.h
SOURCES += updater/updater.cpp \
    dialogs/areaselector.cpp \
    engines/screenshotengine.cpp \
    dialogs/optionsdialog.cpp \
    dialogs/aboutdialog.cpp \
    widgets/hotkeywidget.cpp \
    main.cpp \
    lightscreenwindow.cpp
FORMS += dialogs/optionsdialog.ui \
    dialogs/aboutdialog.ui \
    lightscreenwindow.ui
RESOURCES += lightscreen.qrc
TRANSLATIONS += translations/lightscreen_es.ts \
    translations/lightscreen_pl.ts
RC_FILE += lightscreen.rc
CODECFORSRC = UTF-8
QT += network \
    core \
    gui
win32:LIBS += libgdi32
include($$PWD/tools/globalshortcut/globalshortcut.pri)
