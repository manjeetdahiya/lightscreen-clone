TEMPLATE = app
TARGET = lightscreen
HEADERS += tools/os.h \
    updater/updater.h \
    dialogs/areaselector.h \
    dialogs/optionsdialog.h \
    dialogs/aboutdialog.h \
    widgets/hotkeywidget.h \
    lightscreenwindow.h \
    tools/screenshot.h
SOURCES += tools/os.cpp \
    updater/updater.cpp \
    dialogs/areaselector.cpp \
    dialogs/optionsdialog.cpp \
    dialogs/aboutdialog.cpp \
    widgets/hotkeywidget.cpp \
    main.cpp \
    lightscreenwindow.cpp \
    tools/screenshot.cpp
FORMS += dialogs/optionsdialog.ui \
    dialogs/aboutdialog.ui \
    lightscreenwindow.ui
RESOURCES += lightscreen.qrc
TRANSLATIONS += translations/spanish.ts \
    translations/polish.ts \
    translations/untranslated.ts
RC_FILE += lightscreen.rc
CODECFORSRC = UTF-8
QT += network \
    core \
    gui
win32:LIBS += libgdi32
include($$PWD/tools/globalshortcut/globalshortcut.pri)
