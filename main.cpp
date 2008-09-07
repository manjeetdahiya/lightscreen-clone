#include <QApplication>
#include <QDesktopWidget>
#include <QLocale>
#include <QImageWriter>
#include <QAbstractEventDispatcher>
#include <QTranslator>
#include <QSettings>

#include "osspecific.h"
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
  a.setOrganizationName("K");
  a.setApplicationName("Lightscreen");
  a.setApplicationVersion("0.6-dev");
  a.setQuitOnLastWindowClosed(false);

  OS::singleInstance("LightscreenN");

  // Localization
  QString language = QSettings().value("options/language").toString().toLower();

  if (!language.isEmpty())
  {
    QTranslator *translator = new QTranslator;
    language.append(".qm");
    translator->load(QCoreApplication::applicationDirPath()+QDir::separator()+"lang"+QDir::separator()+language);
    qApp->installTranslator(translator);
  }

  LightscreenWindow w;

  if (a.arguments().size() == 1)
    w.show();

  QObject::connect(&w, SIGNAL(accepted()), &a, SLOT(quit()));
  QObject::connect(&w, SIGNAL(rejected()), &a, SLOT(quit()));

  return a.exec();
}

