TEMPLATE = app
TARGET = lightscreen
HEADERS += tools/os.h \
    updater/updater.h \
    dialogs/areaselector.h \
    dialogs/optionsdialog.h \
    widgets/hotkeywidget.h \
    lightscreenwindow.h \
    tools/screenshot.h \
    dialogs/previewdialog.h \
    tools/screenshotmanager.h \
    tools/windowpicker.h \
    tools/qtwin.h \
    dialogs/updaterdialog.h \
    dialogs/screenshotdialog.h
SOURCES += tools/os.cpp \
    updater/updater.cpp \
    dialogs/areaselector.cpp \
    dialogs/optionsdialog.cpp \
    widgets/hotkeywidget.cpp \
    main.cpp \
    lightscreenwindow.cpp \
    tools/screenshot.cpp \
    dialogs/previewdialog.cpp \
    tools/screenshotmanager.cpp \
    tools/windowpicker.cpp \
    tools/qtwin.cpp \
    dialogs/updaterdialog.cpp \
    dialogs/screenshotdialog.cpp
FORMS += dialogs/optionsdialog.ui \
    dialogs/aboutdialog.ui \
    lightscreenwindow.ui
RESOURCES += lightscreen.qrc
TRANSLATIONS += translations/spanish.ts \
    translations/polish.ts \
    translations/untranslated.ts
RC_FILE += lightscreen.rc
CODECFORSRC = UTF-8
LIBS += libgcc
QT += network \
    core \
    gui
win32:LIBS += libgdi32
include($$PWD/tools/globalshortcut/globalshortcut.pri)
OTHER_FILES += TODO.txt \
    TODO.txt
