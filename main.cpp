#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QSettings>

#include "tools/os.h"
#include "tools/singleapplication/dsingleapplication.h"

#include "lightscreenwindow.h"

#include <stdio.h>
#include <stdlib.h>

void myMessageOutput(QtMsgType type, const char *msg)
{ // Message output to print everything into stdout, so Windows can catch it.
  //Comment out debug ouput for release build
  switch (type)
  {
  case QtDebugMsg:
    fprintf(stdout, "Debug: %s\n", msg);
    break;
  case QtWarningMsg:
    fprintf(stdout, "Warning: %s\n", msg);
    break;
  case QtCriticalMsg:
    fprintf(stdout, "Critical: %s\n", msg);
    break;
  case QtFatalMsg:
    fprintf(stdout, "Fatal: %s\n", msg);
    abort();
  }
}

int main(int argc, char *argv[])
{
  qInstallMsgHandler(myMessageOutput);

  QApplication a(argc, argv);
  a.setOrganizationName("config");
  a.setApplicationName ("lightscreen");
  a.setApplicationVersion("0.6-dev");
  a.setQuitOnLastWindowClosed(false);

  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QApplication::applicationDirPath());
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope  , QApplication::applicationDirPath());


  DSingleApplication instance("Lightscreen");

  if (instance.isRunning())
  {
    instance.sendMessage("New instance");
    return 0;
  }

  // Localization
  QString language = QSettings().value("options/language").toString().toLower();

  os::translate(language);


  LightscreenWindow w;

  if (a.arguments().size() == 1)
    w.show();


  QObject::connect(&instance, SIGNAL( messageReceived(QString) ), &w, SLOT(instanceMessage(QString)));
  QObject::connect(&w, SIGNAL(accepted()), &a, SLOT(quit()));
  QObject::connect(&w, SIGNAL(rejected()), &a, SLOT(quit()));

  return a.exec();
}

