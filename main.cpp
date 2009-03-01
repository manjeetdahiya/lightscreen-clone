#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QSettings>

#include "tools/os.h"
#include "lightscreenwindow.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setOrganizationName("config");
  a.setApplicationName ("lightscreen");
  a.setApplicationVersion("1.0-dev");
  a.setQuitOnLastWindowClosed(false);

  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QApplication::applicationDirPath());
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope  , QApplication::applicationDirPath());

  LightscreenWindow w;

  if (a.arguments().size() == 1)
    w.show();

  QObject::connect(&w, SIGNAL(accepted()), &a, SLOT(quit()));
  QObject::connect(&w, SIGNAL(rejected()), &a, SLOT(quit()));

  return a.exec();
}

